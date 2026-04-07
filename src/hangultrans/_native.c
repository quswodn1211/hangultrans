#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include "hangul.h"

/*
Python에서:
    hangultrans.h("ㄱ 가 ㅏ 각")

반환:
    bytes
예:
    b'\x01\x00\x00\x00\x00\x00\x01\x01\x00...'
*/

static PyObject *py_h(PyObject *self, PyObject *args) {
    const char *input = NULL;

    if (!PyArg_ParseTuple(args, "s", &input)) {
        return NULL;
    }

    HangulResult r = h(input);

    if (!r.ok) {
        PyErr_SetString(
            PyExc_ValueError,
            "input must contain only supported Hangul syllables, compatibility jamo, or spaces"
        );
        return NULL;
    }

    Py_ssize_t size = (Py_ssize_t)(r.char_count * 3);

    PyObject *result = PyBytes_FromStringAndSize((const char *)r.data, size);

    free_hangul_result(&r);

    return result;
}

/* bytes를 16진수 문자열로 보기 쉽게 바꿔주는 보조 함수 */
static PyObject *py_h_hex(PyObject *self, PyObject *args) {
    const char *input = NULL;

    if (!PyArg_ParseTuple(args, "s", &input)) {
        return NULL;
    }

    HangulResult r = h(input);

    if (!r.ok) {
        PyErr_SetString(
            PyExc_ValueError,
            "input must contain only supported Hangul syllables, compatibility jamo, or spaces"
        );
        return NULL;
    }

    PyObject *parts = PyList_New(0);
    if (!parts) {
        free_hangul_result(&r);
        return NULL;
    }

    for (size_t i = 0; i < r.char_count; i++) {
        char buf[16];
#ifdef _MSC_VER
        _snprintf(buf, sizeof(buf), "%02X %02X %02X",
                  r.data[i * 3 + 0],
                  r.data[i * 3 + 1],
                  r.data[i * 3 + 2]);
#else
        snprintf(buf, sizeof(buf), "%02X %02X %02X",
                 r.data[i * 3 + 0],
                 r.data[i * 3 + 1],
                 r.data[i * 3 + 2]);
#endif
        PyObject *item = PyUnicode_FromString(buf);
        if (!item) {
            Py_DECREF(parts);
            free_hangul_result(&r);
            return NULL;
        }
        PyList_Append(parts, item);
        Py_DECREF(item);
    }

    free_hangul_result(&r);
    return parts;
}

static PyMethodDef NativeMethods[] = {
    {"h", py_h, METH_VARARGS, "Encode Hangul text into 3-byte triples and return bytes."},
    {"h_hex", py_h_hex, METH_VARARGS, "Encode Hangul text and return ['XX XX XX', ...]."},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef nativemodule = {
    PyModuleDef_HEAD_INIT,
    "_native",
    "Hangul native extension",
    -1,
    NativeMethods
};

PyMODINIT_FUNC PyInit__native(void) {
    return PyModule_Create(&nativemodule);
}
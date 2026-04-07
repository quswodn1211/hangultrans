#ifndef HANGUL_H
#define HANGUL_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int ok;
    size_t char_count;
    uint8_t *data;   /* 길이 = char_count * 3 */
} HangulResult;

HangulResult h(const char *s);
void free_hangul_result(HangulResult *r);

const char *cho_to_string(uint8_t code);
const char *jung_to_string(uint8_t code);
const char *jong_to_string(uint8_t code);

#endif
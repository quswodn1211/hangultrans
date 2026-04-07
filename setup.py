from setuptools import setup, Extension, find_packages
import sysconfig
import os

BASE_DIR = os.path.abspath(os.path.dirname(__file__))
python_include = sysconfig.get_paths()["include"]

ext_modules = [
    Extension(
        "hangultrans._native",
        sources=[
            "src/hangultrans/_native.c",
            "csrc/hangul.c",
        ],
        include_dirs=[
            os.path.join(BASE_DIR, "include"),
            python_include,
        ],
    )
]

setup(
    name="hangultrans_bjw",
    version="0.1.0",
    package_dir={"": "src"},
    packages=find_packages(where="src"),
    ext_modules=ext_modules,
)
from setuptools import setup, Extension, find_packages

ext_modules = [
    Extension(
        "mypkg._native",
        sources=[
            "src/mypkg/_native.c",
            "csrc/core.c",
            "csrc/util.c",
        ],
        include_dirs=["include"],
    )
]

setup(
    name="hangultrans",
    version="0.1.0",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    ext_modules=ext_modules,
)
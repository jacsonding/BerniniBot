from distutils.core import setup, Extension
setup(name='airpen', version='1.0',  \
      ext_modules=[Extension('airpen', ['airpen.c'])])
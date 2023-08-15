# anotherclibrary v1.0

`anotherclibrary` is a C library that provides various functionalities. This document provides a guide on how to build and use the library.

## Dependencies

- [libuv (version 1.35 or newer)](https://libuv.org)
- [zlib](https://www.zlib.net)
- a C compiler

Please ensure these dependencies are installed on your system before building or installing `anotherclibrary`.

## Building

To build `anotherclibrary`, follow these steps:

```bash
mkdir build
cd build
./configure
make 
make install
```


## Configure Options

* `--enable-debug`<br/>
    This will enable debug mode and should be used when specifying enable-address-sanitizer and the memory check options.
* `--enable-address-sanitizer`<br/>
    The Address Sanitizer is specific to Mac OSX and can be used to find memory errors.
* `--with-memory-check`<br/>
    AnotherCLibrary can check memory that has been allocated / freed with ac_alloc and ac_free functions.  It can find memory leaks, invalid free(s), and double free(s) without sacrificing performance.  This option will report errors to the terminal.
* `--with-memory-check-file=filename`<br/>
    Similar to the prior option, except that instead of reporting to the terminal, the memory information is stored in the file (based upon filename).  The with-memory-check option only reports on memory leaks at the end of the program.  This option will output memory usage every 2 minutes.  It saves previous versions by rotating the log.


## Development

4. **Generate the Configuration Script**: Run the following command to generate the configuration script:
   ```bash
   autoreconf --install
   ```


## Contact

For any inquiries, suggestions, or issues, please contact the maintainer at [contactandyc@gmail.com](mailto:contactandyc@gmail.com).

## License

Please see the `LICENSE` file for information on how this library is licensed.
```

Make sure to customize this README with additional information specific to your project, such as its features, how to use the library, code examples, etc.
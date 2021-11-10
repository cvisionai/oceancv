## Synopsis

oceancv focusses on computer vision algorithms for underwater image analysis.
This includes image processing, pattern recognition, machine learning and geometric algorithms but also functionality for navigation.

## Motivation

oceancv bundles marine image analysis functionality. Most of the algorithms are related to publications in scientific journals.

## Building

oceancv uses cmake. So follow the usual: mkdir build && cd build && cmake ..

You can enable / disable the following build targets:
* WITH_MPEG7 : Build MPEG7 feature extraction library depencendy
* WITH_CUDA : Build with OpenCV GPU functionality
* WITH_TESTS : Build tests (uses Google Test)
* WITH_EXAMPLES : Build examples
* WITH_BIN : Build binary tools
* WITH_DOC : Build doxygen documentation

By default, all targets are being built. And as always, default install path is /usr/local, but you can control this using cmake -DCMAKE_INSTALL_PREFIX=/target/path ..

## Contributors

Dr. Timm Schoening

tschoening [at] geomar [dot] de

DeepSea Monitoring Group

GEOMAR Helmholtz-Centre for Ocean Research Kiel

## Dependencies

* OpenCV (required, ideally with CUDA)
* jsoncpp (required)
* curl (required)
* MPEG7FexLib (optional)
* GTest (optional, for tests)
* doxygen (optional, for documentation)

## License

Copyright (c) 2020 GEOMAR Helmholtz-Centre for Ocean Research Kiel, Dr. Timm Schoening

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

- When using the software to publish scientific literature, appropriate citation of the software is mandatory.
- Use of the software needs to be reported to the providers of the software
- The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

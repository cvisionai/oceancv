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


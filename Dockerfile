# Based on the dockerfile found here:
# https://github.com/JulianAssmann/opencv-cuda-docker/blob/9beb361e8ad563e145fefce4e8dac9e520c0389d/ubuntu-20.04/opencv-4.5.3/cuda-11.4.1/Dockerfile
FROM nvidia/cuda:11.8.0-devel-ubuntu20.04

ARG DEBIAN_FRONTEND=noninteractive

# Which version of OpenCV to install
ARG OPENCV_VERSION=4.7.0

# ON enables CUDA for OpenCV and OceanCV
ARG WITH_CUDA=ON

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y python3-pip build-essential cmake git wget unzip yasm pkg-config \
        libswscale-dev libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libavformat-dev \
        libpq-dev libxine2-dev libglew-dev libtiff5-dev zlib1g-dev libjpeg-dev libavcodec-dev \
        libavformat-dev libavutil-dev libpostproc-dev libswscale-dev libeigen3-dev libtbb-dev \
        libgtk2.0-dev pkg-config python3-dev python3-numpy libjsoncpp-dev libcurl4-openssl-dev \
        g++-8 doxygen libgtest-dev graphviz vim && rm -rf /var/lib/apt/lists/*

RUN cd /opt && wget https://github.com/opencv/opencv/archive/$OPENCV_VERSION.zip && \
    unzip $OPENCV_VERSION.zip && rm $OPENCV_VERSION.zip && \
    wget https://github.com/opencv/opencv_contrib/archive/$OPENCV_VERSION.zip && \
    unzip ${OPENCV_VERSION}.zip && rm ${OPENCV_VERSION}.zip && \
    mkdir /opt/opencv-${OPENCV_VERSION}/build && cd /opt/opencv-${OPENCV_VERSION}/build && \
    cmake -DOPENCV_EXTRA_MODULES_PATH=/opt/opencv_contrib-${OPENCV_VERSION}/modules \
        -DWITH_CUDA=$WITH_CUDA -DCUDA_ARCH_BIN=7.5,8.0,8.6 -DCMAKE_BUILD_TYPE=RELEASE \
        -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j"$(nproc)" && make install && ldconfig && rm -rf /opt/opencv-${OPENCV_VERSION} && \
    rm -rf /opt/opencv_contrib-${OPENCV_VERSION}

# Build target enables/disables for OceanCV
ARG WITH_MPEG7=ON
ARG WITH_TESTS=ON
ARG WITH_EXAMPLES=ON
ARG WITH_BIN=ON
ARG WITH_DOC=ON

RUN mkdir /data
RUN mkdir -p /opt/oceancv
COPY . /opt/oceancv
RUN cd /opt/oceancv && mkdir build && cd build && \
    cmake -DWITH_TESTS=$WITH_TESTS -DWITH_EXAMPLES=$WITH_EXAMPLES -DWITH_DOC=$WITH_DOC \
    -DWITH_MPEG7=$WITH_MPEG7 -DWITH_CUDA=$WITH_CUDA -DWITH_BIN=$WITH_BIN \
    -DCMAKE_MODULE_PATH=/usr/local/lib/cmake/opencv4/ -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    make -j"$(nproc)" && make install && rm -rf /opt/oceancv

ENV PATH=$PATH:/usr/local/bin/oceancv
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

CMD ["/bin/bash"]

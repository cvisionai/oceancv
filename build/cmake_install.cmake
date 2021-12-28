# Install script for directory: /Users/tschoening/dev/repos/oceancv

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/tschoening/dev/repos/oceancv/build/libMPEG7.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMPEG7.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMPEG7.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMPEG7.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/MPEG7FexLib" TYPE FILE FILES
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/AddressLib/address.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/AddressLib/inter.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/AddressLib/intra.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/AddressLib/momusys.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/AddressLib/nhood.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/AddressLib/vopio.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Convert.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/ColorLayout.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/ColorQuant.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/ColorSpace.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/ColorStructure.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/ContourShape.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/DominantColor.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/EdgeHist.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/FaceRecognition.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/HomoTexture.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/RegionShape.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Descriptors/ScalableColor.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/ColorLayoutExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/ColorSpaceExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/ColorStructureExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/ContourShapeExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/DominantColorExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/EdgeHistExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/EigenFaceData.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/FaceRecognitionExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/GoFGoPColorExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/HomoTextureExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/RegionShapeExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/ScalableColorExtraction.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/homo_texture_extractor.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Extraction/imgutil.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Feature.h"
    "/Users/tschoening/dev/repos/oceancv/dep/MPEG7FexLib/Frame.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/tschoening/dev/repos/oceancv/build/liboceancv_util.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_util.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_util.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_util.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_util.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/oceancv/util" TYPE FILE FILES
    "/Users/tschoening/dev/repos/oceancv/util/cli_arg_provenance.h"
    "/Users/tschoening/dev/repos/oceancv/util/cli_args.h"
    "/Users/tschoening/dev/repos/oceancv/util/data_file_structure.h"
    "/Users/tschoening/dev/repos/oceancv/util/file_parser.h"
    "/Users/tschoening/dev/repos/oceancv/util/file_system.h"
    "/Users/tschoening/dev/repos/oceancv/util/geo_coordinates.h"
    "/Users/tschoening/dev/repos/oceancv/util/json_value.h"
    "/Users/tschoening/dev/repos/oceancv/util/load_remote_image.h"
    "/Users/tschoening/dev/repos/oceancv/util/misc_func.h"
    "/Users/tschoening/dev/repos/oceancv/util/progress_bar.h"
    "/Users/tschoening/dev/repos/oceancv/util/string_func.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/tschoening/dev/repos/oceancv/build/liboceancv_ml.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_ml.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_ml.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_ml.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_ml.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_ml.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/oceancv/ml" TYPE FILE FILES
    "/Users/tschoening/dev/repos/oceancv/ml/aggregation.h"
    "/Users/tschoening/dev/repos/oceancv/ml/cluster_indices.h"
    "/Users/tschoening/dev/repos/oceancv/ml/color_descriptors.h"
    "/Users/tschoening/dev/repos/oceancv/ml/feature_descriptor.h"
    "/Users/tschoening/dev/repos/oceancv/ml/genetic_algorithm.h"
    "/Users/tschoening/dev/repos/oceancv/ml/h2som.h"
    "/Users/tschoening/dev/repos/oceancv/ml/learn_rate.h"
    "/Users/tschoening/dev/repos/oceancv/ml/linear_regression.h"
    "/Users/tschoening/dev/repos/oceancv/ml/mat_algorithms.h"
    "/Users/tschoening/dev/repos/oceancv/ml/mat_pair.h"
    "/Users/tschoening/dev/repos/oceancv/ml/mat_pair_algorithms.h"
    "/Users/tschoening/dev/repos/oceancv/ml/metric.h"
    "/Users/tschoening/dev/repos/oceancv/ml/mpeg7_descriptors.h"
    "/Users/tschoening/dev/repos/oceancv/ml/neural_gas.h"
    "/Users/tschoening/dev/repos/oceancv/ml/vec_algorithms.h"
    "/Users/tschoening/dev/repos/oceancv/ml/vec_pair.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/tschoening/dev/repos/oceancv/build/liboceancv_img.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_img.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_img.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_img.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_img.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/oceancv/img" TYPE FILE FILES
    "/Users/tschoening/dev/repos/oceancv/img/color_constancy.h"
    "/Users/tschoening/dev/repos/oceancv/img/fspice_normalization.h"
    "/Users/tschoening/dev/repos/oceancv/img/image_entropy.h"
    "/Users/tschoening/dev/repos/oceancv/img/pixel_blob.h"
    "/Users/tschoening/dev/repos/oceancv/img/statistical_filters.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/tschoening/dev/repos/oceancv/build/liboceancv_cudaimg.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_cudaimg.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_cudaimg.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_cudaimg.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/liboceancv_cudaimg.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/oceancv/cudaimg" TYPE FILE FILES
    "/Users/tschoening/dev/repos/oceancv/cudaimg/fspice_normalization.h"
    "/Users/tschoening/dev/repos/oceancv/cudaimg/gabor_filter.h"
    "/Users/tschoening/dev/repos/oceancv/cudaimg/gauss_smoothing.h"
    "/Users/tschoening/dev/repos/oceancv/cudaimg/image_entropy.h"
    "/Users/tschoening/dev/repos/oceancv/cudaimg/misc_func.h"
    "/Users/tschoening/dev/repos/oceancv/cudaimg/zernike_filter.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/descriptors")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/descriptors" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/descriptors")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/descriptors")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/descriptors")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/descriptors")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/h2som")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/h2som" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/h2som")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/h2som")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/h2som")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/h2som")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/linearregression")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/linearregression" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/linearregression")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/linearregression")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/linearregression")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/linearregression")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/statistical_descriptors")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/statistical_descriptors" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/statistical_descriptors")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/statistical_descriptors")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/statistical_descriptors")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/statistical_descriptors")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/zernike")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/zernike" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/zernike")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/zernike")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/zernike")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/zernike")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/laserpoints")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/laserpoints" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/laserpoints")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/laserpoints")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/laserpoints")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/examples/laserpoints")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/examples" TYPE FILE FILES "/Users/tschoening/dev/repos/oceancv/examples/Lenna.png")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/asciDataCorrelation")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/asciDataCorrelation" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/asciDataCorrelation")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/asciDataCorrelation")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/asciDataCorrelation")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/asciDataCorrelation")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/openCVBuildConfig")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/openCVBuildConfig" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/openCVBuildConfig")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/openCVBuildConfig")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/openCVBuildConfig")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/runCoMoNoD")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runCoMoNoD" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runCoMoNoD")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runCoMoNoD")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runCoMoNoD")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runCoMoNoD")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/runDeLPHI")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runDeLPHI" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runDeLPHI")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runDeLPHI")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runDeLPHI")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runDeLPHI")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/runMediaFeatureExtraction")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runMediaFeatureExtraction" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runMediaFeatureExtraction")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runMediaFeatureExtraction")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runMediaFeatureExtraction")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runMediaFeatureExtraction")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/runOlimp")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runOlimp" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runOlimp")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runOlimp")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runOlimp")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/runOlimp")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/trainDeLPHI")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/trainDeLPHI" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/trainDeLPHI")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/trainDeLPHI")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/trainDeLPHI")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/trainDeLPHI")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/oceancv" TYPE EXECUTABLE FILES "/Users/tschoening/dev/repos/oceancv/build/videoTiledClustering")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/videoTiledClustering" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/videoTiledClustering")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/tschoening/dev/repos/oceancv/build"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/videoTiledClustering")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/usr/local/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/videoTiledClustering")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/oceancv/videoTiledClustering")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/tschoening/dev/repos/oceancv/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")

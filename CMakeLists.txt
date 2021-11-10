cmake_minimum_required(VERSION 3.10)
project(oceancv)

set(CMAKE_C_COMPILER "gcc-8")
set(CMAKE_CXX_COMPILER "g++-8")


# Version number
set(oceancv_VERSION_MAJOR 1)
set(oceancv_VERSION_MINOR 1)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++-8")

find_package(jsoncpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CURL REQUIRED)

SET(WITH_MPEG7 ON CACHE BOOL "Build MPEG7 feature extraction library dependency")
SET(WITH_CUDA ON CACHE BOOL "Build with OpenCV GPU functionality")
SET(WITH_TESTS ON CACHE BOOL "Build tests (uses Google Test)")
SET(WITH_EXAMPLES ON CACHE BOOL "Build examples")
SET(WITH_BIN ON CACHE BOOL "Build binary tools")
SET(WITH_DOC ON CACHE BOOL "Build doxygen documentation")

if(WITH_MPEG7)

	# Get all source files and remove hidden ones
	file(GLOB SOURCES "dep/MPEG7FexLib/*/*.cpp" "dep/MPEG7FexLib/*/*.c" "dep/MPEG7FexLib/*.cpp")
	list(FILTER SOURCES EXCLUDE REGEX "\\/\\.")

	include_directories(dep)
	add_library(MPEG7 SHARED ${SOURCES})

	file(GLOB HEADERS "dep/MPEG7FexLib/*/*.h" dep/MPEG7FexLib/*.h)
	list(FILTER HEADERS EXCLUDE REGEX "\\/\\.")

	install(TARGETS MPEG7 DESTINATION lib)
	install(FILES ${HEADERS} DESTINATION include/MPEG7FexLib)

endif(WITH_MPEG7)



set(BUILD_DIRS util ml img)
if(WITH_CUDA)
	list(APPEND BUILD_DIRS cudaimg)
endif(WITH_CUDA)

set(DEP_ml MPEG7 oceancv_img opencv_core opencv_imgproc)
set(DEP_util curl jsoncpp opencv_imgproc opencv_core opencv_imgcodecs)
set(DEP_img opencv_core opencv_imgproc)
set(DEP_cudaimg opencv_core)

foreach(DIR IN LISTS BUILD_DIRS)

	file(GLOB SOURCES "${DIR}/*.cpp")
	list(FILTER SOURCES EXCLUDE REGEX "\\/\\.")

	file(GLOB HEADERS "${DIR}/*.h")
	list(FILTER HEADERS EXCLUDE REGEX "\\/\\.")

	# Remove mpeg7_descriptors.cpp if MPEG is not available
	if(${DIR} STREQUAL "ml" AND NOT WITH_MPEG7)
		list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/mpeg7_descriptors.cpp")
		list(REMOVE_ITEM HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/mpeg7_descriptors.h")
	endif(${DIR} STREQUAL "ml" AND NOT WITH_MPEG7)

	include_directories(..)
	add_library(oceancv_${DIR} SHARED ${SOURCES})
	target_link_libraries(oceancv_${DIR} ${DEP_${DIR}})

	install(TARGETS oceancv_${DIR} DESTINATION lib)
	install(FILES ${HEADERS} DESTINATION include/oceancv/${DIR})

	# Create Findoceancv.cmake
	execute_process(COMMAND echo "set(oceancv_FOUND TRUE)\nset(oceancv_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)\nset(oceancv_LIBRARIES ${CMAKE_INSTALL_PREFIX}/lib)" OUTPUT_FILE oceancvConfig.cmake)

endforeach(DIR)



if(WITH_TESTS)

	find_package(GTest)
	if(GTEST_FOUND)
		add_executable(run_tests "test/run_test.cpp")
		target_link_libraries(run_tests ${GTEST_LIBRARIES} oceancv_ml oceancv_util pthread opencv_core opencv_imgcodecs)
		install(TARGETS run_tests RUNTIME DESTINATION bin/oceancv/tests)
	endif(GTEST_FOUND)

endif(WITH_TESTS)



if(WITH_EXAMPLES)

	# Dependencies
	set(DEP_descriptors opencv_imgcodecs opencv_imgproc opencv_core oceancv_util oceancv_img oceancv_ml)
	set(DEP_h2som opencv_core oceancv_util oceancv_ml)
	set(DEP_linearregression opencv_core oceancv_util oceancv_ml)
	set(DEP_statistical_descriptors opencv_imgcodecs opencv_imgproc opencv_core oceancv_util oceancv_img)
	set(DEP_zernike opencv_imgcodecs opencv_imgproc opencv_core oceancv_util oceancv_img oceancv_cudaimg opencv_cudaimgproc)
    set(DEP_laserpoints opencv_imgproc opencv_core oceancv_util oceancv_ml opencv_cudaarithm opencv_cudafilters)

	set(BUILD_DIRS descriptors h2som linearregression statistical_descriptors zernike laserpoints)

	if(NOT WITH_CUDA)
		list(REMOVE_ITEM BUILD_DIRS zernike)
	endif(NOT WITH_CUDA)

	foreach(DIR IN LISTS BUILD_DIRS)
		add_executable(${DIR} ${CMAKE_CURRENT_SOURCE_DIR}/examples/${DIR}/${DIR}.cpp)
		target_link_libraries(${DIR} ${DEP_${DIR}})
		install(TARGETS ${DIR} RUNTIME DESTINATION bin/oceancv/examples)
	endforeach(DIR)

	# Copy sample image
	install(FILES examples/Lenna.png DESTINATION examples)

endif(WITH_EXAMPLES)



if(WITH_BIN)

	# Dependencies
	set(DEP_asciDataCorrelation oceancv_util opencv_imgcodecs)
	set(DEP_videoTiledClustering opencv_imgcodecs opencv_imgproc opencv_core opencv_videoio MPEG7 oceancv_util oceancv_img oceancv_cudaimg opencv_cudaimgproc opencv_cudafilters)
	set(DEP_runCoMoNoD oceancv_util oceancv_img oceancv_cudaimg opencv_cudaimgproc opencv_cudafilters opencv_cudawarping opencv_imgproc opencv_core)
	set(DEP_trainDeLPHI oceancv_ml oceancv_util oceancv_img oceancv_cudaimg opencv_cudaimgproc opencv_cudafilters opencv_cudawarping opencv_imgproc opencv_core)
	set(DEP_runDeLPHI oceancv_ml oceancv_util oceancv_img oceancv_cudaimg opencv_cudaimgproc opencv_cudafilters opencv_cudawarping opencv_imgproc opencv_core)
	set(DEP_runMediaFeatureExtraction oceancv_util oceancv_img oceancv_cudaimg opencv_cudaimgproc opencv_cudafilters opencv_cudawarping opencv_imgproc opencv_core oceancv_ml opencv_videoio)
	set(DEP_openCVBuildConfig opencv_core)
	set(DEP_runOlimp oceancv_ml oceancv_util oceancv_img oceancv_cudaimg opencv_core opencv_video opencv_videoio opencv_cudaimgproc opencv_cudafilters opencv_cudawarping)

	set(BUILD_DIRS util ml uwi)
	foreach(DIR IN LISTS BUILD_DIRS)

		file(GLOB BINS "bin/${DIR}/*.cpp")
		list(FILTER BINS EXCLUDE REGEX "\\/\\.")

		if(NOT WITH_CUDA)
			list(REMOVE_ITEM BINS ${CMAKE_CURRENT_SOURCE_DIR}/bin/${DIR}/videoTiledClustering.cpp)
		endif(NOT WITH_CUDA)

		foreach(BIN IN LISTS BINS)
			string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/bin/${DIR}/ "" NAME ${BIN})
			string(REPLACE ".cpp" "" NAME ${NAME})
			add_executable(${NAME} ${BIN})
			target_link_libraries(${NAME} ${DEP_${NAME}})
			install(TARGETS ${NAME} RUNTIME DESTINATION bin/oceancv)
		endforeach(BIN)
	endforeach(DIR)

endif(WITH_BIN)


if(WITH_DOC)
	find_package(Doxygen)
	if(DOXYGEN_FOUND)

	    # set input and output files
	    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
	    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile)

	    # Copy readme and license to create mainpage
	    file(READ README.md CONTENTS)
		file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/doc/mainpage.md "${CONTENTS}")
		file(READ LICENSE.md CONTENTS)
		file(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/doc/mainpage.md "${CONTENTS}")

		# create target directory
		file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/doc/oceancv)

	    # request to configure the file
	    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

	    # note the option ALL which allows to build the docs together with the application
	    add_custom_target(doc ALL
	        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
	        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	        COMMENT "Generating API documentation with Doxygen"
	        VERBATIM )

	else(DOXYGEN_FOUND)
		message("Doxygen not found")
	endif(DOXYGEN_FOUND)
endif(WITH_DOC)

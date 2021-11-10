#pragma once

#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudaarithm.hpp"

namespace ocv {
namespace cuda {

	/**
	 * @brief Convert CV_8UC3 to CV_32FC1 and interval [0..1].
	 * @details Performs a simple data type conversion from CV_8UC3 to CV_32FC1
	 * utilizing the opencv cvtColor function.
	 * Additionally, the output is normalized to the interval [0..1].
	 * @param src the input matrix that shall be converted.
	 * @param dst the computed gray value float matrix
	 **/
	void RGB2GrayFloat(const cv::cuda::GpuMat& src, cv::cuda::GpuMat& dst);
	
	/**
	 * cv::Mat wrapper of the actual cv::cuda::GpuMat function that does the rgb to gray float conversion.
	 * See documentation of that function for parameter details.
	 */
	void RGB2GrayFloat(const cv::Mat& src, cv::cuda::GpuMat& dst);
	
}
}
 

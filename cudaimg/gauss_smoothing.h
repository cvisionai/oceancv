#pragma once

#include "opencv2/opencv.hpp"

#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudawarping.hpp"
#include "opencv2/cudafilters.hpp"
#include "opencv2/cudaarithm.hpp"

namespace ocv {
namespace cuda {

	/**
	 * Applies a Gaussian blurring filter to the given GpuMat. This GpuMat has to be
	 * of type CV_8UC3 as will the returned GpuMat.
	 */
	void Gauss(const cv::cuda::GpuMat& src, cv::cuda::GpuMat& dst, int gauss_size, bool convert_to_8bit = true);
	
}
}

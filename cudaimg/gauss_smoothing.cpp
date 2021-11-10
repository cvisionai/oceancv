#include "oceancv/cudaimg/gauss_smoothing.h"

namespace ocv {
namespace cuda {

	void Gauss(const cv::cuda::GpuMat& src, cv::cuda::GpuMat& dst, int gauss_size, bool convert_to_8bit) {

 		cv::Mat gauss_kernel;
		cv::cuda::GpuMat c_f_src, c_f_gauss, c_f_dst, c_gauss;
		std::vector<cv::cuda::GpuMat> src_channels, dst_channels, gauss_channels;

		src.convertTo(c_f_src, CV_32FC3);

		// Split the the source image on the GPU
		cv::cuda::split(c_f_src, dst_channels);
		cv::cuda::split(c_f_src, src_channels);

		// Prepare the input image for Gauss filtering
		cv::cuda::copyMakeBorder(c_f_src, c_f_gauss, gauss_size/2, gauss_size/2, gauss_size/2, gauss_size/2, cv::BORDER_REPLICATE);
		cv::cuda::split(c_f_gauss, gauss_channels);

		// Create the Gauss kernel and upload it to the GPU
		cv::Ptr<cv::cuda::Convolution> c = cv::cuda::createConvolution();
		cv::mulTransposed(cv::getGaussianKernel(gauss_size, -1, CV_32FC1), gauss_kernel,false);
		c_gauss.upload(gauss_kernel);

		// Apply Gaussian blur to all channels independently
		for(int i = 0; i < src.channels(); i++) {
			c->convolve(gauss_channels[i], c_gauss, dst_channels[i], true);
		}

		// Store result in given image reference
		if(convert_to_8bit) {
			cv::cuda::merge(dst_channels, c_f_dst);
			c_f_dst.convertTo(dst, CV_8UC3);
		} else {
			cv::cuda::merge(dst_channels, dst);
		}

	}

}
}

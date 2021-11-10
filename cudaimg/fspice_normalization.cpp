#include "oceancv/cudaimg/fspice_normalization.h"

namespace ocv {
namespace cuda {

	void fspice(const cv::cuda::GpuMat& c_src, cv::cuda::GpuMat& c_dst, int gauss_size, bool stretch_hist, bool convert_to_8bit) {

		cv::Mat f_src, gauss_kernel, hist;//, dst;
		cv::cuda::GpuMat c_f_src, c_f_dst, c_gauss, c_hist, c_f_gauss, c_f_tmp;//c_src,
		std::vector<cv::cuda::GpuMat> src_channels, dst_channels, gauss_channels;

		// Upload the src imagea to the GPU and convert it to float precision there
		c_src.convertTo(c_f_src, CV_32FC3);

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

		// Apply Gaussian blur to all channels independently, also compute subtraction image
		for(int i = 0; i < c_src.channels(); i++) {
			c->convolve(gauss_channels[i], c_gauss, gauss_channels[i], true);
			cv::cuda::addWeighted(src_channels[i], .5, gauss_channels[i], -.5, 127, dst_channels[i]);
		}

		// Prepare histogram computation (requires a CV_8UC1 image)
		cv::cuda::merge(dst_channels, c_f_dst);
		c_f_dst.convertTo(c_f_tmp, CV_8UC3);
		cv::cuda::cvtColor(c_f_tmp, c_dst, cv::COLOR_BGR2GRAY);

		// Compute histogram
		cv::cuda::calcHist(c_dst, c_hist);
		c_hist.download(hist);

		int offset = 10, minv = 0, maxv = hist.cols, hist_max = 0, mmin = hist.cols, mmax = 0, old_peak = 0, new_peak = 0;
		float gamma = 1.0, fac = 1.0;

		// Find peak in histogram
		for(int i = offset; i < hist.cols; i++) {
			if(hist.at<int>(i) > hist_max) {
				hist_max = hist.at<int>(i);
				old_peak = i;
			}
			if(hist.at<int>(i) > 0) {
				mmin = std::min(mmin,i);
				mmax = std::max(mmax,i);
			}
		}

		if(stretch_hist) {

			float limit = hist.at<int>(old_peak)/1000.0;
			for (int i = old_peak; i >= offset; i--) {
				if(hist.at<int>(i) <= limit) {
					minv = i+1;
					break;
				}
			}

			for (int i = old_peak; i < hist.cols; i++) {
				if(hist.at<int>(i) <= limit) {
					maxv = i-1;
					break;
				}
			}
		} else {
			maxv = mmax;
			minv = mmin;
		}

		// Compute histogram scaling factor
		maxv *= 2;
		minv *= 2;
		fac = 256.0/(maxv-minv);

		// Compute gamma correction factor
		if(stretch_hist) {
			new_peak = ((float)2.0 * old_peak - minv) * fac;
			if(new_peak <= 0)
				new_peak = 5;
			if(new_peak >= 250)
				new_peak = 220;
			gamma = std::log(128) / std::log(float(new_peak));
		}

		// Apply scaling and gamma correction to image
		for(size_t i = 0; i < src_channels.size(); i++) {
			cv::cuda::addWeighted(src_channels[i], fac, gauss_channels[i], -fac, fac*(255-minv), dst_channels[i]);
			cv::cuda::max(dst_channels[i],0,dst_channels[i]);

			cv::cuda::pow(dst_channels[i], gamma, dst_channels[i]);
			cv::cuda::min(dst_channels[i],255,dst_channels[i]);
			cv::cuda::max(dst_channels[i],0,dst_channels[i]);
		}

		// Retrieve result from GPU
		if(convert_to_8bit) {
			cv::cuda::merge(dst_channels, c_f_dst);
			c_f_dst.convertTo(c_dst, CV_8UC3);
		} else {
			cv::cuda::merge(dst_channels, c_dst);
		}

	}

	cv::Mat fspice(const cv::Mat& src, int gauss_size, bool stretch_hist) {

		cv::Mat ret;
		cv::cuda::GpuMat c_src, c_dst;
		c_src.upload(src);

		ocv::cuda::fspice(c_src,c_dst,gauss_size,stretch_hist);
		c_dst.download(ret);
		return ret;

	}

}
}

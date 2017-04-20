#include "opencv2/opencv.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudawarping.hpp"
#include "opencv2/cudafilters.hpp"
#include "opencv2/cudaarithm.hpp"

namespace ocv {
namespace cuda {
	
/**
 * @brief: Gauss-Smoothing of an image
 * Applies a Gaussian blurring filter to the given GpuMat. This GpuMat has to be 
 * of type CV_8UC3 as will the returned GpuMat.
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
void Gauss(const cv::cuda::GpuMat& src, cv::cuda::GpuMat& dst, int gauss_size, bool convert_to_8bit = true) {
	
	cv::Mat gauss_kernel;
	cv::cuda::GpuMat gf_src, gf_gauss, gf_dst;
	std::vector<cv::cuda::GpuMat> gfv_src, gfv_dst, gfv_gauss;
	
	src.convertTo(gf_src, CV_32FC3);

	// Split the the source image on the GPU
	cv::cuda::split(gf_src, gfv_dst);
	cv::cuda::split(gf_src, gfv_src);

	// Prepare the input image for Gauss filtering
	cv::cuda::copyMakeBorder(gf_src, gf_gauss, 0.5*gauss_size, 0.5*gauss_size, 0.5*gauss_size, 0.5*gauss_size, cv::BORDER_REPLICATE);
	cv::cuda::split(gf_gauss, gfv_gauss);

	// Create the Gauss kernel and upload it to the GPU
	cv::Ptr<cv::cuda::Convolution> c = cv::cuda::createConvolution();
	cv::mulTransposed(cv::getGaussianKernel(gauss_size, -1, CV_32FC1), gauss_kernel,false);
	gf_gauss.upload(gauss_kernel);

	// Apply Gaussian blur to all channels independently, also compute subtraction image
	for(int i = 0; i < src.channels(); i++)
		c->convolve(gfv_gauss[i], gf_gauss, gfv_dst[i], true);
	
	// Prepare histogram computation (requires a CV_8UC1 image)
	cv::cuda::merge(gfv_dst, gf_dst);
	gf_dst.convertTo(dst, CV_8UC3);
	
}


/**
 * @brief: fSpice on OpenCV GpuMat
 * Applies the fSpice color normalization to the given src image.
 * Uses cv::cuda::GpuMat as source and destination format.
 * See https://doi.org/10.1371/journal.pone.0038179 for details.
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
void fspice(const cv::cuda::GpuMat& c_src, cv::cuda::GpuMat& c_dst, int gauss_size, bool stretch_hist = true, bool convert_to_8bit = true) {
	
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
	cv::cuda::cvtColor(c_f_tmp, c_dst, CV_BGR2GRAY);
	
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

/**
 * @brief: fSpice on OpenCV Mat (using GPU)
 * Applies the fSpice color normalization to the given src image.
 * Uses cv::Mat as source and destination format but operates on the GPU.
 * See https://doi.org/10.1371/journal.pone.0038179 for details.
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
cv::Mat fspice(const cv::Mat& src, int gauss_size, bool stretch_hist = true) {
	
	cv::Mat ret;
	cv::cuda::GpuMat c_src, c_dst;
	c_src.upload(src);
	
	ocv::cuda::fspice(c_src,c_dst,gauss_size,stretch_hist);
	c_dst.download(ret);
	return ret;
	
}



}

/**
 * @brief: fSpice on OpenCV Mat (using CPU)
 * Applies the fSpice color normalization to the given src image.
 * Uses cv::Mat as source and destination format and does not operate on the GPU!
 * See https://doi.org/10.1371/journal.pone.0038179 for details.
 * @author: Timm Schoening - tschoening [at] geomar [dot] de
 * @date: 2017-04-20
 */
cv::Mat fspice(const cv::Mat& src, int gauss_size, bool stretch_hist = true) {
	
	cv::Mat gauss(src.size(), CV_8UC3);
	cv::Mat tmp1(src.size(), CV_32FC3);
	cv::Mat tmp2(src.size(), CV_32FC3);
	
	src.convertTo(tmp1, CV_32FC3);
	cv::GaussianBlur(tmp1,tmp2,cv::Size(gauss_size,gauss_size),0);
	
	tmp2.convertTo(gauss, CV_8UC3);
	
	float hist_max = 0, gamma = 1.0, tmp_val;
	int grey, new_val, bins = 512, minv = 0, maxv = bins, old_peak = 10, new_peak, mmin = bins, mmax = 0, offset = 10;
	int hist[bins];
	
	for(int j = 0; j < bins; j++) {
		hist[j] = 0;
	}

	cv::Vec3b gpix,fpix;
	
	int sx = 0, sy = 0, ex = src.cols, ey = src.rows;
	for(int i = sy; i < ey; i++) {
		for(int j = sx; j < ex; j++) {
	
			gpix = gauss.at<cv::Vec3b>(i,j);
			fpix = src.at<cv::Vec3b>(i,j);
			
			grey = int(floor((255-gpix[0]+fpix[0] + 255-gpix[1]+fpix[1] + 255-gpix[2]+fpix[2])/3));
			hist[grey]++;

		}
	}
	
	for(int i = offset; i < bins; i++) {
		if(hist[i] > hist_max) {
			hist_max = hist[i];
			old_peak = i;
		}
		if(hist[i]) {
			mmin = std::min(mmin,i);
			mmax = std::max(mmax,i);
		}
	}
	
	if(stretch_hist) {
		for(int i = old_peak; i >= offset; i--) {
			if(hist[i] <= hist[old_peak]/1000.0) {
				minv = i+1;
				break;
			}
		}
		
		for(int i = old_peak; i < bins; i++) {
			if(hist[i] <= hist[old_peak]/1000.0) {
				maxv = i-1;
				break;
			}
		}
	} else {
		minv = mmin;
		maxv = mmax;
	}
	
	float fac = 256.0/(maxv-minv);
	
	if(stretch_hist) {
		new_peak = ((float)old_peak - minv) * fac;
		if(new_peak <= 0)
			new_peak = 5;
		if(new_peak >= 250)
			new_peak = 220;
		gamma = log(128) / log(float(new_peak));
	}
	
	cv::Mat dst = cv::Mat(src.size(),CV_8UC3);
	for(int i = 0; i < src.rows; i++) {
		for(int j = 0; j < src.cols; j++) {
			
			gpix = gauss.at<cv::Vec3b>(i,j);
			fpix = src.at<cv::Vec3b>(i,j);
			
			for(int k = 0; k < 3; k++) {

				tmp_val = fac * (255 + fpix[k] - gpix[k] - minv);
				new_val = pow(tmp_val, gamma);
				
				if(new_val < 0) {
					new_val = 0;
				}
				if(new_val > 255) {
					new_val = 255;
				}
				dst.at<cv::Vec3b>(i,j)[k] = (uchar)floor(new_val);

			}

		}
	}
	
	return dst;

}
	
}

#include "oceancv/cudaimg/gabor_filter.h"

namespace ocv {
namespace cuda {

	std::vector<cv::cuda::GpuMat> gaborBank(const cv::cuda::GpuMat& img, bool useplain, bool usesig, bool usemu, int size, float jetsize, int scales, int phis) {

		std::vector<cv::cuda::GpuMat> gabor_filtered = {};

		cv::cuda::GpuMat gray_float, dst, gabor,tmpm, tmpm2, mu, sig, gauss_c;
		cv::Mat filter;
		float maxneg, maxpos, sigma, c2, c3;

		// Convolution pointers. One for the Gaussian, one for the Gabor
		cv::Ptr<cv::cuda::Filter> gaussian_filter;
		cv::Ptr<cv::cuda::Convolution> c = cv::cuda::createConvolution(cv::Size(2*size+1,2*size+1));

		// Transform given CPU RGB image to GPU Float image and add border for filtering
		if(useplain || usemu || usesig) {
			RGB2GrayFloat(img,tmpm);
			cv::cuda::copyMakeBorder(tmpm, gray_float,size, size, size, size, cv::BORDER_REPLICATE);
		}

		for(int m = 1; m <= scales; m++) {

			// Std-dev. for the Gaussian kernel
			sigma = jetsize * m;

			// Make Gaussian filter
			gaussian_filter = cv::cuda::createGaussianFilter(CV_32FC1, CV_32FC1, cv::Size(2*size+1,2*size+1), sigma);

			for(int phi = 1; phi <= phis; phi++) {

				// Create the Gabor filter wavelet
				filter = cv::getGaborKernel(cv::Size(2*size+1,2*size+1),sigma,CV_PI*phi/phis,1.8*sigma,1,0,CV_32F);

				// If no result images are to be computed, return the Gabor wavelets
				if(!useplain && !usemu && !usesig) {
					gabor_filtered.push_back(cv::cuda::GpuMat());
					gabor_filtered[gabor_filtered.size()-1].upload(filter);
					continue;
				}


				// Sum up pos and neg values in Gabor for normalization
				maxneg = 0;
				maxpos = 0;
				for(int i = 0; i < 2*size+1; i++) {
					for(int j = 0; j < 2*size+1; j++) {
						if(filter.at<float>(i,j) < 0 && filter.at<float>(i,j) > -200)
							maxneg += filter.at<float>(i,j);
						else if(filter.at<float>(i,j) > 0 && filter.at<float>(i,j) < 200)
							maxpos += filter.at<float>(i,j);
					}
				}

				// Apply jet to image
				gabor.upload(filter);
				c->convolve(gray_float, gabor, dst, true);

				if(useplain) {
					gabor_filtered.push_back(cv::cuda::GpuMat());
					dst.copyTo(gabor_filtered[gabor_filtered.size()-1]);
				}

				if(usemu || usesig) {

					// Normalize Gabor response
					c3 = 4.0 / (maxpos - maxneg);
					c2 = -0.5 * (maxneg + maxpos);

					cv::cuda::add(dst,c2,tmpm);
					cv::cuda::multiply(tmpm,c3,dst);


					// Apply tanh function (implemented using exponential GPU function)
					cv::cuda::exp(dst,tmpm);
					cv::cuda::add(tmpm,1,tmpm2);
					cv::cuda::divide(tmpm,tmpm2,dst);

					// Apply gauss to hyperbolized image -> mu
					gaussian_filter->apply(dst,mu);

					if(usemu) {
						gabor_filtered.push_back(cv::cuda::GpuMat());
						mu.copyTo(gabor_filtered[gabor_filtered.size()-1]);
					}

					if(usesig) {

						// Compute squared difference image
						cv::cuda::subtract(mu,dst,tmpm);
						cv::cuda::pow(tmpm,2,mu);

						// Apply gauss to diff -> sigma
						gaussian_filter->apply(mu,tmpm);

						//tmpm.download(tmp);
						gabor_filtered.push_back(cv::cuda::GpuMat());
						tmpm.copyTo(gabor_filtered[gabor_filtered.size()-1]);

					}

				}

			}

		}

		return gabor_filtered;
	}


	std::vector<cv::Mat> gaborBank(const cv::Mat& img, bool useplain, bool usesig, bool usemu, int size, float jetsize, int scales, int phis) {

		cv::cuda::GpuMat img_g;
		cv::Mat tmp;

		img_g.upload(img);

		std::vector<cv::cuda::GpuMat> ret_g = gaborBank(img_g, useplain, usesig, usemu, size, jetsize, scales, phis);

		std::vector<cv::Mat> ret;
		for(cv::cuda::GpuMat tmp_g : ret_g) {
			tmp_g.download(tmp);
			ret.push_back(tmp);
		}
		return ret;

	}

}
}

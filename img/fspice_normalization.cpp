#include "oceancv/img/fspice_normalization.h"

namespace ocv {

	cv::Mat fspice(const cv::Mat& src, int gauss_size, bool stretch_hist) {
		
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


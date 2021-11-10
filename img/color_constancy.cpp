#include "oceancv/img/color_constancy.h"

namespace ocv {

void cc::_floatGaussianBlur(const cv::Mat &src, cv::Mat& dst, cv::Mat& gau) {
	
	// Convert color space to float an scale to interval 0..1
	src.convertTo(dst,CV_32FC3,1/255.0);

	// Create linear intensity space
	cv::pow(dst, 2.2, dst);

	// The local space average color is computed using a gaussian blur
	cv::GaussianBlur(dst, gau, cv::Size(0,0), 0.093*(std::max(src.rows, src.cols)), 0 , cv::BORDER_REFLECT);

}

void cc::_floatToRGB(cv::Mat& srcdst) {

	// Re-transformation of the intensity space
	cv::pow(srcdst, 1/2.2, srcdst);
	srcdst.convertTo(srcdst,CV_8UC3,255.0);

}

void cc::localSpaceAverageColorScaling(const cv::Mat& src, cv::Mat& dst) {
	
	cv::Mat gau;
	
	// Transform the data type and intensity range
	_floatGaussianBlur(src,dst,gau);
	
	// The actual color constancy correction
	dst = 0.5 * dst / gau;

	// Re-transformation of the intensity range
	_floatToRGB(dst);

}

void cc::normalizedColorShiftLSAC(const cv::Mat &src, cv::Mat& dst) {
	
	cv::Mat gau;
	std::vector<cv::Mat> channels, gau_channels;

	cv::Mat avg(src.size(),CV_32FC1);
	cv::Mat gau_avg(src.size(),CV_32FC1);

	// Transform the data type and intensity range
	_floatGaussianBlur(src,dst,gau);
	
	// The normalized image is calculated
	cv::split(dst, channels);
	avg = 1/3.0 * (channels[0] + channels[1] + channels[2]);

	// The normalization of the local space average color image
	cv::split(gau, gau_channels);
	gau_avg = 1/3.0 * (gau_channels[0] + gau_channels[1] + gau_channels[2]);

	avg = avg / gau_avg;

	for(int i = 0; i < 3; i++) {
		channels[i] = channels[i] - avg.mul(gau_channels[i] - gau_avg);
	}	

	cv::merge(channels, dst);

	// Re-transformation of the intensity space
	_floatToRGB(dst);

}

void cc::colorShiftLSAC(const cv::Mat &src, cv::Mat& dst) {
	
	cv::Mat gau;
	std::vector<cv::Mat> channels, gau_channels;

	cv::Mat avg(src.size(),CV_32FC1);

	// Transform the data type and intensity range
	_floatGaussianBlur(src,dst,gau);

	// We need each color channel seperatly
	cv::split(dst, channels);

	// The channel-average of the local space average color image
	cv::split(gau, gau_channels);
	avg = 1/3.0 * (gau_channels[0] + gau_channels[1] + gau_channels[2]);

	for(int i = 0; i < 3; i++) {
		channels[i] = channels[i] - gau_channels[i] + avg;
	}	

	cv::merge(channels, dst);

	// Re-transformation of the intensity space
	_floatToRGB(dst);
	
}

void cc::whitePatchRetinexAndGrayWorldCombination(const cv::Mat& src, cv::Mat& dst) {
	
	cv::Mat gau;
	std::vector<cv::Mat> channels;

	// Transform the data type and intensity range
	_floatGaussianBlur(src,dst,gau);

	dst -= gau;

	// We need each color channel seperatly
	cv::split(dst, channels);
	for(int i = 0; i < 3; i++) {
		double d_min = 0;
		double d_max = 0;
		cv::minMaxIdx(channels[i],&d_min,&d_max);
		channels[i] = (channels[i] - d_min) / (d_max - d_min);
	}

	cv::merge(channels, dst);

	// Re-transformation of the intensity space
	_floatToRGB(dst);
	
}

}

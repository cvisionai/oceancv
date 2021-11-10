#include <math.h>
#include <vector>

#include "opencv2/opencv.hpp"

namespace ocv {

/**
 * Some color constancy algorithms implemented from the Book "Color Constancy" (Ebner, 2007)
 **/
class cc {

public:

	/**
	 * Needs a 8U image and uses the local space average color for color correction 
	 * by dividing the input image by twice the local space average color.
	 * @param src the source image
	 * @param dst the result image
	 **/
	static void localSpaceAverageColorScaling(const cv::Mat &src, cv::Mat& dst);

	/**
	 * Uses normalized color shifts and the local space average color for color constancy correction.
	 * @param src the source image
	 * @param dst the result image
	 **/
	static void normalizedColorShiftLSAC(const cv::Mat &src, cv::Mat& dst);

	/**
	 * Uses normalized color shifts and the local space average color for color constancy correction.
	 * @param src the source image
	 * @param dst the result image
	 **/
	static void colorShiftLSAC(const cv::Mat &src, cv::Mat& dst);

	/**
	 * This CC method substracts the local space average color and rescals the image by value_i - min / (max - min).
	 * @param src the source image
	 * @param dst the result image
	 **/
	static void whitePatchRetinexAndGrayWorldCombination(const cv::Mat &src, cv::Mat& dst);

private:
	
	static void _floatGaussianBlur(const cv::Mat &src, cv::Mat& dst, cv::Mat& gau);

	static void _floatToRGB(cv::Mat& srcdst);

};

}

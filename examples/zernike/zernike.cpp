#include <random>

#include "oceancv/cudaimg/zernike_filter.h"

using namespace std;

int main() {

	cv::Mat img = cv::imread("Lenna.png");
	
	double min,max;
	cv::Mat dst, zern;

	for(int n = 0; n < 6; n++) {
		for(int m = -n; m <= n; m += 2) {

			// Create Zernike polynomial
			zern = ocv::cuda::makeZernike(15,m,n);
			
			// Write to file
			cv::minMaxLoc(zern,&min,&max);
			cv::normalize(zern, dst, 0, 255, cv::NORM_MINMAX);
			cv::imwrite("zernike_" + to_string(n) + "_" + to_string(m) + ".png",dst);

			// Apply to sample image
			cv::filter2D(img,dst,CV_32F,zern);
			cv::imwrite("lenna_" + to_string(n) + "_" + to_string(m) + ".jpg",dst);
			
		}
	}
	
}

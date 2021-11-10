#pragma once

#include "opencv2/opencv.hpp"
#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudawarping.hpp"
#include "opencv2/cudafilters.hpp"
#include "opencv2/cudaarithm.hpp"

namespace ocv {
namespace cuda {

	// Computes a factorial
	int fac(int n);
	
	// Computes the sum term of the Zernike polynomial
	double zernikeSum(int m, int n, double rho);
	
	// Creates the zernike polynomial image
	cv::Mat makeZernike(int radius, int m, int n);
	
}
}
 

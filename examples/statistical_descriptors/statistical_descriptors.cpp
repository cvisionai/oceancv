#include <random>

#include "oceancv/img/statistical_filters.h"

using namespace std;

int main() {

	cv::Mat_<float> dst, sill, range, nugget, src;
	
	cv::Mat tmp, img = cv::imread("Lenna.png");
	cv::cvtColor(img,tmp,cv::COLOR_BGR2GRAY);
	tmp.convertTo(src,CV_32FC1);
	
	// Create fibonacci series for radii
	vector<int> fib = {1,2};
	for(int i = 0; i < 8; i++) {
		fib.push_back(fib[fib.size()-1]+fib[fib.size()-2]);
	}
	
	int r1, r2;
	for(size_t i1 = 3; i1 < fib.size(); i1++) {
		
		r1 = fib[i1];
		
		for(size_t i2 = i1+1; i2 < fib.size(); i2++) {
			
			r2 = fib[i2];
			
			// Compute TPI image
			ocv::applyTPI(src,dst,r1,r2);
			
			cv::normalize(dst,dst,0,255,cv::NORM_MINMAX);
			cv::imwrite("tpi_" + to_string(r1) + "_" + to_string(r2) + ".png",dst);
			
		}
		
		// Compute variogram (sill, range & nugget)
		ocv::variogram(src, sill, range, nugget, r1);
		cv::normalize(sill,dst,0,255,cv::NORM_MINMAX);
		cv::imwrite("var_" + to_string(r1) + "_sill.png",dst);
		
		cv::normalize(range,dst,0,255,cv::NORM_MINMAX);
		cv::imwrite("var_" + to_string(r1) + "_range.png",dst);
		
		cv::normalize(nugget,dst,0,255,cv::NORM_MINMAX);
		cv::imwrite("var_" + to_string(r1) + "_nugget.png",dst);
		
	}
	
}


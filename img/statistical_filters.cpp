#include "oceancv/img/statistical_filters.h"

namespace ocv {

void applyTPI(const cv::Mat_<float>& src, cv::Mat_<float>& dst, int inner_radius, int outer_radius) {
	
	// Get set of neighbouring pixels between inner and outer radius
	std::vector<cv::Point> nbs;
	ocv::getNeighboursInCircle(inner_radius,outer_radius,nbs);
	
	dst = cv::Mat_<float>(src.size(),static_cast<float>(0));
	float avg, fac = 1.f / nbs.size();
	
	// Work only on the inner part. A boundary of outer_radius widths will remain 0
	for(int y = outer_radius; y < src.rows - outer_radius; y++) {
		for(int x = outer_radius; x < src.cols - outer_radius; x++) {
			avg = 0;
			for(size_t i = 0; i < nbs.size(); i++) {
				avg += src(y+nbs[i].y,x+nbs[i].x);
			}
			dst(y,x) = avg * fac - src(y,x);
		}
	}
	
}

void variogram(const cv::Mat_<float>& src, cv::Mat_<float>& sill, cv::Mat_<float>& range, cv::Mat_<float>& nugget, int radius) {
	assert(radius > 3);
	
	// Initialize one set of neighbours per radius
	std::vector<std::vector<cv::Point>> nbs(radius+1,std::vector<cv::Point>());
	std::vector<cv::Point> tmp;
	std::vector<float> facs(radius+1,0);
	
	for(int i = 1; i <= radius; i++) {
		ocv::getNeighboursInCircle(0,i,tmp);
		nbs[i] = tmp;
		facs[i] = 1.f / tmp.size();
	}
	
	// Initialize the result matrix
	sill = cv::Mat_<float>(src.size(),0);
	range = cv::Mat_<float>(src.size(),0);
	nugget = cv::Mat_<float>(src.size(),0);
	
	float avg;
	int asy;
	std::vector<float> vars(radius+2,0);
	std::vector<float> s_vars(radius+2,0);
	for(int y = radius; y < src.rows - radius; y++) {
		for(int x = radius; x < src.cols - radius; x++) {

			// Compute variances
			for(int r = 1; r <= radius; r++) {
				avg = 0;
				for(size_t i = 0; i < nbs[r].size(); i++)
					avg += src(y+nbs[r][i].y,x+nbs[r][i].x);
				avg *= facs[r];
				vars[r] = 0;
				for(size_t i = 0; i < nbs[r].size(); i++)
					vars[r] += std::pow(src(y+nbs[r][i].y,x+nbs[r][i].x)-avg,2);
				vars[r] *= facs[r];
			}
			
			// Smooth variances
			vars[0] = vars[1];
			vars[radius+1] = vars[radius];
			for(int r = 1; r <= radius; r++) {
			 	s_vars[r] = 0.25 * (vars[r-1] + 2*vars[r] + vars[r+1]);
			}
			
			// Use start value as nugget
			nugget(y,x) = s_vars[1];
			
			// Find asymptote value as sill
			asy = radius+1;
			for(size_t r = radius; r > 0; r--) {
				if(s_vars[r] < 0.9 * s_vars[radius]) {
					asy = r;
					break;
				}
			}
			sill(y,x) = s_vars[asy];
			
			// Use distance to asymptote as range
			range(y,x) = asy;
			
		}
	}

}

}

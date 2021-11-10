#include "oceancv/cudaimg/zernike_filter.h"

namespace ocv {
namespace cuda {

	int fac(int n) {
		return (n < 2) ? 1 : fac(n - 1) * n;
	}

	double zernikeSum(int m, int n, double rho) {
		assert(n >= m && ((n-m) % 2 == 0));
		double ret = 0;
		for(int k = 0; k <= 0.5*(n-m); k++)
			ret += std::pow(rho,n-2*k) * 1.f * std::pow(-1,k) * fac(n - k) / (1.f * fac(k) * fac(0.5*(n+m)-k) * fac(0.5*(n-m)-k));
		return ret;
	}

	cv::Mat makeZernike(int radius, int m, int n) {
		assert(n >= 0 && n >= m && ((n-m) % 2 == 0));

		cv::Mat ret(2*radius+1,2*radius+1,CV_32FC1);

		double (*f)(double);

		if(m < 0)
		 	f = std::sin;
		else
			f = std::cos;

		double rho, phi, d, val, sum = 0;
		for(int y = -radius; y <= radius; y++) {
			for(int x = -radius; x <= radius; x++) {
				d = sqrt(x*x+y*y);
				phi = atan(1.f * y / x);
				rho = 1.f * d / radius;
				if(d != 0 && d < radius) {
					val = zernikeSum(abs(m),n,rho) * f(abs(m)*phi);
					if(n%2 == 1 && x < 0)
						val *= -1;
					ret.at<float>(radius-y,x+radius) = val;
					sum += val;
				} else
					ret.at<float>(radius-y,x+radius) = 0;
			}
		}

		return ret;

	}

}
}

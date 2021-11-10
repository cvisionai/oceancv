#include <random>

#include "oceancv/ml/linear_regression.h"

using namespace std;

int main() {

	cv::Mat_<float> features(5000,3);
	cv::Mat_<float> classes(features.rows,1,static_cast<float>(0));
	
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 10);
	
	float x,y,z;
	
	for(int i = 0; i < features.rows; i++) {
		x = dist(gen);
		y = dist(gen);
		z = dist(gen);
		features(i,0) = x;
		features(i,1) = y;
		features(i,2) = z;
		classes(i) = 42 * x + 23 + 0.1*dist(gen) + y * 12;
	}
	
	ocv::LinearRegression<float> lr(features,classes);

	std::cout << lr.determination() << std::endl;
	std::cout << lr.sumOfSquaredDifferences() << std::endl;
	std::cout << lr.yIntersect() << std::endl;
	std::cout << lr.regressionCoefficients().size() << std::endl;
	for(int i = 0; i < features.cols; i++) {
		std::cout << lr.regressionCoefficients()(i) << std::endl;
	}
}

#include <random>

#include "opencv2/opencv.hpp"

#include "oceancv/ml/feature_descriptor.h"
#include "oceancv/ml/color_descriptors.h"
#include "oceancv/ml/mpeg7_descriptors.h"
#include "oceancv/ml/metric.h"

using namespace std;

int main() {

	cv::Mat img = cv::imread("Lenna.png");

	cv::Mat mask, gray;

	cv::cvtColor(img,gray,cv::COLOR_BGR2GRAY);


	std::vector<ocv::FeatureDescriptor<float>*> desc_rgb;
	std::vector<ocv::FeatureDescriptor<float>*> desc_gry;

	desc_rgb.push_back(new ocv::ColorValueDescriptor<float>(3,0,1));
	desc_rgb.push_back(new ocv::ColorStatisticDescriptor<float>(3,true,true,true,true,true));
	desc_rgb.push_back(new ocv::HistogramColorDescriptor<float>(3,8));
	desc_gry.push_back(new ocv::HaralickTextureDescriptor<float>(16));
	desc_rgb.push_back(new ocv::ColorLayoutDescriptor<float>());
	desc_rgb.push_back(new ocv::ColorStructureDescriptor<float>());
	desc_rgb.push_back(new ocv::DominantColorDescriptor<float>());
	desc_rgb.push_back(new ocv::EdgeHistogramDescriptor<float>());
	desc_gry.push_back(new ocv::HomogeneousTextureDescriptor<float>());
	desc_rgb.push_back(new ocv::ScalableColorDescriptor<float>());

	desc_gry.push_back(new ocv::ColorValueDescriptor<float>(1,2,2));

	cv::Mat_<float> vec;

	vector<string> setup;
	for(auto desc : desc_rgb) {
		vec = cv::Mat_<float>(1,desc->size());
		desc->extract(img,vec,mask);
		setup = desc->setup();
		for(int i = 0; i < vec.cols; i++)
			std::cout << setup[i] << " " << vec(i) << std::endl;
	}

	for(auto desc : desc_gry) {
		vec = cv::Mat_<float>(1,desc->size());
		desc->extract(gray,vec,mask);
		setup = desc->setup();
		for(int i = 0; i < vec.cols; i++)
			std::cout << setup[i] << " " << vec(i) << std::endl;
	}

}

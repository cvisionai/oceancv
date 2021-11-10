#include "oceancv/ml/vec_algorithms.h"

class TestVecAlgorithms : public ::testing::Test {
protected:
	virtual void SetUp() {
		
		vec_size = 4;
		
		// Create a sample vector filled with numbers 1,2,3,...
		vec = cv::Mat_<float>(1,vec_size);
		std::iota(vec.begin(), vec.end(), 0);
		
		// And one with the inverse numbers
		vec_2 = cv::Mat_<float>(1,vec_size,vec_size-1);
		vec_2 -= vec;
	
	}
	
	int vec_size;
	cv::Mat_<float> vec;
	cv::Mat_<float> vec_2;
};

TEST_F(TestVecAlgorithms, BasicFunctions) {
	
	// Less and greater helper functions
	EXPECT_EQ(5, ocv::valg<float>::less(13,5));
	EXPECT_EQ(5, ocv::valg<float>::greater(3,5));

}

TEST_F(TestVecAlgorithms, MinMaxFunctions) {

	// Min/max elements or their indices (scalar)
	EXPECT_EQ(vec(vec.cols-1), ocv::valg<float>::maxElement(vec));
	EXPECT_EQ(vec.cols-1, ocv::valg<float>::maxElementIndex(vec));
	EXPECT_EQ(vec(0), ocv::valg<float>::minElement(vec));
	EXPECT_EQ(0, ocv::valg<float>::minElementIndex(vec));
	
	// Min/max elements of two vectors
	cv::Mat_<float> tmp;
	tmp = ocv::valg<float>::maxElements(vec, vec_2);
	EXPECT_EQ(tmp(0), tmp(tmp.cols-1));
	tmp = ocv::valg<float>::minElements(vec, vec_2);
	EXPECT_EQ(tmp(0), tmp(tmp.cols-1));
	
}

TEST_F(TestVecAlgorithms, ValueStatistics) {

	// Value statistics
	float sum = ocv::valg<float>::sumElements(vec);
	EXPECT_EQ(sum, vec_size * (vec_size - 1) * 0.5);
	float mean = ocv::valg<float>::mean(vec);
	std::vector<float> foo(vec_size);
	std::transform(vec.begin(), vec.end(), foo.begin(), [&mean](float a) -> float { return (a - mean) * (a - mean); });
	float var = std::accumulate(foo.begin(), foo.end(), static_cast<float>(0));
 	var = std::sqrt(var / vec_size);

	EXPECT_EQ(mean, sum / vec_size);
	EXPECT_EQ(ocv::valg<float>::median(vec), (int)vec_size/2);
	EXPECT_EQ(var, ocv::valg<float>::stdDev(vec));
	EXPECT_EQ(ocv::valg<float>::deviation(vec, mean), ocv::valg<float>::stdDev(vec));
	
	float t_mean, t_var;
	ocv::valg<float>::zscore(vec, t_mean, t_var);
	EXPECT_EQ(t_mean, mean);
	EXPECT_FLOAT_EQ(t_var, pow(var,2));
	
}

TEST_F(TestVecAlgorithms, OtherFunctions) {

	// Other
	cv::Mat_<float> vec_one(1,vec_size,1.f);
	cv::Mat_<float> vec_sgl(1,vec_size,0.f);
	vec_sgl(1) = 1;
	
	EXPECT_EQ(1,ocv::valg<float>::euclideanLength(vec_sgl));
	EXPECT_EQ(sqrt(vec_size),ocv::valg<float>::euclideanLength(vec_one));
	
}

TEST_F(TestVecAlgorithms, VectorModifications) {

	// Vector modifications
	vec = cv::Mat_<float>(1,3,2.f);
	vec(0) = 1;
	vec(2) = 3;
	ocv::valg<float>::applyZScore(vec);
	EXPECT_FLOAT_EQ(vec(0),-1.5);
	EXPECT_FLOAT_EQ(vec(1), 0);
	EXPECT_FLOAT_EQ(vec(2),1.5);
	
	cv::Mat_<float> mean_vec(1,3,-2);
	cv::Mat_<float> var_vec(1,3,2.f/3);
	ocv::valg<float>::applyZScore(vec, mean_vec, var_vec);
	EXPECT_FLOAT_EQ(vec(0),.75);
	EXPECT_FLOAT_EQ(vec(1), 3);
	EXPECT_FLOAT_EQ(vec(2),5.25);
	
	
	vec = cv::Mat_<float>(1,3,12.f);
	cv::Mat_<float> scale(1,3,1.f);
	scale(0) = 0.5;
	scale(1) = -1.f/3;
	scale(2) = -1.f/6;
	
	cv::Mat_<float> shift(1,3,0.f);
	shift(0) = -5;
	shift(1) = 5;
	shift(2) = 3;
	
	ocv::valg<float>::applyScaleRange(vec, shift, scale);
	EXPECT_FLOAT_EQ(vec(0),1);
	EXPECT_FLOAT_EQ(vec(1),1);
	EXPECT_FLOAT_EQ(vec(2),1);
	
	cv::Mat_<float> ret = ocv::valg<float>::append(vec, vec_2);
	EXPECT_EQ(ret.cols, vec_size + 3);
	EXPECT_FLOAT_EQ(ret(0), 1);
	EXPECT_FLOAT_EQ(ret(2), 1);
	EXPECT_FLOAT_EQ(ret(3), vec_size - 1);
	EXPECT_FLOAT_EQ(ret(ret.cols-1), 0);
	
}

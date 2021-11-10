#include "oceancv/ml/mat_algorithms.h"

class TestMatAlgorithms : public ::testing::Test {
protected:
		virtual void SetUp() {
			m = cv::Mat_<float>(3,3,static_cast<float>(0));
			m(0,0) = 1;
			m(1,0) = 2;
			m(1,1) = 3;
			m(1,2) = -4;
			m(2,0) = 1.1;
		}
		cv::Mat_<float> m;
};

TEST_F(TestMatAlgorithms, bestMatchIndex) {
	
	cv::Mat_<float> v1(1,3,static_cast<float>(0));
	v1(0,0) = 0.5;
	
	cv::Mat_<float> v2(1,3,static_cast<float>(0));
	v2(0,0) = 1.3;
	
	auto ret = ocv::malg<float>::bestMatchIndex(m, v1, ocv::EuclideanMetric<float>());
	EXPECT_EQ(ret,0);
	
	auto ret2 = ocv::malg<float>::bestMatchIndex(m, v2, ocv::EuclideanMetric<float>());
	EXPECT_EQ(ret2,2);
	
}

TEST_F(TestMatAlgorithms, minmaxElements) {

	cv::Mat_<float> max = ocv::malg<float>::maxElements(m);
	EXPECT_FLOAT_EQ(max(0),2);
	EXPECT_FLOAT_EQ(max(1),3);
	EXPECT_FLOAT_EQ(max(2),0);
	
	cv::Mat_<float> min = ocv::malg<float>::minElements(m);
	EXPECT_FLOAT_EQ(min(0),1);
	EXPECT_FLOAT_EQ(min(1),0);
	EXPECT_FLOAT_EQ(min(2),-4);

}

TEST_F(TestMatAlgorithms, zscore) {

	cv::Mat_<float> tmp;
	m.copyTo(tmp);
	cv::Mat_<float> avg = ocv::malg<float>::mean(tmp);
	
	EXPECT_FLOAT_EQ(avg(0),1.3666666);
	EXPECT_FLOAT_EQ(avg(1),1);
	EXPECT_FLOAT_EQ(avg(2),-1.3333333);

	ocv::malg<float>::applyMean(tmp,avg);
	cv::Mat_<float> avg2 = ocv::malg<float>::mean(tmp);
	
	EXPECT_FLOAT_EQ(avg2(0),0);
	EXPECT_FLOAT_EQ(avg2(1),0);
	EXPECT_FLOAT_EQ(avg2(2)+1,1);
	
	m.copyTo(tmp);
	avg = ocv::malg<float>::mean(tmp);
	
	cv::Mat_<float> var = ocv::malg<float>::variance(tmp, avg);
	
	EXPECT_FLOAT_EQ(var(0),0.202222222);
	EXPECT_FLOAT_EQ(var(1),2);
	EXPECT_FLOAT_EQ(var(2),3.555555555);

	ocv::malg<float>::applyZScore(tmp, avg, var);

	avg = ocv::malg<float>::mean(tmp);
	var = ocv::malg<float>::variance(tmp, avg);

	EXPECT_FLOAT_EQ(avg(0),0);
	EXPECT_FLOAT_EQ(avg(1),0);
	EXPECT_FLOAT_EQ(avg(2)+1,1);
	
	EXPECT_FLOAT_EQ(var(0),1);
	EXPECT_FLOAT_EQ(var(1),1);
	EXPECT_FLOAT_EQ(var(2),1);
	
}


TEST_F(TestMatAlgorithms, range) {

	cv::Mat_<float> min(1,3,static_cast<float>(0));
 	cv::Mat_<float> max(1,3,static_cast<float>(1));
	
	ocv::VecPair<float> vp1 = ocv::malg<float>::scaleRange(m, min, max);
	ocv::VecPair<float> vp2 = ocv::malg<float>::scaleRange(m, 0, 1);
	
	EXPECT_FLOAT_EQ(vp1.i(0),vp2.i(0));
	EXPECT_FLOAT_EQ(vp1.i(1),vp2.i(1));
	EXPECT_FLOAT_EQ(vp1.i(2),vp2.i(2));
	EXPECT_FLOAT_EQ(vp1.o(0),vp2.o(0));
	EXPECT_FLOAT_EQ(vp1.o(1),vp2.o(1));
	EXPECT_FLOAT_EQ(vp1.o(2),vp2.o(2));
	
	cv::Mat_<float> tmp;
	m.copyTo(tmp);
	
	ocv::malg<float>::applyScaleRange(tmp, vp1.i(), vp1.o());
	
	vp2 = ocv::malg<float>::scaleRange(tmp, 0, 1);
	
	EXPECT_FLOAT_EQ(1,vp2.i(0));
	EXPECT_FLOAT_EQ(1,vp2.i(1));
	EXPECT_FLOAT_EQ(1,vp2.i(2));
	
	EXPECT_FLOAT_EQ(0,vp2.o(0));
	EXPECT_FLOAT_EQ(0,vp2.o(1));
	EXPECT_FLOAT_EQ(0,vp2.o(2));
	
}

TEST_F(TestMatAlgorithms, other) {

	// Normalization
	cv::Mat_<float> tmp;
	m.copyTo(tmp);
	
	ocv::malg<float>::normalize(tmp, ocv::EuclideanMetric<float>());
	
	EXPECT_FLOAT_EQ(tmp(1,0), 0.3713907);
	EXPECT_FLOAT_EQ(tmp(1,2), -0.742781353);
	
	EXPECT_FLOAT_EQ(ocv::valg<float>::euclideanLength(tmp.row(1)),1);
	
	// Transpose
	cv::Mat_<float> dst;
	ocv::malg<float>::transpose(m, dst);
	
	EXPECT_FLOAT_EQ(m(1,0), dst(0,1));
	EXPECT_FLOAT_EQ(m(0,0), dst(0,0));
	EXPECT_FLOAT_EQ(m(2,0), dst(0,2));
	EXPECT_FLOAT_EQ(m(1,1), dst(1,1));
	
	std::vector<cv::Mat_<float>> vec = {m,tmp,dst};
	std::vector<cv::Mat_<float>> ret;
	ocv::malg<float>::means(vec, ret);
	
	cv::Mat_<float> avg = ocv::malg<float>::mean(m);
	
	EXPECT_FLOAT_EQ(avg(0),ret[0](0));
	EXPECT_FLOAT_EQ(avg(1),ret[0](1));
	EXPECT_FLOAT_EQ(avg(2),ret[0](2));
	
	avg = ocv::malg<float>::mean(dst);
	
	EXPECT_FLOAT_EQ(avg(0),ret[2](0));
	EXPECT_FLOAT_EQ(avg(1),ret[2](1));
	EXPECT_FLOAT_EQ(avg(2),ret[2](2));
	
}

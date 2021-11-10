#include "oceancv/ml/metric.h"

class TestMetric : public ::testing::Test {
protected:
	
	virtual void SetUp() {
		vec_size = 3;
		vec_one = cv::Mat_<float>(1,vec_size,1.f);
		vec_nil = cv::Mat_<float>(1,vec_size,0.f);
		vec_sgl_1 = cv::Mat_<float>(1,vec_size,0.f);
		vec_sgl_1(1) = 1;
		vec_sgl_2 = cv::Mat_<float>(1,vec_size,0.f);
		vec_sgl_2(2) = 1;
		vec_cpx_1 = cv::Mat_<float>(1,vec_size,0.f);
		std::iota(vec_cpx_1.begin(), vec_cpx_1.end(), 0);
		vec_cpx_2 = cv::Mat_<float>(1,vec_size,0.f);
		std::iota(vec_cpx_2.begin(), vec_cpx_2.end(), 23);
	}

	size_t vec_size;
	cv::Mat_<float> vec_one;
	cv::Mat_<float> vec_nil;
	cv::Mat_<float> vec_sgl_1;
	cv::Mat_<float> vec_sgl_2;
	cv::Mat_<float> vec_cpx_1;
	cv::Mat_<float> vec_cpx_2;
	
};

TEST_F(TestMetric, EuclideanMetric) {
	
	// Construct the metric object
	ocv::EuclideanMetric<float> met_eucl;
	
	EXPECT_FLOAT_EQ(3, vec_size);
	
	// Check values and dimensions
	EXPECT_FLOAT_EQ(0, met_eucl.distance(vec_one, vec_one));
	EXPECT_FLOAT_EQ(sqrt(vec_size),met_eucl.distance(vec_one, vec_nil));
	EXPECT_FLOAT_EQ(met_eucl.distance(vec_one, vec_sgl_1), met_eucl.distance(vec_one, vec_sgl_2));
	EXPECT_FLOAT_EQ(met_eucl.distance(vec_cpx_1, vec_cpx_2), met_eucl.distance(vec_cpx_2, vec_cpx_1));
	EXPECT_FLOAT_EQ(sqrt(2), met_eucl.distance(vec_sgl_1, vec_sgl_2));
	EXPECT_EQ(met_eucl.type(), ocv::METRIC_TYPES::EUCLIDEAN);
	
}

TEST_F(TestMetric, EuclideanMetricSquared) {

	// Construct the metric object
	ocv::EuclideanMetricSquared<float> met_eucl2;
	
	EXPECT_FLOAT_EQ(0, met_eucl2.distance(vec_one, vec_one));
	EXPECT_FLOAT_EQ(vec_size,met_eucl2.distance(vec_one, vec_nil));
	EXPECT_FLOAT_EQ(met_eucl2.distance(vec_one, vec_sgl_1), met_eucl2.distance(vec_one, vec_sgl_2));
	EXPECT_FLOAT_EQ(met_eucl2.distance(vec_cpx_1, vec_cpx_2), met_eucl2.distance(vec_cpx_2, vec_cpx_1));
	EXPECT_EQ(met_eucl2.type(), ocv::METRIC_TYPES::EUCLIDEAN_SQUARED);

}

TEST_F(TestMetric, ManhattanMetric) {

	ocv::ManhattanMetric<float> met_man;
	
	vec_sgl_1 *= 2;
	vec_sgl_2 *= 2;
	vec_nil(1) = 1;
	vec_nil(2) = 1;
	
	EXPECT_FLOAT_EQ(met_man.distance(vec_nil, vec_sgl_1), met_man.distance(vec_nil, vec_sgl_2));
	EXPECT_FLOAT_EQ(met_man.distance(vec_cpx_1, vec_cpx_2), met_man.distance(vec_cpx_2, vec_cpx_1));
	EXPECT_EQ(met_man.type(), ocv::METRIC_TYPES::MANHATTAN);
	
}

TEST_F(TestMetric, ScalarMetric) {
	ocv::ScalarMetric<float> met_scl;
	EXPECT_EQ(met_scl.type(), ocv::METRIC_TYPES::SCALAR);
	EXPECT_FLOAT_EQ(met_scl.distance(vec_cpx_1, vec_cpx_2), met_scl.distance(vec_cpx_2, vec_cpx_1));
	EXPECT_FLOAT_EQ(met_scl.distance(vec_nil, vec_sgl_1), met_scl.distance(vec_nil, vec_sgl_2));
}

TEST_F(TestMetric, AngleMetric) {
	
	ocv::AngleMetric<float> met_ang;
	
	cv::Mat_<float> two_sgl_1;
	cv::Mat_<float> two_sgl_2;
	
	vec_sgl_1.copyTo(two_sgl_1);
	vec_sgl_2.copyTo(two_sgl_2);
	
	two_sgl_1 *= 5;
	two_sgl_2 *= 5;
	
	EXPECT_FLOAT_EQ(met_ang.distance(vec_sgl_1, vec_sgl_2), met_ang.distance(two_sgl_1, two_sgl_2));
	EXPECT_FLOAT_EQ(met_ang.distance(vec_cpx_1, vec_cpx_2), met_ang.distance(vec_cpx_2, vec_cpx_1));
	EXPECT_EQ(met_ang.type(), ocv::METRIC_TYPES::ANGLE);
	
}

TEST_F(TestMetric, MaximumMetric) {
	ocv::MaximumMetric<float> met_max;
	EXPECT_FLOAT_EQ(met_max.distance(vec_cpx_1, vec_nil), vec_size-1);
	EXPECT_FLOAT_EQ(met_max.distance(vec_cpx_1, vec_cpx_2), met_max.distance(vec_cpx_2, vec_cpx_1));
	EXPECT_EQ(met_max.type(), ocv::METRIC_TYPES::MAXIMUM);
}


#include "oceancv/ml/mat_pair.h"

class TestMatPair : public ::testing::Test {
 protected:
};

TEST_F(TestMatPair, ConstructorFillsDataCorrectly) {
	
	// Construct a MatPair object
	ocv::MatPair<float,int> mp(15,2,1,23,42);
	
	// Check values and dimensions
	EXPECT_EQ(23, mp.i(0,0));
	EXPECT_EQ(42, mp.o(0,0));
	EXPECT_EQ(15, mp.rows());
	EXPECT_EQ(2, mp.iCols());
	EXPECT_EQ(1, mp.oCols());
	
}

TEST_F(TestMatPair, CopyingWorks) {
	
	// Construct a MatPair object
	ocv::MatPair<float,int> mp(15,2,1,23,42);
	
	// Fetch input and putput data and check reuslt values
	cv::Mat_<float> vec_i = mp.iRow(5);
	EXPECT_EQ(23, vec_i(0));
	EXPECT_EQ(2, vec_i.cols);
	cv::Mat_<int> vec_o = mp.oRow(3);
	EXPECT_EQ(42, vec_o(0));
	EXPECT_EQ(1, vec_o.cols);
	
	// Write new data to MatPair and check results
	vec_i(0) = 13.3;
	vec_o(0) = 12;
	mp.pair(12, vec_i, vec_o);
	EXPECT_FLOAT_EQ(13.3, mp.i(12,0));
	EXPECT_EQ(12, mp.o(12,0));
	
}

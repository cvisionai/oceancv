#include "oceancv/ml/mat_pair_algorithms.h"

class TestMatPairAlgorithms : public ::testing::Test {
protected:
		virtual void SetUp() {
			
			mp = ocv::MatPair<float,int>(5,3,3,static_cast<float>(0),static_cast<int>(0));
			
			mp.i(0,0) = 1;
			mp.i(1,0) = 2;
			mp.i(1,1) = 3;
			mp.i(1,2) = -4;
			mp.i(2,0) = 1.1;
			mp.i(2,1) = 11.1;
			mp.i(3,0) = 5.1;
			mp.i(3,1) = 2.1;
			mp.i(4,2) = 3.1;
			
			mp.o(0,1) = 3;
			mp.o(1,1) = 3;
			mp.o(2,1) = 5;
			mp.o(3,1) = 5;
			mp.o(4,1) = 2;
			
		}
		ocv::MatPair<float,int> mp;
};

TEST_F(TestMatPairAlgorithms, search) {
	
	cv::Mat_<int> vec(1,3,static_cast<int>(0));
	vec(1) = 6;
	
	int idx = ocv::mpalg<float,int>::bestMatchIndexOutput(mp, vec, ocv::EuclideanMetric<int>());
	
	EXPECT_EQ(idx,2);
	
}

TEST_F(TestMatPairAlgorithms, zscore) {

	cv::Mat_<float> mean, variance;
	
	int label = 5;
	int col_index = 1;
	
	int affected = ocv::mpalg<float,int>::mean(mp, mean, label, col_index);
	
	EXPECT_EQ(affected,2);
	EXPECT_FLOAT_EQ(mean(0),3.1);
	EXPECT_FLOAT_EQ(mean(1),6.6);

	label = 5;
	affected = ocv::mpalg<float,int>::zscore(mp, mean, variance, label, col_index);
	
	EXPECT_EQ(affected,2);
	EXPECT_FLOAT_EQ(mean(1),6.6);
	
	EXPECT_FLOAT_EQ(variance(0),4);
	EXPECT_FLOAT_EQ(variance(1),20.25);
	EXPECT_FLOAT_EQ(variance(2),0);
	
}

TEST_F(TestMatPairAlgorithms, subset) {

	ocv::MatPair<float,int> subset;
	
	ocv::mpalg<float,int>::selectInputColumns(mp, subset, {0,1,1});
	
	EXPECT_EQ(subset.rows(),5);
	EXPECT_EQ(subset.iCols(),2);
	EXPECT_EQ(subset.oCols(),3);
	
	EXPECT_FLOAT_EQ(subset.i(3,0),2.1);
	EXPECT_EQ(subset.o(2,1),5);
	
    int affected = ocv::mpalg<float,int>::selectRows(mp, subset, std::vector<int>({0,1,0,1,0}));
	
    EXPECT_EQ(affected,2);
    EXPECT_EQ(subset.rows(),2);
    EXPECT_FLOAT_EQ(subset.i(1,1),2.1);
    EXPECT_EQ(subset.o(1,1),5);

	ocv::mpalg<float,int>::randomSubset(mp, subset, 3);
	
	EXPECT_EQ(subset.rows(),3);
	EXPECT_EQ(subset.iCols(),3);
	EXPECT_EQ(subset.oCols(),3);
	
}

TEST_F(TestMatPairAlgorithms, append) {

	cv::Mat_<float> add(mp.rows(),2,static_cast<float>(1));
	
	ocv::mpalg<float,int>::appendCols(mp, add);
	
	EXPECT_EQ(mp.iCols(),5);
	EXPECT_EQ(mp.rows(),5);
	EXPECT_EQ(mp.oCols(),3);
	
	ocv::MatPair<float,int> add_mp(2,mp.iCols(),mp.oCols(),static_cast<float>(13.13),static_cast<int>(7));
	ocv::mpalg<float,int>::appendRows(mp, add_mp);
	
	EXPECT_EQ(mp.iCols(),5);
	EXPECT_EQ(mp.rows(),7);
	EXPECT_EQ(mp.oCols(),3);
	
	EXPECT_FLOAT_EQ(mp.i(6,2),13.13);
	EXPECT_EQ(mp.o(6,2),7);
	
}

TEST_F(TestMatPairAlgorithms, split) {

	int o_col_idx = 1;
	
	std::vector<ocv::MatPair<float,int>> dst;
    ocv::mpalg<float,int>::splitMatPair(mp, dst, o_col_idx);
	
    EXPECT_EQ(dst.size(),3);
	
    // The order of the MatPairs is by increasing value occurring in the o_col_idx
    EXPECT_EQ(dst[0].rows(),1);
    EXPECT_EQ(dst[1].rows(),2);
    EXPECT_EQ(dst[2].rows(),2);
    EXPECT_EQ(dst[1].iCols(),3);
    EXPECT_EQ(dst[1].oCols(),3);
	
	std::vector<cv::Mat_<float>> dst_m;
	ocv::mpalg<float,int>::splitMatPairToMat(mp, dst_m, o_col_idx);
	
	EXPECT_EQ(dst_m[0].rows,1);
    EXPECT_EQ(dst_m[1].rows,2);
    EXPECT_EQ(dst_m[2].rows,2);
    EXPECT_EQ(dst_m[1].cols,3);
	
}

TEST_F(TestMatPairAlgorithms, other) {

	bool isit = ocv::mpalg<float,int>::isNaN(mp);
	EXPECT_EQ(isit,false);
	
	mp.i(3,2,NAN);
	isit = ocv::mpalg<float,int>::isNaN(mp);
	EXPECT_EQ(isit,true);
	
	mp.i(3,2,17);
	mp.i(4,1,NAN);
	isit = ocv::mpalg<float,int>::isNaN(mp);
	EXPECT_EQ(isit,true);
	
}

#include "oceancv/ml/vec_pair.h"

class TestVecPair : public ::testing::Test {
protected:
		virtual void SetUp() {
			cv::Mat_<float> vec_i(1,5,1.f);
			cv::Mat_<int> vec_o(1,3,2);
			vp = new ocv::VecPair<float,int>(vec_i, vec_o);
		}
		ocv::VecPair<float,int>* vp;
};

TEST_F(TestVecPair, ConstructorWorks) {
	
	EXPECT_EQ(vp->iCols(), 5);
	EXPECT_EQ(vp->oCols(), 3);
	
	EXPECT_EQ((*vp)[3], 1.f);
	EXPECT_EQ((*vp).o(2), 2);
	
}

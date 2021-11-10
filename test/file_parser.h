#include "oceancv/util/file_parser.h"

class TestFileParser : public ::testing::Test {
protected:

};

TEST_F(TestFileParser, ConstructorWorks) {
	
	auto p = ocv::guessASCIFileSeparators("file_parser.txt");
	
	EXPECT_EQ(std::get<0>(p), "\n");
	EXPECT_EQ(std::get<1>(p), ",");
	
	auto vv = ocv::readASCIFile("file_parser.txt");
	
	EXPECT_EQ(vv[0][0], "Hello");
	EXPECT_EQ(vv[0][1], "World;;");
	
	auto vv2 = ocv::readASCIFile("file_parser.txt",";;\n",",");
	
	EXPECT_EQ(vv2[0][0], "Hello");
	EXPECT_EQ(vv2[0][1], "World");
	
}

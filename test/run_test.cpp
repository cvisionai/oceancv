#include "gtest/gtest.h"

#include "metric_test.h"
#include "vec_pair_test.h"
#include "mat_pair_test.h"
#include "vec_algorithms_test.h"
#include "file_parser.h"
#include "mat_algorithms_test.h"
#include "mat_pair_algorithms_test.h"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

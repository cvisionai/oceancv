#include "oceancv/cudaimg/misc_func.h"

namespace ocv {
namespace cuda {

	void RGB2GrayFloat(const cv::cuda::GpuMat& src, cv::cuda::GpuMat& dst) {

		if(src.channels() == 3) {
			cv::cuda::GpuMat tmp;
			cv::cuda::cvtColor(src,tmp,cv::COLOR_BGR2GRAY);
			tmp.convertTo(dst,CV_32FC1);
		} else {
			src.convertTo(dst,CV_32FC1);
		}

		cv::cuda::multiply(dst,1.0 / 255.0,dst);

	}

	void RGB2GrayFloat(const cv::Mat& src, cv::cuda::GpuMat& dst) {
		cv::cuda::GpuMat tmp;
		tmp.upload(src);
		RGB2GrayFloat(tmp,dst);
	}

}
}

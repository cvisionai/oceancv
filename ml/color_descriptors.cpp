#include "oceancv/ml/color_descriptors.h"

namespace ocv {

template<class T>
ColorValueDescriptor<T>::ColorValueDescriptor(int channels, int offset, int rings) : _channels(channels), _offset(offset), _rings(rings) { }

template<class T>
void ColorValueDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && ((mask.rows == 0 && mask.cols == 0) || (mask.rows == img.rows && mask.cols == img.cols)) && this->valid(img));
	
	int idx = 0;
	int cx = img.cols / 2;
	int cy = img.rows / 2;

	std::vector<cv::Mat> imgs;
	cv::split(img,imgs);
	
	bool use_mask = !mask.empty();
	
	for(int i = 0; i < this->_rings; i++) {
		for(int x = -i; x <= i; x++) {
			for(int y = -i; y <= i; y++) {
				if(abs(x)+abs(y) == i) {
					for(int c = 0; c < _channels; c++) {
						if(use_mask && mask.at<uchar>(cx+x*this->_offset,cy+y*this->_offset) != 255)
							vec(idx++) = -1;
						else
							vec(idx++) = imgs[c].at<uchar>(cx+x*this->_offset,cy+y*this->_offset);
					}
				}
			}
		}
	}
	
}

template<class T>
bool ColorValueDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.channels() == _channels && img.rows > 2 * _rings * _offset + 1 && img.cols > 2 * _rings * _offset + 1);
}

template<class T>
int ColorValueDescriptor<T>::size() const {
	int idx = 0;
	for(int i = 0; i < _rings; i++) {
		for(int x = -i; x <= i; x++) {
			for(int y = -i; y <= i; y++) {
				if(abs(x)+abs(y) == i) {
					idx += _channels;
				}
			}
		}
	}
	return idx;
}

template<class T>
std::vector<std::string> ColorValueDescriptor<T>::setup() const {
	std::vector<std::string> ret;
	for(int i = 0; i < _rings; i++) {
		for(int x = -i; x <= i; x++) {
			for(int y = -i; y <= i; y++) {
				if(abs(x)+abs(y) == i) {
					for(int c = 0; c < _channels; c++)
						ret.push_back("ColorValueDescriptor_" + std::to_string(c));
				}
			}
		}
	}
	return ret;
}




template<class T>
ColorStatisticDescriptor<T>::ColorStatisticDescriptor(int channels, bool use_minmax, bool use_mean, bool use_median, bool use_stddev, bool use_entropy) : _channels(channels), _use_minmax(use_minmax), _use_mean(use_mean), _use_median(use_median), _use_stddev(use_stddev), _use_entropy(use_entropy) {}


template<class T>
void ColorStatisticDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && ((mask.rows == 0 && mask.cols == 0) || (mask.rows == img.rows && mask.cols == img.cols)) && this->valid(img));
	
	// Eventually split channels (no real effect for a CV-8UC1 mat)
	std::vector<cv::Mat> channels;
	cv::split(img,channels);
	
	size_t idx = 0;
	
	if(_use_minmax) {
		double minv, maxv;
		for(int c = 0; c < _channels; c++) {
			cv::minMaxLoc(channels[c], &minv, &maxv, 0, 0, mask);
			vec(idx++) = minv;
			vec(idx++) = maxv;
		}
	}
	
	if(_use_mean) {
		cv::Scalar mean = cv::mean(img, mask);
		for(int c = 0; c < _channels; c++)
			vec(idx++) = mean[c];
	}
		
	if(_use_stddev) {
		cv::Scalar mean, stddev;
		cv::meanStdDev(img, mean, stddev, mask);
		for(int c = 0; c < _channels; c++)
			vec(idx++) = stddev[c];
	}
	
	if(_use_median) {
	
		bool use_mask = !mask.empty();
		
		// Extract color values
		std::vector<std::vector<uchar>> color_values(_channels,std::vector<uchar>(img.rows*img.cols));
		for(int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				if(!use_mask || (use_mask && mask.at<uchar>(y,x) == 255)) {
					for(int c = 0; c < _channels; c++) {
						color_values[c][y*img.cols+x] = channels[c].at<uchar>(y,x);
					}
				}
			}
		}
		
		size_t total_pixels = img.total();
		size_t skip_pixels = total_pixels;
		if(use_mask)
			skip_pixels = cv::countNonZero(mask);
		size_t median = total_pixels - 0.5 * skip_pixels;
		for(int c = 0; c < _channels; c++) {
			std::sort(color_values[c].begin(), color_values[c].end());
			vec(idx++) = color_values[c][median];
		}
		
	}
	
	if(_use_entropy) {
		for(int c = 0; c < _channels; c++)
			vec(idx++) = ocv::entropy(channels[c], mask);
	}
	
}



template<class T>
bool ColorStatisticDescriptor<T>::valid(const cv::Mat& img) const{
	return ((img.type() == CV_8UC1 && _channels == 1) || (img.type() == CV_8UC3 && _channels == 3));
}

template<class T>
int ColorStatisticDescriptor<T>::size() const {
	int ret = 0;
	if(_use_minmax)
		ret += 2;
	if(_use_mean)
		ret++;
	if(_use_median)
		ret++;
	if(_use_stddev)
		ret++;
	if(_use_entropy)
		ret++;
	return _channels * ret;
}

template<class T>
std::vector<std::string> ColorStatisticDescriptor<T>::setup() const {
	std::vector<std::string> ret;
	if(_use_minmax) {
		for(int i = 0; i < _channels; i++) {
			ret.push_back("ColorStatisticDescriptor_min");
			ret.push_back("ColorStatisticDescriptor_max");
		}
	}
	if(_use_mean) {
		for(int i = 0; i < _channels; i++)
			ret.push_back("ColorStatisticDescriptor_mean");
	}
	if(_use_stddev) {
		for(int i = 0; i < _channels; i++)
			ret.push_back("ColorStatisticDescriptor_stddev");
	}
	if(_use_median) {
		for(int i = 0; i < _channels; i++)
			ret.push_back("ColorStatisticDescriptor_median");
	}
	if(_use_entropy) {
		for(int i = 0; i < _channels; i++)
			ret.push_back("ColorStatisticDescriptor_entropy");
	}
	return ret;
}




template<class T>
HistogramColorDescriptor<T>::HistogramColorDescriptor(int channels, int num_bins, T max_value) : _channels(channels), _num_bins(num_bins), _max_value(max_value) {
	_bin_size = _max_value / _num_bins;
}

template<class T>
void HistogramColorDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && ((mask.rows == 0 && mask.cols == 0) || (mask.rows == img.rows && mask.cols == img.cols)) && this->valid(img));
	
	// Prepare histogram
	size_t hist[_channels*_num_bins];
	for(int c = 0; c < _channels*_num_bins; c++)
		hist[c] = 0;
	
	std::vector<cv::Mat> channels;
	cv::split(img,channels);
	
	bool use_mask = !mask.empty();
	
	// Count color occurrence
	int tmp_bin;
	size_t num_pix = 0;
	for(int y = 0; y < img.rows; y++) {
		for(int x = 0; x < img.cols; x++) {
			if(!use_mask || (use_mask && mask.at<uchar>(y,x) == 255)) {
				for(int c = 0; c < _channels; c++) {
					tmp_bin = floor(((double) channels[c].at<uchar>(y,x)) / _bin_size);
					hist[c*_channels+tmp_bin]++;
				}
				num_pix++;
			}
		}
	}
	
	// Store bins in featvec
	size_t idx = 0;
	for(int c = 0; c < _channels; c++) {
		for(int b = 0; b < _num_bins; b++) {
			vec(idx++) = (1./T(num_pix)) * hist[c * _channels + b];
		}
	}
	
}

template<class T>
bool HistogramColorDescriptor<T>::valid(const cv::Mat& img) const{
	return ((img.type() == CV_8UC1 && _channels == 1) || (img.type() == CV_8UC3 && _channels == 3));
}

template<class T>
int HistogramColorDescriptor<T>::size() const {
	return _num_bins * _channels;
}

template<class T>
std::vector<std::string> HistogramColorDescriptor<T>::setup() const {
	std::vector<std::string> ret;
	for(int c = 0; c < _channels; c++) {
		for(int i = 0; i < _num_bins; i++) {
			ret.push_back("Histogram_c" + std::to_string(c));
		}
	}
	return ret;
}



template<class T>
HaralickTextureDescriptor<T>::HaralickTextureDescriptor(int num_bins, int offset, bool angular_second_moment, bool contrast, bool correlation, bool sum_of_squares_variance, bool inverse_difference_moment, bool sum_average, bool sum_variance, bool sum_entropy, bool entropy, bool difference_variance, bool difference_entropy) : _num_bins(num_bins), _offset(offset) {

	_subdescriptor_activity = {angular_second_moment,contrast,correlation,sum_of_squares_variance,inverse_difference_moment,sum_average,sum_entropy,sum_variance,entropy,difference_variance,difference_entropy};
	
}


template<class T>
void HaralickTextureDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && ((mask.rows == 0 && mask.cols == 0) || (mask.rows == img.rows && mask.cols == img.cols)) && this->valid(img));
	
	// Compute the pixel pair color occurrences
	
	// Prepare GLCM matrix
	T* glcm = new T[_num_bins*_num_bins];
	for(int a = 0; a < _num_bins; a++) {
		for(int b = 0; b < _num_bins; b++) {
			glcm[a*_num_bins+b] = 0;
		}
	}
	
	bool use_mask = !mask.empty();
	
	// Compute absolute counts
	int bin, tbin;
	float bin_width = 256 / _num_bins;
	size_t num_pix = 0;
	for(int y = 0; y < img.rows - _offset; y++) {
		for(int x = 0; x < img.cols - _offset; x++) {
		
			if(use_mask && mask.at<uchar>(y,x) != 255)
				continue;
			
			bin = img.at<uchar>(y,x) / bin_width;
			
			// Vertical
			tbin = img.at<uchar>(y+_offset,x) / bin_width;
			glcm[bin*_num_bins+tbin]++;
			
			// Horizontal
			tbin = img.at<uchar>(y,x+_offset) / bin_width;
			glcm[bin*_num_bins+tbin]++;
		
			// Diagonal
			tbin = img.at<uchar>(y+_offset,x+_offset) / bin_width;
			glcm[bin*_num_bins+tbin]++;
			
			num_pix++;
			
		}
		
	}

	// Normalize counts to probabilites
	
	for(int i = 0; i < _num_bins; i++) {
		for(int j = 0; j < _num_bins; j++) {
			glcm[i*_num_bins+j] /= num_pix;
		}
	}
	
	
	T p_plus[2*_num_bins];
	for(int i = 0; i < 2 * _num_bins; i++)
		p_plus[i] = 0;
	for(int n = 0; n < 2* _num_bins; n++) {
		for(int i = 0; i < _num_bins; i++) {
			for(int j = 0; j < _num_bins; j++) {
				if(i + j == n) {
					p_plus[n] += glcm[i*_num_bins+j];
				}
			}
		}
	}
	
	T p_minus[_num_bins];
	for(int i = 0; i < _num_bins; i++)
		p_minus[i] = 0;
	for(int n = 0; n < 2* _num_bins; n++) {
		for(int i = 0; i < _num_bins; i++) {
			for(int j = 0; j < _num_bins; j++) {
				if(std::abs(i - j) == n) {
					p_minus[n] += glcm[i*_num_bins+j];
				}
			}
		}
	}
	
	T tmp;
	int idx = 0;

	// Angular second momentum
	if(_subdescriptor_activity[0]) {
		
		tmp = 0.0;
		for(int i = 0; i < _num_bins; i++) {
			for(int j = 0; j < _num_bins; j++) {
				tmp += std::pow(glcm[i*_num_bins+j],2);
			}
		}
		vec(idx++) += tmp;
		
	}
	
	// Contrast
	if(_subdescriptor_activity[1]) {
	
		tmp = 0.0;
		for(int n = 0; n < _num_bins; n++) {
			tmp += std::pow(n,2) * p_minus[n];
		}
		vec(idx++) += tmp;
		
	}
	
	// Correlation
	if(_subdescriptor_activity[2]) {
	
		double mean_x = 0, mean_y = 0, var_x = 0, var_y = 0;
		
		for(int i = 0; i < _num_bins; i++) {
			tmp = 0;
			for(int j = 0; j < _num_bins; j++) {
				tmp += glcm[i*_num_bins+j];
			}
			mean_x += i * tmp;
			
			tmp = 0;
			for(int j = 0; j < _num_bins; j++) {
				tmp += glcm[j*_num_bins+i];
			}
			mean_y += i * tmp;
		}
		
		for(int i = 0; i < _num_bins; i++) {
			tmp = 0;
			for(int j = 0; j < _num_bins; j++) {
				tmp += glcm[i*_num_bins+j];
			}
			var_x += std::pow(i - mean_x,2) * tmp;
			
			tmp = 0;
			for(int j = 0; j < _num_bins; j++) {
				tmp += glcm[j*_num_bins+i];
			}
			var_y += std::pow(i - mean_y,2) * tmp;
		}
		
		tmp = 0;
		for(int i = 0; i < _num_bins; i++) {
			for(int j = 0; j < _num_bins; j++) {
				tmp += (i - mean_x) * (j - mean_y) * glcm[i*_num_bins+j];
			}
		}
		tmp /= std::sqrt(var_x);
		tmp /= std::sqrt(var_y);
		vec(idx++) += tmp;
		
	}
	
	// Sum of squares variance
	if(_subdescriptor_activity[3]) {
	
		T mean = 0;
		for(int i = 0; i < _num_bins; i++) {
			for(int j = 0; j < _num_bins; j++) {
				mean += glcm[i*_num_bins+j];
			}
		}
		mean /= (_num_bins * _num_bins);
		
		tmp = 0;
		for(int i = 0; i < _num_bins; i++) {
			for(int j = 0; j < _num_bins; j++) {
				tmp += std::pow(i - mean,2) * glcm[i*_num_bins+j];
			}
		}
		vec(idx++) += tmp;
		
	}
	
	// Inverse Difference Moment
	if(_subdescriptor_activity[4]) {
	
		tmp = 0;
		for(int i = 0; i < _num_bins; i++) {
			for(int j = 0; j < _num_bins; j++) {
				tmp += 1/(1+std::pow(i - j,2)) * glcm[i*_num_bins+j];
			}
		}
		vec(idx++) += tmp;
		
	}
	
	// Sum average
	if(_subdescriptor_activity[5]) {
	
		tmp = 0;
		for(int n = 0; n < 2* _num_bins; n++) {
			tmp += n * p_plus[n];
		}
		vec(idx++) += tmp;
		
	}
	
	// Sum entropy
	if(_subdescriptor_activity[6]) {
	
		tmp = 0;
		for(int n = 0; n < 2* _num_bins; n++) {
			tmp += p_plus[n] * _llog(p_plus[n]);
		}
		vec(idx++) += -tmp;
		
	}
	
	// Sum variance
	if(_subdescriptor_activity[7]) {
	
		T fstmp = 0;
		for(int n = 0; n < 2* _num_bins; n++) {
			fstmp += p_plus[n] * _llog(p_plus[n]);
		}
		
		tmp = 0;
		for(int n = 0; n < 2* _num_bins; n++) {
			tmp += std::pow(n + fstmp,2) * p_plus[n];
		}
		vec(idx++) += tmp;
		
	}
	
	// Entropy
	if(_subdescriptor_activity[8]) {
	
		tmp = 0;
		for(int i = 0; i < _num_bins; i++) {
			for(int j = 0; j < _num_bins; j++) {
				tmp += glcm[i*_num_bins+j] * _llog(glcm[i*_num_bins+j]);
			}
		}
		vec(idx++) += -tmp;
		
	}
	
	// Difference variance
	if(_subdescriptor_activity[9]) {
	
		T meanm = 0;
		for(int n = 0; n < _num_bins; n++) {
			meanm += p_minus[n];
		}
		meanm /= _num_bins;
		tmp = 0;
		for(int n = 0; n < _num_bins; n++) {
			tmp += std::pow(p_minus[n] - meanm,2);
		}
		vec(idx++) += -tmp;
		
	}
	
	// Difference Entropy
	if(_subdescriptor_activity[10]) {
	
		tmp = 0;
		for(int n = 0; n < _num_bins; n++) {
			tmp += p_minus[n] * _llog(p_minus[n]);
		}
		vec(idx++) += -tmp;
		
	}
	
}



template<class T>
bool HaralickTextureDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC1);
}

template<class T>
int HaralickTextureDescriptor<T>::size() const {
	int idx = 0;
	for(size_t i = 0; i < _subdescriptor_activity.size(); i++) {
		if(_subdescriptor_activity[i])
			idx++;
	}
	return idx;
}

template<class T>
std::vector<std::string> HaralickTextureDescriptor<T>::setup() const {
	
	std::vector<std::string> names({"angular-second-moment","contrast","correlation","sum-of-squares-variance","inverse-difference-moment","sum-averages","sum-entropy","sum-variance","entropy","difference-variance","difference-entropy"});
	
	std::vector<std::string> ret;
	for(size_t i = 0; i < _subdescriptor_activity.size(); i++) {
		if(_subdescriptor_activity[i]) {
			ret.push_back("HARALICK_" + names[i]);
		}
	}
	return ret;
	
}

template<class T>
T HaralickTextureDescriptor<T>::_llog(T val) const {
	if(val == 0)
		return std::log(0.0000000000001);
	else
		return std::log(val);
}


template class ColorValueDescriptor<float>;
template class ColorStatisticDescriptor<float>;
template class HistogramColorDescriptor<float>;
template class HaralickTextureDescriptor<float>;

}

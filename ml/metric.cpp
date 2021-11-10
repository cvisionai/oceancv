#include "oceancv/ml/metric.h"

namespace ocv {

	template<class T>
	T Metric<T>::similarity(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		return 1 - distance(vec_1, vec_2);
	}
	
	
	template<class T>
	T EuclideanMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2)  const {
		T tmp = static_cast<T>(0);
		for(auto it1 = vec_1.begin(), it2 = vec_2.begin(); it1 < vec_1.end(); it1++, it2++) {
			tmp += (*it1 - *it2) * (*it1 - *it2);
		}
		return std::sqrt(tmp);
	}

	template<class T>
	ocv::METRIC_TYPES EuclideanMetric<T>::type() const {
		return ocv::METRIC_TYPES::EUCLIDEAN;
	}


	template<class T>
	T EuclideanMetricSquared<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		T tmp = static_cast<T>(0);
		for(auto it1 = vec_1.begin(), it2 = vec_2.begin(); it1 < vec_1.end(); it1++, it2++) {
			tmp += (*it1 - *it2) * (*it1 - *it2);
		}
		return tmp;
	}

	template<class T>
	ocv::METRIC_TYPES EuclideanMetricSquared<T>::type() const {
		return ocv::METRIC_TYPES::EUCLIDEAN_SQUARED;
	}


	template<class T>
	T ManhattanMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		T tmp = 0;
		for(auto it1 = vec_1.begin(), it2 = vec_2.begin(); it1 < vec_1.end(); it1++, it2++) {
			tmp += abs(*it1 - *it2);
		}
		return tmp;
	}

	template<class T>
	ocv::METRIC_TYPES ManhattanMetric<T>::type() const {
		return ocv::METRIC_TYPES::MANHATTAN;
	}


	template<class T>
	T MaximumMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		T tmp = 0;
		for(auto it1 = vec_1.begin(), it2 = vec_2.begin(); it1 < vec_1.end(); it1++, it2++) {
			tmp = std::max(tmp, (T)abs(*it1 - *it2));
		}
		return tmp;
	}

	template<class T>
	ocv::METRIC_TYPES MaximumMetric<T>::type() const {
		return ocv::METRIC_TYPES::MAXIMUM;
	}


	template<class T>
	T ScalarMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const{
		return 1 - std::inner_product(vec_1.begin(),vec_1.end(),vec_2.begin(),static_cast<T>(0));
	}

	template<class T>
	ocv::METRIC_TYPES ScalarMetric<T>::type() const {
		return ocv::METRIC_TYPES::SCALAR;
	}

	
	template<class T>
	T AngleMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		return acos(std::inner_product(vec_1.begin(),vec_1.end(),vec_2.begin(),static_cast<T>(0))/(ocv::valg<T>::euclideanLength(vec_1)*ocv::valg<T>::euclideanLength(vec_2)));
	}

	template<class T>
	ocv::METRIC_TYPES AngleMetric<T>::type() const {
		return ocv::METRIC_TYPES::ANGLE;
	}
	
	
	template<class T>
	T DivergenceMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		T tmp = static_cast<T>(0);
		for(auto it1 = vec_1.begin(), it2 = vec_2.begin(); it1 < vec_1.end(); it1++, it2++) {
			tmp += (*it1 - *it2) * (*it1 - *it2) / (*it1 + *it2);
		}
		return 1.f/vec_1.cols*std::sqrt(tmp);
	}

	template<class T>
	ocv::METRIC_TYPES DivergenceMetric<T>::type() const {
		return ocv::METRIC_TYPES::DIVERGENCE;
	}
	
	template<class T>
	T EHDMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		
		float ret = static_cast<T>(0);
		
		std::array<int,5> g_values_1 = {0,0,0,0,0};
		std::array<int,5> g_values_2 = {0,0,0,0,0};
		
		for(auto it1 = vec_1.begin(), it2 = vec_2.begin(); it1 < vec_1.end(); it1 += 5, it2 += 5) {
			for(int i = 0; i < 5; i++) {
				ret += std::abs(*(it1+i) - *(it2+i));
				g_values_1[i] += *(it1+i);
				g_values_2[i] += *(it2+i);
			}
		}
		for(int i = 0; i < 5; i++)
			ret += std::abs(g_values_1[i] - g_values_2[i])*5.0/16;

		return ret;
		
	}

	template<class T>
	ocv::METRIC_TYPES EHDMetric<T>::type() const {
		return ocv::METRIC_TYPES::MPEG7_EHD;
	}
	
	
	template<class T>
	T HTDMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		
		float ret = static_cast<T>(0);
		
		double wm[5] = {0.42,1.00,1.00,0.08,1.00};
		double wd[5] = {0.32,1.00,1.00,1.00,1.00};

		
		for(int n = 0; n < 5; n++) {
			for(int m = 0; m < 6; m++) {
				ret += (float)( wm[n]*abs( vec_1(n*6+m+2) - vec_2(n*6+m+2) ) )
						     + ( wd[n]*abs( vec_1(n*6+m+30+2) - vec_2(n*6+m+30+2) ) );
			}
		}

		return ret/9237.0f;

	}

	template<class T>
	ocv::METRIC_TYPES HTDMetric<T>::type() const {
		return ocv::METRIC_TYPES::MPEG7_HTD;
	}
	
	
	
	template<class T>
	T CLDMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) const {
		return distance(vec_1, vec_2, 64);
	}
	
	template<class T>
	T CLDMetric<T>::distance(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2, int num_y_coefficients) const {
		
		int ny = num_y_coefficients;
		int nc = 0.5 * (vec_1.cols - num_y_coefficients);
		
		// Weight factors to increase the influence of the first colors (3x y, 1x Cb, 3x Cr)
		std::map<int,int> weights = {{0,2},{1,2},{2,2},{ny,2},{ny+nc,4},{ny+nc+1,2},{ny+nc+2,2}};
		std::vector<int> w(vec_1.cols,1);
		for(auto p : weights)
			w[p.first] = p.second;
		
		int sum1 = 0;
		for(int i = 0; i < ny; i++)
			sum1 += w[i] * (vec_1(i) - vec_2(i)) * (vec_1(i) - vec_2(i));
		
		int sum2 = 0;
		for(int i = ny; i < ny+nc; i++)
			sum2 += w[i] * (vec_1(i) - vec_2(i)) * (vec_1(i) - vec_2(i));
		
		int sum3 = 0;
		for(int i = ny+nc; i < ny+nc+nc; i++)
			sum3 += w[i] * (vec_1(i) - vec_2(i)) * (vec_1(i) - vec_2(i));
		
		return sqrt(sum1) + sqrt(sum2) + sqrt(3);
		
	}

	template<class T>
	ocv::METRIC_TYPES CLDMetric<T>::type() const {
		return ocv::METRIC_TYPES::MPEG7_CLD;
	}
	

	
	template class DivergenceMetric<int>;
	template class DivergenceMetric<float>;
	template class DivergenceMetric<double>;
	
	template class EHDMetric<int>;
	template class EHDMetric<float>;
	template class EHDMetric<double>;

	template class CLDMetric<int>;
	template class CLDMetric<float>;
	template class CLDMetric<double>;
	
	template class HTDMetric<int>;
	template class HTDMetric<float>;
	template class HTDMetric<double>;
	
	
	template class EuclideanMetric<int>;
	template class EuclideanMetric<float>;
	template class EuclideanMetric<double>;

	template class EuclideanMetricSquared<int>;
	template class EuclideanMetricSquared<float>;
	template class EuclideanMetricSquared<double>;

	template class ManhattanMetric<int>;
	template class ManhattanMetric<float>;
	template class ManhattanMetric<double>;

	template class MaximumMetric<int>;
	template class MaximumMetric<float>;
	template class MaximumMetric<double>;

	template class ScalarMetric<int>;
	template class ScalarMetric<float>;
	template class ScalarMetric<double>;

	template class AngleMetric<int>;
	template class AngleMetric<float>;
	template class AngleMetric<double>;

	template class Metric<int>;
	template class Metric<float>;
	template class Metric<double>;

}

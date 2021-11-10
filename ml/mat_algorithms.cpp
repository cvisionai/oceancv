#include "oceancv/ml/mat_algorithms.h"

namespace ocv {

	template<class T>
	size_t malg<T>::bestMatchIndex(const cv::Mat_<T>& mat, const cv::Mat_<T>& vec, const ocv::Metric<T>& metric) {
		
		T tmp_dist, min_dist = metric.distance(vec, mat.row(0));
	    int best_id = 0;
	    for(int i = 1; i < mat.rows; i++) {
	        tmp_dist = metric.distance(vec, mat.row(i));
	        if(tmp_dist < min_dist) {
	            min_dist = tmp_dist;
	            best_id = i;
	        }
	    }
	    return best_id;
	    
	}

	template<class T>
	void malg<T>::autoCorrelation(const cv::Mat_<T>& m, cv::Mat_<T>& dst) {
	    assert(m.rows > 0);
	    assert(m.cols > 0);
	    dst = cv::Mat_<T>(m.cols, m.cols);
	    dst = static_cast<T>(0);
	    for(int i = 0; i < m.rows; i++) {
	        for(int j = 0; j < m.rows; j++) {
	            for(int k = 0; k < m.cols; k++) {
	            	dst(j,k) += m(i,j) * m(i,k) / m.rows;
	            }
	        }
	    }
	}

	template<class T>
	void malg<T>::covarianceMat(const cv::Mat_<T>& m, cv::Mat_<T>& dst) {
	    assert(m.rows > 0);
	    assert(m.cols > 0);
		
	    dst = cv::Mat_<T>(m.cols, m.cols, T(0));
	    cv::Mat_<T> allmean = ocv::malg<T>::mean(m);

	    for(int i = 0; i < m.rows-1; i++) {
	        for(int j = 0; j < m.cols; j++) {
	            for(int k = 0; k < m.cols; k++) {
	            	dst(j,k) = (1. - 1. / (i + 1)) * dst(j,k) + 1. / (i + 1) * (m(i,j) - allmean(j)) * (m(i,k) - allmean(k));
	            }
	        }
	    }

	    for(int j = 0; j < m.cols; j++) {
			for(int k = 0; k < m.cols; k++) {
				dst(j,k) = dst(j,k) + 1. / (m.rows-1) * (m(m.rows-1,j) - allmean(j)) * (m(m.rows-1,k) - allmean(k));
			}
		}

	}

	template<class T>
	cv::Mat_<T> malg<T>::mean(const cv::Mat_<T>& m) {
	    cv::Mat_<T> ret(1, m.cols, (T) 0.0);
	    for(int i = 0; i < m.rows; i++) {
	        ret *= 1. - 1. / (i + 1);
	        std::transform(ret.begin(),ret.end(),m.row(i).begin(),ret.begin(),[&i](T a, T b) -> T { return a + T(1. / (i + 1)) * b; });
	    }
	    return ret;
	}

	template<class T>
	cv::Mat_<T> malg<T>::maxElements(const cv::Mat_<T>& m) {
	    cv::Mat_<T> ret;
		m.row(0).copyTo(ret);
	    for(int i = 1; i < m.rows; i++) {
	    	ret = ocv::valg<T>::maxElements(ret,m.row(i));
		}
		return ret;
	}

	template<class T>
	cv::Mat_<T> malg<T>::minElements(const cv::Mat_<T>& m) {
	    cv::Mat_<T> ret;
	    m.row(0).copyTo(ret);
	    for(int i = 1; i < m.rows; i++)
	        ret = ocv::valg<T>::minElements(ret,m.row(i));
	    return ret;
	}
	
	template<class T>
	void malg<T>::applyMean(cv::Mat_<T>& m, const cv::Mat_<T>& mean) {
		assert(m.rows > 0);
	    cv::Mat_<T> row;
	    for(int i = 0; i < m.rows; i++) {
	    	row = m.row(i);
	    	std::transform(row.begin(),row.end(),mean.begin(),row.begin(),std::minus<T>());
	    }
	}

	template<class T>
	cv::Mat_<T> malg<T>::variance(const cv::Mat_<T>& m, const cv::Mat_<T>& mean) {
		
	    cv::Mat_<T> variance(1,m.cols,T(0));
		std::vector<T> tmp(m.cols,T(0));
		cv::Mat_<T> row;
		
	    for(int i = 0; i < m.rows; i++) {
	    	row = m.row(i);
	    	std::transform(row.begin(), row.end(), mean.begin(), tmp.begin(), std::minus<T>());
	    	std::transform(tmp.begin(), tmp.end(), tmp.begin(), tmp.begin(), std::multiplies<T>());
	    	std::transform(tmp.begin(), tmp.end(), variance.begin(), variance.begin(), std::plus<T>());
	    }
		
	    std::transform(variance.begin(), variance.end(), variance.begin(), std::bind2nd(std::divides<T>(), T(m.rows)));
	    
	    return variance;
		
	}

	template <class T>
	void malg<T>::applyZScore(cv::Mat_<T>& m, const cv::Mat_<T>& mean, const cv::Mat_<T>& variance) {
		
	    assert(m.rows >= 1);
		std::vector<T> tmp(m.cols,T(0));
		cv::Mat_<T> row;
		int idx;
	    for(int i = 0; i < m.rows; i++) {
	    	idx = 0;
	    	row = m.row(i);
	    	std::transform(row.begin(), row.end(), mean.begin(), row.begin(), [&idx,&variance](T a, T b) -> T { return (a - b) / sqrt(variance(idx++)); });
	    }
		
	}

	template <class T>
	ocv::VecPair<T> malg<T>::scaleRange(const cv::Mat_<T>& m, const cv::Mat_<T> new_min, const cv::Mat_<T> new_max) {
		
	    cv::Mat_<T> old_min = ocv::malg<T>::minElements(m);
	    cv::Mat_<T> old_max = ocv::malg<T>::maxElements(m);
		
	    cv::Mat_<T> shift = cv::Mat_<T>(1,m.cols);
	    cv::Mat_<T> scale = cv::Mat_<T>(1,m.cols);
		
	    for(int i = 0; i < m.cols; i++) {
	        if(old_max(i) == old_min(i)) {
				scale(i) = 0;
				shift(i) = 0;
			} else {
				scale(i) = (new_max(i) - new_min(i)) / (old_max(i) - old_min(i));
				shift(i) = (old_max(i) * new_min(i) - old_min(i) * new_max(i)) / (old_max(i) - old_min(i));
			}
	    }
		
		return ocv::VecPair<T>(scale,shift);
		
	}

	template <class T>
	ocv::VecPair<T> malg<T>::scaleRange(const cv::Mat_<T>& m, T new_min, T new_max) {
	    cv::Mat_<T> min = cv::Mat_<T>(1, m.cols, new_min);
	    cv::Mat_<T> max = cv::Mat_<T>(1, m.cols, new_max);
	    return ocv::malg<T>::scaleRange(m, min, max);
	}

	template <class T>
	void malg<T>::applyScaleRange(cv::Mat_<T>& m, const cv::Mat_<T>& scale, const cv::Mat_<T>& shift) {
		size_t idx;
		cv::Mat_<T> row;
	    for(int i = 0; i < m.rows; i++) {
	    	idx = 0;
	    	row = m.row(i);
	    	std::transform(row.begin(),row.end(), scale.begin(), row.begin(), [&idx,&shift](T a, T b) -> T { return (a * b) + shift(idx++); });
	    }
	}

	template<class T>
	void malg<T>::normalize(cv::Mat_<T>& m, const ocv::Metric<T>& metric) {
	    cv::Mat_<T> zeros(1, m.cols, static_cast<T> (0.0));
	    T distance;
	    cv::Mat_<T> row;
	    for(int i = 0; i < m.rows; i++) {
	    	distance = metric.distance(zeros, m.row(i));
	    	row = m.row(i);
	    	if(distance == 0) {
	    		for(auto it = row.begin(); it < row.end(); it++)
	    			*it = 0;
			} else {
				m(i) = m(i) / distance;
				for(auto it = row.begin(); it < row.end(); it++)
	    			*it /= distance;
			}
	    }
	}

	template<class T>
	void malg<T>::transpose(const cv::Mat_<T>& m, cv::Mat_<T>& dst) {
	    dst = cv::Mat_<T>(m.cols, m.rows);
	    for(int j = 0; j < m.cols; j++)
	        for(int i = 0; i < m.rows; i++)
	            dst(j,i) = m(i,j);
	}
	
	
	template<class T>
	void malg<T>::means(const std::vector<cv::Mat_<T>>& single_mats, std::vector<cv::Mat_<T>>& ret) {
		ret.clear();
		for(auto m : single_mats) {
			ret.push_back(ocv::malg<T>::mean(m));
		}
	}
	
	template class malg<int>;
//	template class malg<size_t>;
	template class malg<float>;
	template class malg<double>;

}

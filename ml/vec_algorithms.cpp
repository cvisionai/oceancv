#include "oceancv/ml/vec_algorithms.h"

namespace ocv {

	template<class T>
	T valg<T>::less(T one, T two) {
		if(one < two)
			return one;
		else
			return two;
	}
	
	template<class T>
	T valg<T>::greater(T one, T two) {
		if(one > two)
			return one;
		else
			return two;
	}

	template<class T>
	T valg<T>::maxElement(const cv::Mat_<T>& vec) {
		return *std::max_element(vec.begin(), vec.end());
	}

	template<class T>
	int valg<T>::maxElementIndex(const cv::Mat_<T>& vec) {
		return std::max_element(vec.begin(), vec.end()) - vec.begin();
	}

	template<class T>
	T valg<T>::minElement(const cv::Mat_<T>& vec) {
		return *std::min_element(vec.begin(), vec.end());
	}

	template<class T>
	int valg<T>::minElementIndex(const cv::Mat_<T>& vec) {
		return std::min_element(vec.begin(), vec.end()) - vec.begin();
	}

	template<class T>
	cv::Mat_<T> valg<T>::maxElements(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) {
		cv::Mat_<T> ret(1,vec_1.cols);
		std::transform(vec_1.begin(),vec_1.end(),vec_2.begin(),ret.begin(),ocv::valg<T>::greater);
		return ret;
	}

	template<class T>
	cv::Mat_<T> valg<T>::minElements(const cv::Mat_<T>& vec_1, const cv::Mat_<T>& vec_2) {
		cv::Mat_<T> ret(1,vec_1.cols);
		std::transform(vec_1.begin(),vec_1.end(),vec_2.begin(),ret.begin(),ocv::valg<T>::less);
		return ret;
	}

	template<class T>
	T valg<T>::mean(const cv::Mat_<T>& vec) {
		return (static_cast<T>(1) * std::accumulate(vec.begin(),vec.end(),static_cast<T>(0)) / (vec.end()-vec.begin()));
	}

	template<class T>
	T valg<T>::median(const cv::Mat_<T>& vec) {
		std::vector<T> v2(vec.end()-vec.begin());
		std::copy(vec.begin(),vec.end(),v2.begin());
		std::sort(v2.begin(),v2.end());
		return v2[v2.size() / 2];
	}

	template<class T>
	T valg<T>::stdDev(const cv::Mat_<T>& vec) {
		return valg<T>::deviation(vec,valg<T>::mean(vec));
	}
	
	template<class T>
	T valg<T>::deviation(const cv::Mat_<T>& vec, T t) {
		T squared_sum = std::inner_product(vec.begin(),vec.end(),vec.begin(),static_cast<T>(0));
		T single_sum = std::accumulate(vec.begin(),vec.end(),static_cast<T>(0));
		return std::sqrt((squared_sum - 2*t*single_sum + (vec.end()-vec.begin())*std::pow(t,2)) / (vec.end()-vec.begin()));
	}
	
	template<class T>
	T valg<T>::sumElements(const cv::Mat_<T>& vec) {
		return std::accumulate(vec.begin(),vec.end(),static_cast<T>(0));
	}

	template<class T>
	T valg<T>::euclideanLength(const cv::Mat_<T>& vec) {
		return std::sqrt(std::inner_product(vec.begin(), vec.end(), vec.begin(), static_cast<T>(0), std::plus<T>(), std::multiplies<T>()));
	}
	
	template<class T>
	void valg<T>::zscore(const cv::Mat_<T>& vec, T& mean, T& variance) {
		mean = ocv::valg<T>::mean(vec);
		variance = std::pow(ocv::valg<T>::deviation(vec, mean),2);
	}
	
	
	
	template<class T>
	void valg<T>::applyZScore(cv::Mat_<T>& vec) {
		T mean = ocv::valg<T>::mean(vec);
		T variance = pow(ocv::valg<T>::deviation(vec,mean),2);
		std::transform(vec.begin(),vec.end(),vec.begin(),[&mean,&variance](T t) -> T { return (t - mean) / variance; });
	}

	template<class T>
	void valg<T>::applyZScore(cv::Mat_<T>& vec, const cv::Mat_<T>& mean, const cv::Mat_<T>& variance) {
		std::transform(vec.begin(),vec.end(),mean.begin(),vec.begin(),std::minus<T>());
		std::transform(vec.begin(),vec.end(),variance.begin(),vec.begin(),std::divides<T>());
	}

	template<class T>
	void valg<T>::applyScaleRange(cv::Mat_<T>& vec, const cv::Mat_<T>& shift, const cv::Mat_<T>& scale) {
		std::transform(vec.begin(),vec.end(),scale.begin(),vec.begin(),std::multiplies<T>());
		std::transform(vec.begin(),vec.end(),shift.begin(),vec.begin(),std::plus<T>());
	}

	
	template<class T>
	cv::Mat_<T> valg<T>::append(const cv::Mat_<T>& vec, const cv::Mat_<T>& append) {
		cv::Mat_<T> tmp(1,(vec.end()-vec.begin()) + (append.end()-append.begin()));
		std::copy(vec.begin(),vec.end(),tmp.begin());
		std::copy(append.begin(),append.end(),tmp.begin()+(vec.end()-vec.begin()));
		return tmp;
	}
	
	template class valg<int>;
//	template class valg<size_t>;
	template class valg<float>;
	template class valg<double>;

}

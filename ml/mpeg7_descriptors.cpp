#include "oceancv/ml/mpeg7_descriptors.h"

namespace ocv {

template<class T>
ColorLayoutDescriptor<T>::ColorLayoutDescriptor(int num_y_coeff, int num_c_coeff) : _num_y_coeff(num_y_coeff), _num_c_coeff(num_c_coeff) { }

template<class T>
void ColorLayoutDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && this->valid(img) );
	
	Frame* frame = new Frame(img);
	if(!mask.empty())
		frame->setMaskAll(mask,255,255,0);

	XM::ColorLayoutDescriptor* cld = Feature::getColorLayoutD(frame, _num_y_coeff, _num_c_coeff);
	
	int  *y_coeff = cld->GetYCoeff();
	int *cb_coeff = cld->GetCbCoeff();
	int *cr_coeff = cld->GetCrCoeff();
	
	int c = 0;
	for(int i = 0; i < _num_y_coeff; i++)
		vec(c++) = y_coeff[i];
	for(int i = 0; i < _num_c_coeff; i++)
		vec(c++) = cb_coeff[i];
	for(int i = 0; i < _num_c_coeff; i++)
		vec(c++) = cr_coeff[i];
	
	delete cld;
	delete frame;
	
}

template<class T>
bool ColorLayoutDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC3) && (img.cols > 8 && img.rows > 8) && (_num_y_coeff == 3 || _num_y_coeff == 6 || _num_y_coeff == 10 || _num_y_coeff == 15 || _num_y_coeff == 21 || _num_y_coeff == 28 || _num_y_coeff == 64) && (_num_c_coeff == 3 || _num_c_coeff == 6 || _num_c_coeff == 10 || _num_c_coeff == 15 || _num_c_coeff == 21 || _num_c_coeff == 28 || _num_c_coeff == 64);
}

template<class T>
int ColorLayoutDescriptor<T>::size() const {
	return _num_y_coeff + 2 * _num_c_coeff;
}

template<class T>
std::vector<std::string> ColorLayoutDescriptor<T>::setup() const {
	std::vector<std::string> ret;
	for(int i = 0; i < _num_y_coeff; i++)
		ret.push_back("ColorLayoutDescriptor_y_coeff");
	for(int i = 0; i < 2 * _num_c_coeff; i++)
		ret.push_back("ColorLayoutDescriptor_c_coeff");
	return ret;
}



template<class T>
ColorStructureDescriptor<T>::ColorStructureDescriptor(int size) : _size(size) {}


template<class T>
void ColorStructureDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && this->valid(img));
	
	Frame* frame = new Frame(img);
	if(!mask.empty())
		frame->setMaskAll(mask,255,255,0);

	XM::ColorStructureDescriptor* csd = Feature::getColorStructureD(frame, _size);

	for(size_t i = 0; i < csd->GetSize(); i++) {
		vec(i) = (int) csd->GetElement(i);
	}

	delete csd;
	delete frame;

}

template<class T>
bool ColorStructureDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC3) && (_size == 32 || _size == 64 || _size == 128 || _size == 256);
}

template<class T>
int ColorStructureDescriptor<T>::size() const {
	return _size;
}

template<class T>
std::vector<std::string> ColorStructureDescriptor<T>::setup() const {
	return std::vector<std::string>(this->size(),"ColorStructureDescriptor");
}



template<class T>
DominantColorDescriptor<T>::DominantColorDescriptor(int num_dc, bool normalization_flag, bool variance_flag, bool spatial_flag, int num_bin_0, int num_bin_1, int num_bin_2) : _num_dc(num_dc), _normalization_flag(normalization_flag), _variance_flag(variance_flag), _spatial_flag(spatial_flag), _num_bin_0(num_bin_0), _num_bin_1(num_bin_1), _num_bin_2(num_bin_2) { }


template<class T>
void DominantColorDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && this->valid(img));
	
	Frame* frame = new Frame(img);
	if(!mask.empty())
		frame->setMaskAll(mask,255,255,0);

	XM::DominantColorDescriptor* dcd = Feature::getDominantColorD(frame,_normalization_flag,_variance_flag,_spatial_flag,_num_bin_0,_num_bin_1,_num_bin_2);

	int	ndc = dcd->GetDominantColorsNumber();

	int c = 0;
	vec(c++) = ndc;
	if(_spatial_flag) {
		vec(c++) = dcd->GetSpatialCoherency();
	}
	XM::DOMCOL* domcol = dcd->GetDominantColors();

	for(int ii = 0; ii < ndc; ii++) {
		vec(c++) = domcol[ii].m_Percentage;
		for(int j = 0; j < 3; j++) {
			vec(c++) = domcol[ii].m_ColorValue[j];
		}
		if(_variance_flag) {
			for(int j = 0; j < 3; j++) {
				vec(c++) = domcol[ii].m_ColorVariance[j];
			}
		}
		if(_num_dc > 0 && ii == _num_dc)
			break;
	}

	delete dcd;
	delete domcol;
	delete frame;
	
}

template<class T>
bool DominantColorDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC3);
}

template<class T>
int DominantColorDescriptor<T>::size() const {
	int ret = 1;
	ret += _num_dc * 4;
	if(_spatial_flag)
		ret++;
	if(_variance_flag)
		ret += _num_dc * 3;
	return ret;
}

template<class T>
std::vector<std::string> DominantColorDescriptor<T>::setup() const {
	std::vector<std::string> ret;// = {{"ColorLayoutDescriptor",{}}};
//	size_t start = 0;
	ret.push_back("DominantColorDescriptor_numcols");//,{start++}));
	if(_spatial_flag)
		ret.push_back("DominantColorDescriptor_spatial-flag");//,{start++}));
//	ret.insert(std::pair<std::string,std::vector<size_t>>("DominantColorDescriptorP",{}));
//	ret.insert(std::pair<std::string,std::vector<size_t>>("DominantColorDescriptorC",{}));
	if(_variance_flag)
		ret.push_back("DominantColorDescriptor_variance-flag");//,{}));
	for(int i = 0; i < _num_dc; i++) {
		ret.push_back("DominantColorDescriptor_P");
		ret.push_back("DominantColorDescriptor_C");//].push_back(start++);
		ret.push_back("DominantColorDescriptor_C");//].push_back(start++);
		ret.push_back("DominantColorDescriptor_C");//].push_back(start++);
		if(_variance_flag) {
			for(int j = 0; j < 3; j++)
				ret.push_back("DominantColorDescriptor_V");//].push_back(start++);
		}
	}
	return ret;
}




template<class T>
EdgeHistogramDescriptor<T>::EdgeHistogramDescriptor() { }

template<class T>
void EdgeHistogramDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && this->valid(img));
	
	Frame* frame = new Frame(img);
	if(!mask.empty())
		frame->setMaskAll(mask,255,255,0);

	XM::EdgeHistogramDescriptor* ehd = Feature::getEdgeHistogramD(frame);
	char* de = ehd->GetEdgeHistogramElement();
	for(size_t ui = 0; ui < ehd->GetSize(); ui++) {
		vec(ui) = (int) de[ui];
	}
	
	delete ehd;
	delete frame;
	
}

template<class T>
bool EdgeHistogramDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC3);
}

template<class T>
int EdgeHistogramDescriptor<T>::size() const {
	return 80;
}

template<class T>
std::vector<std::string> EdgeHistogramDescriptor<T>::setup() const {
	return std::vector<std::string>(this->size(),"EdgeHistogramDescriptor");
}



template<class T>
HomogeneousTextureDescriptor<T>::HomogeneousTextureDescriptor(bool layerFlag) : _layerFlag(layerFlag) { }


template<class T>
void HomogeneousTextureDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == size() && this->valid(img));

	Frame* frame = new Frame(img.cols, img.rows, false, true, false);
	frame->setGray(img);
	if(!mask.empty())
		frame->setMaskAll(mask,255,255,0);

	XM::HomogeneousTextureDescriptor* htd = Feature::getHomogeneousTextureD(frame, _layerFlag);
	int* ht_values = htd->GetHomogeneousTextureFeature();

	for(int i = 0; i < size(); i++)
		vec(i) = ht_values[i];

	delete frame;
	delete htd;
	
}

template<class T>
bool HomogeneousTextureDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC1);
}

template<class T>
int HomogeneousTextureDescriptor<T>::size() const {
	if(_layerFlag)
		return 62;
	else
		return 32;
}

template<class T>
std::vector<std::string> HomogeneousTextureDescriptor<T>::setup() const {
	std::vector<std::string> ret;// = {{"HomogeneousTextureDescriptor",{}}};
//	int start = 0;
	for(int i = 0; i < 32; i++)
		ret.push_back("HomogeneousTextureDescriptor_energy");//].push_back(start++);
	if(_layerFlag) {
//		ret.insert(std::pair<std::string,std::vector<size_t>>("HomogeneousTextureDescriptorlayer",{}));
		for(int i = 0; i < 30; i++)
			ret.push_back("HomogeneousTextureDescriptor_deviation");//].push_back(start++);
	}
	return ret;
}



template<class T>
ScalableColorDescriptor<T>::ScalableColorDescriptor(int size) : _size(size) { }

template<class T>
void ScalableColorDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == this->size() && this->valid(img));
	
	Frame* frame = new Frame(img);
	if(!mask.empty())
		frame->setMaskAll(mask,255,255,0);
	
	XM::ScalableColorDescriptor* scd = Feature::getScalableColorD(frame, mask.empty(), _size);
	
	int coeff = (int) scd->GetNumberOfCoefficients();
	for(int i = 0; i < coeff; i++) {
		vec(i) = (int) scd->GetCoefficient(i);
	}

	delete scd;
	delete frame;
	
}

template<class T>
bool ScalableColorDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC3 && (_size == 16 || _size == 32 || _size == 64 || _size == 128 || _size == 256));
}

template<class T>
int ScalableColorDescriptor<T>::size() const {
	return _size;
}

template<class T>
std::vector<std::string> ScalableColorDescriptor<T>::setup() const {
	return std::vector<std::string>(this->size(),"ScalableColorDescriptor");
}



template<class T>
ContourShapeDescriptor<T>::ContourShapeDescriptor(int max_num) : _max_num(max_num) { }

template<class T>
void ContourShapeDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == this->size() && this->valid(img));
	
	Frame* frame = new Frame(img);
	if(!mask.empty())
		frame->setMaskAll(mask,255,255,0);
	
	size_t idx = 0;
	
	XM::ContourShapeDescriptor* csd = Feature::getContourShapeD(frame);
	
	unsigned long lgcv[2];
	csd->GetGlobalCurvature(lgcv[0], lgcv[1]);
	vec(idx++) = (int)lgcv[0];
	vec(idx++) = (int)lgcv[1];

	unsigned int num = csd->GetNoOfPeaks();
	if(num > 0) {
		unsigned long lpcv[2];
		csd->GetPrototypeCurvature(lpcv[0], lpcv[1]);
		vec(idx++) = (int)lpcv[0];
		vec(idx++) = (int)lpcv[1];
	} else {
		vec(idx++) = 0;
		vec(idx++) = 0;
	}

	vec(idx++) = (int) csd->GetHighestPeakY();

	for(unsigned int i = 0; i < num; i++) {
		unsigned short xp, yp;
		csd->GetPeak(i, xp, yp);
		vec(idx++) = xp;
		vec(idx++) = yp;
	}
	for(int i = num; i < _max_num; i++) {
		vec(idx++) = 0;
		vec(idx++) = 0;
	}
	
	delete csd;
	delete frame;
	
}

template<class T>
bool ContourShapeDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC3);
}

template<class T>
int ContourShapeDescriptor<T>::size() const {
	return 5+_max_num*2;
}

template<class T>
std::vector<std::string> ContourShapeDescriptor<T>::setup() const {
	std::vector<std::string> ret;
	for(int i = 0; i < 4; i++)
		ret.push_back("ContourShapeDescriptor_curvature");
	ret.push_back("ContourShapeDescriptor_peak-height");
	for(int i = 0; i < _max_num; i++) {
		ret.push_back("ContourShapeDescriptor_curvature");
		ret.push_back("ContourShapeDescriptor_curvature");
	}
	return ret;
}



template<class T>
RegionShapeDescriptor<T>::RegionShapeDescriptor() { }

template<class T>
void RegionShapeDescriptor<T>::extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask) const {
	assert(vec.cols == this->size() && this->valid(img));
	
	Frame* frame = new Frame(img);
	if(!mask.empty())
		frame->setMaskAll(mask,255,255,0);
	
	XM::RegionShapeDescriptor* rsd = Feature::getRegionShapeD(frame);
	
	size_t idx = 0;
	for(int p = 0; p < 12; p++) {
		for(int r = 0; r < 3; r++) {
			vec(idx++) = rsd->GetRealValue(p,r);
		}
	}

	delete rsd;
	delete frame;
	
}

template<class T>
bool RegionShapeDescriptor<T>::valid(const cv::Mat& img) const {
	return (img.type() == CV_8UC3);
}

template<class T>
int RegionShapeDescriptor<T>::size() const {
	return 36;
}

template<class T>
std::vector<std::string> RegionShapeDescriptor<T>::setup() const {
	return std::vector<std::string>(this->size(),"RegionShapeDescriptor");
}




template class ScalableColorDescriptor<float>;
template class ScalableColorDescriptor<double>;

template class HomogeneousTextureDescriptor<float>;
template class HomogeneousTextureDescriptor<double>;

template class EdgeHistogramDescriptor<float>;
template class EdgeHistogramDescriptor<double>;

template class DominantColorDescriptor<float>;
template class DominantColorDescriptor<double>;

template class ColorStructureDescriptor<float>;
template class ColorStructureDescriptor<double>;

template class ColorLayoutDescriptor<float>;
template class ColorLayoutDescriptor<double>;

}

#pragma once

#include <map>
#include "MPEG7FexLib/Feature.h"
#include "oceancv/ml/feature_descriptor.h"

namespace ocv {

/**
 * (C)olor (L)ayout (D)escriptor for extraction from an CV_8UC3 image.
 * "The extraction for the descriptor consists of four stages;
 * image partitioning, dominant color selection, DCT transform,
 * and non-linear quantization of the zigzag-scanned DCT coefficients. (...)
 * In the first stage, an input picture is partitioned into 64 blocks.
 * The size of the each block is W/8 x H/8, where W and H denote the
 * width and height of an input picture, respectively.
 * In the second stage, a single dominant color is selected in each block
 * to build a tiny image whose size is 8x8 (...)
 * In the third stage, each of the three components (Y, Cb and Cr) is
 * transformed by 8x8 DCT, and we obtain three sets of DCT coefficients.
 * (In the fourth stage) A few low frequency coefficients are extracted
 * using zigzag scanning and quantized to form the Color Layout descriptor
 * for a still picture."
 * See Sikora: "The MPEG-7 visual standard for content description-an overview"
 **/
template<class T>
class ColorLayoutDescriptor : public FeatureDescriptor<T> {

public:
	ColorLayoutDescriptor(int num_y_coeff = 64, int num_c_coeff = 28);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	int _num_y_coeff;
	int _num_c_coeff;
	
};


/**
 * (C)olor (S)tructure (D)escriptor for extraction from an CV_8UC3 image.
 **/
template<class T>
class ColorStructureDescriptor : public FeatureDescriptor<T> {
	
public:

	// size  can be: 32, 64, 128 or 256
	ColorStructureDescriptor(int size = 64);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	int _size;
	
};


/**
 * (D)ominant (C)olor (D)escriptor for extraction from an CV_8UC3 image.
 **/
template<class T>
class DominantColorDescriptor : public FeatureDescriptor<T> {

public:
	
	/**
	 * @param num_dc maximum number of dominant colors to return.
	 * @param normalization_flag whether to normalize values to MPEG-7 range
	 *                            true -> (color:0-32, variance:0-1, weight:0-32)
	 *                            false -> (color:0-255, variance:as computed, weight:0-100)
	 * @param variance_flag whether to compute the variance
	 * @param spatial_flag whether to compute the spatial coherency
	 * @param num_bin_0 bin numbers to quantize the dominant color values to
	 * @param num_bin_1 bin numbers to quantize the dominant color values to
	 * @param num_bin_2 bin numbers to quantize the dominant color values to
	 */
	DominantColorDescriptor(int num_dc = 5, bool normalization_flag = true, bool variance_flag = true, bool spatial_flag = true, int num_bin_0 = 32, int num_bin_1 = 32, int num_bin_2 = 32);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	int _num_dc;
	bool _normalization_flag, _variance_flag, _spatial_flag;
	int _num_bin_0, _num_bin_1, _num_bin_2;
	
};


/**
 * (E)dge (H)istogram (D)escriptor for extraction from an CV_8UC3 image.
 **/
template<class T>
class EdgeHistogramDescriptor : public FeatureDescriptor<T> {

public:
	EdgeHistogramDescriptor();
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;
	
};


/**
 * (H)omogeneous (T)exture (D)escriptor for extraction from an CV_8UC1 (gray) image.
 **/
template<class T>
class HomogeneousTextureDescriptor : public FeatureDescriptor<T> {

public:
	HomogeneousTextureDescriptor(bool layer_flag = true);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	bool _layerFlag;
	
};


/**
 * (S)calable (C)olor Descriptor for extraction from an CV_8UC3 image.
 **/
template<class T>
class ScalableColorDescriptor : public FeatureDescriptor<T> {

public:
	// Size can be 16, 32, 64, 128 or 256
	ScalableColorDescriptor(int size = 256);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	int _size;
	
};


// TODO: XM::ContourShapeDescriptor* getContourShapeD( Frame* f );
/**
 * (S)calable (C)olor Descriptor for extraction from an CV_8UC3 image.
 **/
template<class T>
class ContourShapeDescriptor : public FeatureDescriptor<T> {

public:
	ContourShapeDescriptor(int max_num);
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

private:
	int _max_num;
	
};


// TODO: XM::RegionShapeDescriptor* getRegionShapeD( Frame* f );
template<class T>
class RegionShapeDescriptor : public FeatureDescriptor<T> {

public:
	RegionShapeDescriptor();
	void extract(const cv::Mat& img, cv::Mat_<T>& vec, const cv::Mat& mask = cv::Mat()) const;
	bool valid(const cv::Mat& img) const;
	int size() const;
	std::vector<std::string> setup() const;

};

}

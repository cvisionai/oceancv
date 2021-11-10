#include "oceancv/ml/h2som.h"

using namespace std;

int main() {

	cv::Mat_<float> features(5000,3);
	std::vector<int> classes(features.rows,0);
	
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 10);
	
	float x,y,z;
	
	for(int i = 0; i < features.rows; i++) {
		x = dist(gen);
		y = dist(gen);
		z = dist(gen);
		features(i,0) = x;
		features(i,1) = y;
		features(i,2) = z;
		if(y < 4 && z < 4)
			classes[i] = 1;
		else if(y > 4 && z > 4)
			classes[i] = 1;
	}
	
	int iterations = 100;
	
	ocv::H2SOM<float> h(features, features.rows*iterations, new ocv::EuclideanMetric<float>(), new ocv::ExponentialRate<float>(0.99,0.01,features.rows*iterations), new ocv::ExponentialRate<float>(3,0.01,features.rows*iterations), 3, 8);
	
	// Get centroids
	cv::Mat_<float> centroids = h.centroids();
	
	// Find closest vector for each centroid
	size_t bmu;
	vector<int> bmus(features.rows,0);
	vector<int> centroid_c0(features.rows,0);
	vector<int> centroid_c1(features.rows,0);
	
	for(int i = 0; i < features.rows; i++) {
		
		bmu = ocv::malg<float>::bestMatchIndex(centroids,features.row(i),ocv::EuclideanMetric<float>());
		
		bmus[i] = bmu;
		if(classes[i] == 1)
			centroid_c1[bmu]++;
		else
			centroid_c0[bmu]++;
	}
	
	// Compare to ground truth
	size_t tp = 0,fp = 0,fn = 0,tn = 0;
	for(size_t i = 0; i < centroid_c0.size(); i++) {
		if(centroid_c0[i] > centroid_c1[i]) {
			// BMU represents the negative class
			tn += centroid_c0[i];
			fn += centroid_c1[i];
		} else {
			// BMU represents the positive class
			tp += centroid_c1[i];
			fp += centroid_c0[i];
		}
	}
	std::cout << tp << " " << fp << " " << fn << " " << tn << std::endl;
	std::cout << 1.f * tp / (tp + fp) << " " << 1.f * tp / (tp + fn) << std::endl;
	
}

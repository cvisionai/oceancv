#include "oceancv/ml/feature_descriptor.h"

namespace ocv {

template<class T>
FeatureDescriptor<T>::FeatureDescriptor() { }

template class FeatureDescriptor <float>;
template class FeatureDescriptor <double>;

}

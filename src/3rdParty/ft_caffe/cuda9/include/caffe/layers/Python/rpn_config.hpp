#ifndef __FACETHINK_CAFFE_LAYERS_RPN_CONFIG_HPP__
#define __FACETHINK_CAFFE_LAYERS_RPN_CONFIG_HPP__

#include "caffe/core/common.hpp"
#include <vector>

namespace facethink {

  template <typename Dtype>
  class RPNConfig {
  public:
    RPNConfig() {  
      BBOX_REG = true;
      
      HAS_RPN = true;

      SCALES = std::vector<int> { 600 };
      MAX_SIZE = 1000;

      PIXEL_MEANS = std::vector<Dtype> { Dtype(102.9801), Dtype(115.9465), Dtype(122.7717) };

      TRAIN_BBOX_NORMALIZE_MEANS = std::vector<Dtype>{Dtype(0.0), Dtype(0.0), Dtype(0.0), Dtype(0.0)};
      TRAIN_BBOX_NORMALIZE_STDS  = std::vector<Dtype>{Dtype(0.1), Dtype(0.1), Dtype(0.2), Dtype(0.2)};
   
      RPN_NMS_THRESH     = Dtype(0.7);
      RPN_PRE_NMS_TOP_N  = 6000;
      RPN_POST_NMS_TOP_N = 300;
      RPN_MIN_SIZE       = 16;
      
      NMS_THRESH  = Dtype(0.5);
      CONF_THRESH = Dtype(0.7);
    }
    
  public:   
    bool HAS_RPN;
    bool BBOX_REG;
    
    std::vector<int> SCALES;
    int MAX_SIZE;

    std::vector<Dtype> PIXEL_MEANS;

    std::vector<Dtype> TRAIN_BBOX_NORMALIZE_MEANS;
    std::vector<Dtype> TRAIN_BBOX_NORMALIZE_STDS; 
    
    Dtype RPN_NMS_THRESH;
    int RPN_PRE_NMS_TOP_N;
    int RPN_POST_NMS_TOP_N;
    int RPN_MIN_SIZE;
 
    Dtype NMS_THRESH;
    Dtype CONF_THRESH;
  }; // class RPNConfig

} // namespace facethink


#endif

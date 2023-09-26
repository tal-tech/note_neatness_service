#ifndef __FACETHINK_CAFFE_CORE_BASE_LAYER_HPP__
#define __FACETHINK_CAFFE_CORE_BASE_LAYER_HPP__

#include "caffe/core/blob.hpp"
#include "caffe/layers/Python/rpn_config.hpp"

namespace facethink {

  template <typename Dtype>
  class BaseLayer {
  public:
    explicit BaseLayer(const std::string& name=""):name_(name) { }

    virtual inline void SetUp(const std::vector<std::shared_ptr<Blob<Dtype> > >& inputs,
		       const std::vector<std::shared_ptr<Blob<Dtype> > >& outputs) {
      SetInputs(inputs);
      SetOutputs(outputs);
    }

    virtual inline void SetInputs(const std::vector<std::shared_ptr<Blob<Dtype> > > & inputs) {
      inputs_ = inputs;
    }
    virtual inline void SetOutputs(const std::vector<std::shared_ptr<Blob<Dtype> > > & outputs) {
      outputs_ = outputs;
    }
    virtual inline void SetWeights(const std::vector<std::shared_ptr<Blob<Dtype> > > & weights) {
      weights_ = weights;
    }
    

    virtual void InitLayer() {};

    virtual void Forward() {
      ForwardShape();
      ForwardComputation();
    }
    virtual void ForwardShape() = 0;
    virtual void ForwardComputation() = 0;
    
     //only for proposal Layer
    virtual void ForwardWithConfig(const RPNConfig<Dtype>& rpn_config) {
      ForwardShape();
      ForwardComputationWithConfig(rpn_config);
    }
    virtual void ForwardComputationWithConfig(const RPNConfig<Dtype>& rpn_config) { }
    ////////////////////////////////////////////
    
    inline const std::vector<std::shared_ptr<Blob<Dtype> > >& inputs() const { return inputs_; }
    inline const std::vector<std::shared_ptr<Blob<Dtype> > >& outputs() const { return outputs_; }


    inline const std::string& name() const{ return name_; }
    
    virtual inline std::string type() const { return "Base"; }
    virtual inline std::string param_string() const { return "Base_NULL"; }
    
    virtual inline bool has_weights() const { return false; }

  protected:
    virtual inline bool CheckBlobs() const { return true; };
    
  protected:
    std::vector<std::shared_ptr<Blob<Dtype> > > inputs_, outputs_, weights_;
    std::string name_;

    DISABLE_COPY_AND_ASSIGN(BaseLayer);
  }; // class BaseLayer

} // namespace facethink

#endif

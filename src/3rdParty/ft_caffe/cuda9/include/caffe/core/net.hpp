#ifndef __FACETHINK_CAFFE_CORE_NET_HPP__
#define __FACETHINK_CAFFE_CORE_NET_HPP__

#include "caffe/core/common.hpp"
#include "caffe/core/base_layer.hpp"
#include <map>
#include <set>
#include <string>

namespace facethink {

  template <typename Dtype>
  class EXPORT_CLASS Net {
  public:
    explicit Net(const std::string& name=""):name_(name) { }

    void Forward();
    void Forward(const RPNConfig<Dtype>& rpn_config);  //only for proposal Layer

    void AppendLayer(const std::shared_ptr<BaseLayer<Dtype> >& layer);
    void InsertBlob(const std::string& blob_name, std::shared_ptr<Blob<Dtype> >& blob);

    inline std::shared_ptr<BaseLayer<Dtype> > layer(const std::string& layer_name) {

		//std::cout << layers_.size() << std::endl;
      for (size_t i = 0; i < layers_.size(); ++i) {
	if (!layers_[i]->name().compare(layer_name)) {
	  return layers_[i];
	}
      }
      BOOST_LOG_TRIVIAL(error)<< "Net: can not find the layer by this name ["<<layer_name<<"]";
      return nullptr;
    }

    inline std::shared_ptr<Blob<Dtype> > blob(const std::string& blob_name) {
      if (blobs_.find(blob_name) == blobs_.end()) {
	BOOST_LOG_TRIVIAL(error)<< "Net: can not find the blob by this name ["<<blob_name<<"]";
	return nullptr;
      }
      return blobs_[blob_name];
    }


    inline const std::string& name() const { return name_; }
    inline void set_name(const std::string& name) { name_ = name; }

    // only for debug
    void PrintBlobs() {
      for (auto & p : blobs_) {
	std::cout << p.first <<"   " <<p.second->shape_string()<< std::endl;
      }
    }

    void PrintLayers() {
      for (auto & p : layers_) {
	std::cout << p->param_string()<< std::endl;
      }
    }

  private:
    std::string name_;
    std::map<std::string, std::shared_ptr<Blob<Dtype> > > blobs_;
    std::vector<std::shared_ptr<BaseLayer<Dtype > > > layers_;

    DISABLE_COPY_AND_ASSIGN(Net);
  }; // class Net

} // namespace facethink

#endif

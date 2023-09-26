#ifndef __FACETHINK_CAFFE_LAYERS_NET_BUILD_HPP__
#define __FACETHINK_CAFFE_LAYERS_NET_BUILD_HPP__

#include "caffe/core/net.hpp"
#include "caffe/proto/caffe.pb.h"

namespace facethink {
  namespace facethinkcaffe{

  template <typename Dtype>
  class EXPORT_CLASS NetBuilder {
  public:
    std::shared_ptr<Net<Dtype> > Create(const std::string& proto_txt_file,
					const std::string& proto_binary_file);

	std::shared_ptr<Net<Dtype> > Create(const std::string& proto_file, int key = 0xaa, const int len_max = 10);

  private:

	  void SetLog();

    void ParseNetParameterText(const caffe::NetParameter& net_param,
				std::shared_ptr<Net<Dtype> >& net);

    void ParseNetParameterBinary(const caffe::NetParameter& net_param,
				  std::shared_ptr<Net<Dtype> >& net);

    void ParseInputBlobs(const caffe::NetParameter& net_param,
			  std::shared_ptr<Net<Dtype> >& net);

    void InsertOutputBlobsOfLayer(const caffe::LayerParameter& layer_param,
				  std::shared_ptr<Net<Dtype> >& net);

    void SetUpAndAppendLayer(const caffe::LayerParameter& layer_param,
			     std::shared_ptr<Net<Dtype> >& net,
			     std::shared_ptr<BaseLayer<Dtype> >& layer);

  }; // class netbuilder

}
} // namespace facethink

#endif

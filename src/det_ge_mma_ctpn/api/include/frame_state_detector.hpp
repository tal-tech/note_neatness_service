#ifndef __FACETHINK_API_FRAME_STATE_DETECTOR_HPP__
#define __FACETHINK_API_FRAME_STATE_DETECTOR_HPP__

#include <string>
#include "config.hpp"
#include "det_ge_mma_ctpn.hpp"
#include <Python.h>

namespace facethink {
    namespace detgemmactpn {

    class FrameStateDetector: public MmaCtpnDetection {
    public:

	  explicit FrameStateDetector(
		  const std::string& det_model_file,
		  const std::string& config_file);

		virtual ~FrameStateDetector();
	  virtual int detection(const cv::Mat& img, float& score_0, float& score_1, float& interval_mean, int& white_space_length, bool& ltor, int& final_boxes_len, bool is_rgb_format);

    protected:

    private:

        PyObject * pModule = NULL;
        PyObject * pFunc = NULL;
		PyObject* pDict = NULL;
		PyObject* pClass = NULL;
		PyObject* pInstance = NULL;
	  Config config_;
    };

}
}

#endif

#include "frame_state_detector.hpp"
#include "det_ge_mma_ctpn.hpp"
#include <string>

#ifdef WIN32  
#include <Windows.h>  
#else  
#include <stdio.h>  
#include <unistd.h>  
#endif  
namespace facethink {
  using namespace detgemmactpn;
	
  MmaCtpnDetection::MmaCtpnDetection(void) {

	}

  MmaCtpnDetection::~MmaCtpnDetection(void) {

	}


  MmaCtpnDetection* MmaCtpnDetection::create(
		const std::string& det_model_file,
		const std::string& config_file) {

		return new FrameStateDetector(
			det_model_file,
			config_file);
	}

}

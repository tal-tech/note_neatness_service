#include "frame_state_detector.hpp"
//#include <numpy/ndarrayobject.h>

#include<chrono>
#ifdef WIN32
#include <io.h>
#endif


namespace facethink {
	namespace detgemmactpn {

//        class PyThreadStateLock {
//        public:
//            PyThreadStateLock(void) {
//                state = PyGILState_Ensure();
//            }
//            ~PyThreadStateLock(void) {
//                PyGILState_Release(state);
//            }
//        private:
//            PyGILState_STATE state;
//        };

		FrameStateDetector::FrameStateDetector(
			const std::string& det_model_file,
			const std::string& config_file) {

#ifdef WIN32
			if (_access(config_file.c_str(), 0) != -1) {
				config_.ReadIniFile(config_file);
			}
#else
			if (access(config_file.c_str(), 0) != -1) {
				config_.ReadIniFile(config_file);
			}
#endif
            std::string python_path = "sys.path.append('" + det_model_file + "')";
            Py_Initialize();
            //PyEval_InitThreads();
			assert(Py_IsInitialized());

            PyRun_SimpleString("import sys");
            PyRun_SimpleString(python_path.c_str());

            pModule = PyImport_ImportModule("det_line");
            assert(pModule);

            //PyEval_ReleaseThread(PyThreadState_Get());
            //PyThreadStateLock PyThreadLock;

            PyObject* pDict = PyModule_GetDict(pModule);
            assert(pDict);

            pClass = PyDict_GetItemString(pDict, "PythonTextDetector");
            assert(pClass);

            pInstance = PyObject_CallObject(pClass, NULL);
            assert(pInstance);

            pFunc = PyObject_GetAttrString(pInstance, "analyst");
            assert(pFunc);
        }

        FrameStateDetector::~FrameStateDetector() {
            Py_DECREF(pModule);
            Py_DECREF(pClass);
            Py_DECREF(pInstance);
            Py_DECREF(pFunc);
            PyGILState_Ensure();
            Py_Finalize();
            config_.StopFileLogging();
		}

		int FrameStateDetector::detection(const cv::Mat& img, float& score_0, float& score_1, float& interval_mean, int& white_space_length, bool& ltor, int& final_boxes_len, bool is_rgb_format) {
            //PyThreadStateLock PyThreadLock;
			int status = 0;
			if (img.empty() || img.channels() != 3) {
				BOOST_LOG_TRIVIAL(error) << "Input image must has 3 channels.";
				return -1;
			}

			BOOST_LOG_TRIVIAL(debug) << "Face ID Det Net: start.";
			auto time_start = std::chrono::steady_clock::now();
            PyObject *pArgs = PyTuple_New(4);
            PyObject *PyList  = PyList_New(img.rows * img.cols * 3);

            int count = 0;
            for (int y = 0; y < img.rows; y++) {
                for (int x = 0; x < img.cols; x++) {
                    for (int c = 0; c < 3; c++) {
                        if (is_rgb_format) {
                            PyList_SetItem(PyList, count++, Py_BuildValue("i", (u_int8_t)img.at<cv::Vec3b>(y, x)[2 - c]));
                        } else {
                            PyList_SetItem(PyList, count++, Py_BuildValue("i", (u_int8_t)img.at<cv::Vec3b>(y, x)[c]));
                        }
                    }
                }
            }
            PyTuple_SetItem(pArgs, 0, PyList);
            PyTuple_SetItem(pArgs, 1, Py_BuildValue("i", img.rows));
            PyTuple_SetItem(pArgs, 2, Py_BuildValue("i", img.cols));
            PyTuple_SetItem(pArgs, 3, Py_BuildValue("i", 3));

            PyObject *pReturn = PyObject_CallObject(pFunc, pArgs);
            if (pReturn == NULL) {
                std::cout << "PyObject_CallObject error" << std::endl;
                BOOST_LOG_TRIVIAL(error) << "PyObject_CallObject error";
                Py_DECREF(pArgs);
                Py_DECREF(PyList);
                return -2;
            }
            Py_DECREF(pArgs);
            Py_DECREF(PyList);

            PyArg_ParseTuple(pReturn, "f|f|f|i|b|i", &score_0, &score_1, &interval_mean, &white_space_length, &ltor, &final_boxes_len);

            BOOST_LOG_TRIVIAL(info) << "Det Net Cost Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time_start).count() << " ms";

			return status;
		}

	}
}

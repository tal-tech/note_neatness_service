#include "det_ge_mma_ctpn.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <thread>
#include"opencv2/opencv.hpp"

using namespace facethink;

int send_count = 0;
MmaCtpnDetection *faceAttr_detector = nullptr;
cv::Mat img;

void send_thread(int idx) {
    while (1) {
        float score_0, score_1, interval_mean;
        int white_space_length, final_boxes_len;
        bool ltor;
        auto time_start = std::chrono::steady_clock::now();
        faceAttr_detector->detection(img, score_0, score_1, interval_mean, white_space_length, ltor, final_boxes_len, true);
        auto time_end = std::chrono::steady_clock::now();
        std::cout << send_count++ << " MMA ctpn Det Net Cost Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count() << " ms" << std::endl;

    }
}

int main(int argc, char *argv[]){

	if (argc < 4) {
		std::cerr << "Usage: " << argv[0]
			<< " det_model"
			<< " image path"
			<< " config file path" << std::endl;
		return 1;
	}

    const std::string det_model = argv[1];
    const std::string img_path = argv[2];
    const std::string config_file_path = argv[3];

    faceAttr_detector = MmaCtpnDetection::create(
            det_model,
            config_file_path);

	img = cv::imread(img_path);
	if (img.data == 0) {
		std::cout << "read image failed:" << img_path << std::endl;
		return -1;
	}

    std::vector<std::vector<int>> bbox;


	int n = 1;
    std::thread threads[n];
    // spawn 10 threads:
    for (int i = 0; i< n; ++i) {
        threads[i] = std::thread(send_thread, i);
    }

    for (auto& th : threads) th.join();


    getchar();
	return 0;
}

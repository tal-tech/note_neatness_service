#include "det_ge_mma_ctpn.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <iomanip>
#include <vector>

using namespace facethink;
using namespace std;


int main(int argc, char *argv[]) {

	if (argc < 4) {
		std::cerr << "Usage: " << argv[0]
			<< " det_model"
			<< " image path"
			<< " config file path" << std::endl;
		return 1;
	}

	const std::string det_model = argv[1];
	const std::string images_folder_path = argv[2];
	const std::string config_file_path = argv[3];

	std::string file_list = images_folder_path + "list.txt";
	std::ifstream fin(file_list);
	if (!fin) {
		std::cout << "read file list failed" << std::endl;
		return -1;
	}
	//read images and annotations

	MmaCtpnDetection *faceAttr_detector = MmaCtpnDetection::create(
		det_model,
		config_file_path);

	std::string file_string;
	int total = 0;
	int idx = 0;
	while (std::getline(fin, file_string))
	{
		std::istringstream istr(file_string);
		std::string img_path, ano_path;
		istr >> img_path;
		istr >> ano_path;
		img_path = images_folder_path + img_path;
		ano_path = images_folder_path + ano_path;
		//std::cout << img_path << std::endl;
		//std::cout << ano_path << std::endl;

		cv::Mat img = cv::imread(img_path);
		if (img.data == 0) {
			std::cout << "read image failed:" << img_path << std::endl;
			return -1;
		}

		auto time_start = std::chrono::steady_clock::now();

		float score_0, score_1, interval_mean;
		int white_space_length, final_boxes_len;
		bool ltor;
		faceAttr_detector->detection(img, score_0, score_1, interval_mean, white_space_length, ltor, final_boxes_len, true);
		auto time_end = std::chrono::steady_clock::now();

		if (idx != 0) {
            total += std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
		}

		idx++;
		std::cout << "Cost Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count() << " ms" << std::endl;
	}
    std::cout << "Avg Cost Time: " << total / (idx - 1) << " ms" << std::endl;
	return 0;
}
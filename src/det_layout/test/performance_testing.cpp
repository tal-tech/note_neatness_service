#include "det_layout.hpp"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace facethink;

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0]
              << " image path"
              << " config file path" << std::endl;
    return 1;
  }
  const std::string images_folder_path = argv[1];
  std::string file_list = images_folder_path + "list.txt";
  std::ifstream fin(file_list);
  if (!fin) {
    std::cout << "read file list failed" << std::endl;
    return -1;
  }
  std::string file_string;
  while (std::getline(fin, file_string)) {
    std::istringstream istr(file_string);
    std::string img_path, ano_path;
    istr >> img_path;
    istr >> ano_path;
    img_path = images_folder_path + img_path;
    ano_path = ano_path;
    //std::cout << img_path << std::endl;
    //std::cout << ano_path << std::endl;
    cv::Mat img = cv::imread(img_path);
    if (img.data == 0) {
      std::cout << "read image failed:" << img_path << std::endl;
      continue;
    }
    auto start = std::chrono::steady_clock::now();

    det_layout::NeatTarget nt;
    int ret = det_layout::IsNoteNeat(img, nt);
    auto cost_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
    // std::cout << "cost time :" <<
    // cost_time <<
    // " ms " <<
    // "label is " << ano_path <<
    // " det is " << ret << std::endl;
    std::size_t slice_pos = img_path.find("images");
    auto partial_path = img_path.substr(slice_pos);
    std::cout << partial_path << "," << cost_time << "," << ano_path << "," << ret << "," << nt.mean_width << std::endl;
  }
  return 0;
}

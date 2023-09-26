#include "det_layout.hpp"
#include <iostream>
#include <fstream>

using namespace facethink;

// todo: 建议使用gtest
// todo: 对给定输入图片，在unit test里要检查输出是否正确
int main(int argc, char *argv[])
{
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0]
              << " image path"
              << " config file path" << std::endl;
    return 1;
  }
  const std::string good_img = argv[1];
  const std::string config_file = argv[2];
  cv::Mat img = cv::imread(good_img); //test image
  if (img.data == 0) {
    std::cout << "read image failed:" << good_img << std::endl;
    return -1;
  }

  det_layout::Init(config_file);
  det_layout::NeatTarget nt;
  std::cout << det_layout::IsNoteNeat(img, nt) << std::endl;

  return 0;
}

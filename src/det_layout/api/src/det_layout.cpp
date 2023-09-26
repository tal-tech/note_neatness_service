#include "det_layout.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <vector>
#include <tuple>
#include <numeric>
#include <algorithm>
#include <set>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace facethink
{

namespace det_layout
{

void DilateImage(const cv::Mat& img, cv::Mat& result)
{
  auto intermediate = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(4, 2));
  cv::dilate(img, result, intermediate);
}

typedef std::tuple<double, double, double> Proportion;

Proportion CalcProportion(const cv::Mat& bin_img, const cv::Mat& stats)
{
  // todo: assert bin_img是黑白图
  // todo: assert bin_img w>0 h>0，否则后边可能除0
  std::vector<unsigned char> img_data;
  if(bin_img.isContinuous()) {
    img_data.assign(bin_img.datastart, bin_img.dataend);
  }
  // todo: 使用reserve避免多次分配内存
  else {
    for(int i = 0; i < bin_img.rows; ++i) {
      img_data.insert(img_data.end(), bin_img.ptr<unsigned int>(i), bin_img.ptr<unsigned int>(i) + bin_img.cols);
    }
  }
  auto text_part = std::accumulate(img_data.begin(), img_data.end(), 0) / 255;

  int left_x = 0, top_y = 0, right_x = 0, bottom_y = 0;
  // todo: 检查不完备，可能越界
  // todo: 这里应该使用第0行，还是第1行？
  if(stats.rows > 0) {
    left_x = stats.at<int>(cv::Point(0, 1));
    top_y = stats.at<int>(cv::Point(1, 1));
    right_x = stats.at<int>(cv::Point(2, 1)) + left_x;
    bottom_y = stats.at<int>(cv::Point(3, 1)) + top_y;
  }
  // todo: 多次随机访问Mat元素，可改写以提升效率
  for(int i = 1; i < stats.rows; ++i) {
    int x = stats.at<int>(cv::Point(0, i));
    int y = stats.at<int>(cv::Point(1, i));
    int w = stats.at<int>(cv::Point(2, i));
    int h = stats.at<int>(cv::Point(3, i));
    if (x < left_x) left_x = x;
    if (y < top_y) top_y = y;
    if (x + w > right_x) right_x = x + w;
    if (y + h > bottom_y) bottom_y = y + h;
  }
  double relative_bg_part = (right_x - left_x) * (bottom_y - top_y);
  double bg_part = bin_img.rows * bin_img.cols;
  double ab_proportion = text_part / bg_part;
  double rl_proportion = text_part / relative_bg_part;
  double text_block_proportion = (right_x - left_x) * (bottom_y - top_y) / bg_part;
  return std::make_tuple(ab_proportion,
                         rl_proportion,
                         text_block_proportion);
}

// todo: 这是常用函数，应该放在专门地方，比如AI_Engine/utils，不应该每个工程里都放
void ShowImgWithRect(cv::Mat& img, const cv::Mat& stats)
{
  // ignore the first block
  for(int i = 1; i < stats.rows; i++) {
    int x = stats.at<int>(cv::Point(0, i));
    int y = stats.at<int>(cv::Point(1, i));
    int w = stats.at<int>(cv::Point(2, i));
    int h = stats.at<int>(cv::Point(3, i));

    cv::Scalar color(0, 0, 255);
    cv::Rect rect(x, y , w, h);

    // cv::rectangle(after_dilate, rect, color);
    cv::rectangle(img, rect, color);
  }

  cv::imshow("test", img);
  cv::waitKey(0);
}

// todo: iou是intersecton over union，为什么只除了a的面积？
// todo: 放到专门的utils里
// todo: 计算width/height，不应该+1
double Iou(const cv::Rect& a, const cv::Rect& b)
{
  auto inter_left_x = std::max(a.x, b.x);
  auto inter_top_y = std::max(a.y, b.y);
  auto inter_right_x = std::min(a.x + a.width, b.x + b.width);
  auto inter_bottom_y = std::min(a.y + a.height, b.y + b.height);
  double inter_w = std::max(0, inter_right_x - inter_left_x + 1);
  double inter_h = std::max(0, inter_bottom_y - inter_top_y + 1);
  return (inter_w * inter_h) / a.area();
}

// todo: iou是intersecton over union，为什么只除了b的面积？
// todo: 放到专门的utils里
// todo: a, b 简单类型要传值，不传引用
// todo: 计算w or h，不应该+1
// todo: 只应保留一个iou函数实现，其他iou函数内部应调用第一个
double Iou(const cv::Mat& stats, const size_t& a, const size_t& b)
{
  auto a_x = stats.ptr<int>(a)[0];
  auto a_y = stats.ptr<int>(a)[1];
  auto a_w = stats.ptr<int>(a)[2];
  auto a_h = stats.ptr<int>(a)[3];
  auto b_x = stats.ptr<int>(b)[0];
  auto b_y = stats.ptr<int>(b)[1];
  auto b_w = stats.ptr<int>(b)[2];
  auto b_h = stats.ptr<int>(b)[3];
  auto inter_topleft_x = std::max(a_x, b_x);
  auto inter_topleft_y = std::max(a_y, b_y);
  auto inter_bottomright_x = std::min(a_x + a_w, b_x + b_w);
  auto inter_bottomright_y = std::min(a_y + a_h, b_y + b_h);
  double inter_w = std::max(0, inter_bottomright_x - inter_topleft_x + 1);
  double inter_h = std::max(0, inter_bottomright_y - inter_topleft_y + 1);

  return (inter_w * inter_h) / (b_w * b_h);
}

// todo: 放到专门的utils里
// todo: 什么是total_area?
// todo: 0.2不能hard code
// todo: stats应该叫boxes或rects
// todo: 在排序后，就不需要remove_if遍历所有boxes
void Nms(std::vector<cv::Rect>& stats, int total_area, double threshold, std::vector<cv::Rect>& new_stats)
{
  std::sort(stats.begin(), stats.end(), [](cv::Rect& a, cv::Rect& b) {
    return a.area() > b.area();
  });
  stats.erase(std::remove_if(stats.begin(), stats.end(), [&total_area](const cv::Rect& i) {
    return i.area() > 0.2 * total_area;
  }), stats.end());
  while(!stats.empty()) {
    auto biggest = stats[0];
    new_stats.push_back(biggest);
    stats.erase(stats.begin());
    if(stats.empty()) break;
    stats.erase(std::remove_if(stats.begin(), stats.end(), [&biggest, &threshold](const cv::Rect& j) {
      auto o = Iou(j, biggest);
      return o > threshold;
    }), stats.end());
  }
}

// todo: 放到专门的utils里
// todo: 不应重新实现nms，应调用前一个
void Nms(const cv::Mat& stats, int total_area, double threshold, std::vector<int>& result)
{
  std::vector<int> indices(stats.rows - 1);
  int seed = 0;
  std::generate(indices.begin(), indices.end(), [&seed] {return ++seed;});

  std::sort(indices.begin(), indices.end(), [&stats](size_t a, size_t b){
    auto a_w = stats.at<int>(cv::Point(2, a));
    auto a_h = stats.at<int>(cv::Point(3, a));
    auto a_area = a_w * a_h;

    auto b_w = stats.at<int>(cv::Point(2, b));
    auto b_h = stats.at<int>(cv::Point(3, b));
    auto b_area = b_w * b_h;
    return a_area > b_area;
  });

  indices.erase(std::remove_if(indices.begin(), indices.end(), [&stats, &total_area](const int& i) {
    auto area = stats.ptr<int>(i)[2] * stats.ptr<int>(i)[3];
    return area > 0.2 * total_area;
  }), indices.end());

  while(!indices.empty()) {
    auto biggest = indices[0];
    result.push_back(biggest);
    indices.erase(indices.begin());
    if(indices.empty()) break;
    indices.erase(std::remove_if(indices.begin(), indices.end(), [&stats, &biggest, &threshold](const int& i) {
      auto o = Iou(stats, biggest, i);
      return o > threshold;
    }), indices.end());
  }
}

// todo: all_height提前分配大小，不要动态增长
// todo: 对于复杂类型，for auto -> for const auto& ?
void ConnectBlock(const cv::Mat& stats, std::vector<int>& indices, std::vector<cv::Rect>& new_stats)
{
  // arrange block by y coordinate of center
  std::sort(indices.begin(), indices.end(), [&stats](const int& a, const int& b) {
    auto a_y = stats.ptr<int>(a)[1];
    auto b_y = stats.ptr<int>(b)[1];
    auto a_center_y = a_y + static_cast<double>(stats.ptr<int>(a)[3]) / 2;
    auto b_center_y = b_y + static_cast<double>(stats.ptr<int>(b)[3]) / 2;
    if(a_center_y == b_center_y) {
      return a_y < b_y;
    }
    else {
      return a_center_y < b_center_y;
    }
  });

  std::vector<int> all_height;
  for(auto i : indices) {
    all_height.push_back(stats.ptr<int>(i)[3]);
  }

  double height_sum = std::accumulate(all_height.begin(), all_height.end(), 0);
  double mean_height =  height_sum / all_height.size();
  double target_height = mean_height * 0.5;

  std::vector<std::set<int>> line_group;

  for(auto i : indices) {
    std::set<int> tmp_group;
    int tmp_left_x = stats.ptr<int>(i)[0];
    int tmp_right_x = stats.ptr<int>(i)[0] + stats.ptr<int>(i)[2];
    for(auto j : indices) {
      double centre_y_i = stats.ptr<int>(i)[1] + static_cast<double>(stats.ptr<int>(i)[3]) / 2;
      double centre_y_j = stats.ptr<int>(j)[1] + static_cast<double>(stats.ptr<int>(j)[3]) / 2;
      auto right_x_j = stats.ptr<int>(j)[0] + stats.ptr<int>(j)[2];
      auto left_x_j = stats.ptr<int>(j)[0];
      if(std::abs(centre_y_j - centre_y_i) < mean_height) {
        if(std::abs(tmp_left_x - right_x_j) < target_height ||
           std::abs(left_x_j - tmp_right_x) < target_height) {
          tmp_group.insert(i);
          tmp_group.insert(j);
          tmp_left_x = std::min(tmp_left_x, left_x_j);
          tmp_right_x = std::max(tmp_right_x, right_x_j);
        }
      }
    }

    if(!tmp_group.empty()) {
      line_group.push_back(tmp_group);
    }
  }

  for(auto line : line_group) {
    if(line.empty()) continue;
    auto line_begin = *line.begin();
    auto left_x = stats.ptr<int>(line_begin)[0];
    auto right_x = left_x + stats.ptr<int>(line_begin)[2];
    auto top_y = stats.ptr<int>(line_begin)[1];
    auto bottom_y = top_y + stats.ptr<int>(line_begin)[3];

    for(auto i : line) {
      int x = stats.at<int>(cv::Point(0, i));
      int y = stats.at<int>(cv::Point(1, i));
      int w = stats.at<int>(cv::Point(2, i));
      int h = stats.at<int>(cv::Point(3, i));
      if (x < left_x) left_x = x;
      if (y < top_y) top_y = y;
      if (x + w > right_x) right_x = x + w;
      if (y + h > bottom_y) bottom_y = y + h;
    }
    cv::Rect row(left_x, top_y, right_x - left_x, bottom_y - top_y);
    new_stats.push_back(row);
  }
}

// todo: global var 不要定义在文件中间
double g_area_p = 0.3333;
double g_text_p = 0.02;
int g_box_count = 30;
double g_width_std = 40;

void Init(const std::string& config_file)
{
  boost::property_tree::ptree pt;
  boost::property_tree::ini_parser::read_ini(config_file, pt);

  g_area_p = pt.get<double>("text_block_proportion", g_area_p);
  g_text_p = pt.get<double>("text_proportion", g_text_p);
  g_box_count = pt.get<int>("text_block_count", g_box_count);
  g_width_std = pt.get<int>("text_block_width_std", g_width_std);

  bool log_save = pt.get<bool>("log_save", true);
  if (log_save)
  {
    boost::log::add_file_log
    (
      boost::log::keywords::auto_flush = true,
      boost::log::keywords::file_name = "det_.layoutog",
      boost::log::keywords::format = "[%TimeStamp%]: %Message%"
    );
    boost::log::add_common_attributes();
  }

  int log_level = pt.get<int>("log_level", 4);
  boost::log::core::get()->set_filter(boost::log::trivial::severity >= log_level);
}

// todo: 避免hard code numbers
// todo: auto -> const auto&
bool IsNoteNeat(const cv::Mat& img, NeatTarget& neat_target)
{
  cv::Mat cp_img = img.clone();
    // Convert to grey image
  cv::Mat gray_scale;
  cv::cvtColor(cp_img, gray_scale, cv::COLOR_BGR2GRAY);

  cv::Mat bin_img;
  cv::threshold(gray_scale, bin_img, 127, 255, cv::THRESH_BINARY_INV);

  cv::Mat after_dilate;
  DilateImage(bin_img, after_dilate);

  cv::Mat labels;
  cv::Mat stats;
  cv::Mat centroids;

  cv::connectedComponentsWithStats(after_dilate, labels, stats, centroids);

  auto proportion = CalcProportion(bin_img, stats);

  std::vector<int> indices;
  Nms(stats, bin_img.rows * bin_img.cols, 0.5, indices);
  
  std::vector<cv::Rect> new_stats;
  ConnectBlock(stats, indices, new_stats);

  if(new_stats.empty()) return false;

  std::vector<cv::Rect> final_stats;

  Nms(new_stats, bin_img.rows * bin_img.cols, 0.5, final_stats);

  if(final_stats.empty()) return false;

  auto height_sum = 0;
  auto width_sum = 0;
  for(auto i : final_stats) {
    height_sum += i.height;
    width_sum += i.width;
  }
  double mean_height = height_sum / static_cast<double>(final_stats.size());
  neat_target.mean_width = width_sum / static_cast<double>(final_stats.size());
  double deviation = 0;
  for(auto i : final_stats) {
    deviation += std::pow((i.height - mean_height), 2);
  }

  std::sort(final_stats.begin(), final_stats.end(), [](const cv::Rect& a, const cv::Rect& b) {
    return a.x < b.x;
  });

  // todo: deviation, std_deviation 都应该叫std，这行挪到上面
  double std_deviation = std::pow(deviation / final_stats.size(), 0.5);

  double area_p = std::get<2>(proportion);
  double text_p = std::get<1>(proportion);

  neat_target.box_count = final_stats.size();
  neat_target.std_height = std_deviation;
  neat_target.text_area_p = area_p;
  neat_target.text_block_p = text_p;

  // todo: 每个条件单列一行，注释解释含义
  if(area_p > g_area_p && text_p >= g_text_p && final_stats.size() >= g_box_count && std_deviation <= g_width_std) {
    return true;
  }
  else {
    return false;
  }
}

}

}
#ifndef __FACETHINK_API_DET_LAYOUT_HPP__
#define __FACETHINK_API_DET_LAYOUT_HPP__

#include <string>
#include <math.h>
#include <opencv2/opencv.hpp>

#ifdef WIN32
#ifdef DLL_EXPORTS
#define EXPORT_CLASS   __declspec(dllexport)
#define EXPORT_API  extern "C" __declspec(dllexport)
#define EXPORT_CLASS_API

#else
#define EXPORT_CLASS   __declspec(dllimport )
#define EXPORT_API  extern "C" __declspec(dllimport )
#endif
#else
#define EXPORT_CLASS
#define EXPORT_API  extern "C" __attribute__((visibility("default")))
#define EXPORT_CLASS_API __attribute__((visibility("default")))
#endif

namespace facethink
{

namespace det_layout
{

struct NeatTarget
{
  double mean_width = 0;  //文本框平均宽度
  double std_height = 0;  //文本框高度方差
  double text_area_p = 0;  //文本绝对占比
  double text_block_p = 0;  //文本相对占比
  int box_count = 0;  //文本框数量
};

/*
todo: 函数功能描述
    输入参数描述
*/
// todo: 匈牙利表示法
EXPORT_API void Init(const std::string& config_file);

/*
判断一张图片是否字迹工整
输入参数：
  img: BGR格式图像
  mean_width: 返回平均宽度
*/
// todo: 返回使用引用代替裸指针
EXPORT_API bool IsNoteNeat(const cv::Mat& img, NeatTarget& neat_target);

/* todo: 使用面向对象接口
class DocumentAnalyzer{
public:
  ...
  bool loadConfig(...);
  bool isNeat(...);
  void setAreaRatioThreshold(...);
  ...
private:
  static int _areaRatioThreshold;
  ...
};
*/

}

}

#endif // __FACETHINK_API_DET_LAYOUT_HPP__

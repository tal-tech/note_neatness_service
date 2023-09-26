///////////////////////////////////////////////////////////////////////////////////////
///  Copyright (C) 2017, TAL AILab Corporation, all rights reserved.
///
///  @file: det_ge_mma_ctpn.hpp
///  @brief
///  @details 最初版本
//
//
///  @version 1.0.0.0
///  @author Hua Qingbo
///  @date 2019-04-22
///
///  @see 使用参考：demo.cpp
///
///////////////////////////////////////////////////////////////////////////////////////
#ifndef __FACETHINK_API_DET_MMA_CTPN_HPP__
#define __FACETHINK_API_DET_MMA_CTPN_HPP__
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
namespace facethink {
	class EXPORT_CLASS MmaCtpnDetection {
	public:
		EXPORT_CLASS_API explicit MmaCtpnDetection(void);
		EXPORT_CLASS_API virtual ~MmaCtpnDetection(void);
		/*!
		\brief SDK初始化函数，必须先于任何其他SDK函数之前调用，create的重载函数。
		@param [in] det_model_file python文件夹路径。
		@param [in] config_file 指定SDK对应的参数配置文件路径,详情见config.ini文件。
		@return
		@remarks 初始化函数需要读取模型等文件，需要一定时间等待。
		*/
		EXPORT_CLASS_API static MmaCtpnDetection* create(
				const std::string& det_model_file,
				const std::string& config_file);

		/// \brief 检测人脸属性。
		/// @param [in] img 输入的图像数据，支持如下两种种种格式:
		/// - 1.BGR图：img为一维数组，每个元素（字节）表示一个像素点的单通道取值，三个连续元素表示一个像素点的三通道取值，顺序为BGR。
		/// - 2.RGB图：此时is_rgb_format应设置为true。
		/// @param [out] score_0，area_p，文本框面积占比。
		/// @param [out] score_1，relative_p，笔画面积占比。
		/// @param [out] interval_mean 行间距
		/// @param [out] white_space_length  大括号剩余长度
        /// @param [out] ltor 是否从左往右书写
        /// @param [out] final_boxes_len  3级文本数量
		/// @param [in] is_rgb_format 是否是rgb格式
		/// @return
		/// @retval 0 检测成功。
		/// @retval -1 图片为空或图片通道不为3。
		/// @retval -2 模型运行出错。
		EXPORT_CLASS_API virtual int detection(const cv::Mat& img, float& score_0, float& score_1, float& interval_mean, int& white_space_length, bool& ltor, int& final_boxes_len, bool is_rgb_format) = 0;
	};
}
#endif
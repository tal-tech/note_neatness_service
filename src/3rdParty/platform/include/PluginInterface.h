///////////////////////////////////////////////////////////////////////////////////////
///  Copyright (C) 2017, TAL AILab Corporation, all rights reserved.
///
///  @file: PluginInterface.h
///  @brief 算法插件接口
///  @details
//
//
///  @version 0.0.0.1
///  @author Yan hui
///  @date 2018-05-3
///  
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef __PLUGIN_INTERFACE_H__
#define __PLUGIN_INTERFACE_H__

#include <string>
#include <map>
#include <math.h>
#include <opencv2/opencv.hpp>

namespace Plugin {

	class IPlugin {
	public:

		/*!
		\brief 多元化命令接口。
		@param [in] strCommand 指定要调用的算法接口。
		@param [in] args 算法接口运行所依赖的参数。
		@param [out] out 运算结果。
		@return
		@remarks 同步调用。
		*/
		virtual  int InvokeMethod(const char* strCommand,
			const std::map<std::string, std::string>& args,
			std::map<std::string, std::string>& out
		) = 0;



		
		/*!
		\brief 多元化命令接口。
		@param [in] strCommand 指定要调用的算法接口。
		@param [in] args 算法接口运行所依赖的参数。
		@param [out] out 运算结果。
		@return
		@remarks 同步调用。
		*/
		virtual  int InvokeMethod(const char* strCommand,
			const char* args,
			char* out
		) = 0;

		
		/*!
		\brief 获取插件信息。
		@param [in] args 算法接口运行所依赖的参数。
		@param [out] out 运算结果。
		@return
		@remarks 同步调用。
		*/
		virtual  int GetInterface(
			const std::map<std::string, std::string>& args,
			std::map<std::string, std::string>& out
		) = 0;
	

	};

}

#endif

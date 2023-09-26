///////////////////////////////////////////////////////////////////////////////////////
///  Copyright (C) 2017, TAL AILab Corporation, all rights reserved.
///
///  @file: PluginMgrInterface.h
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

#ifndef __PLAT_FORM_INTERFACE_H__
#define __PLAT_FORM_INTERFACE_H__

#include <string>
#include <map>
#include <math.h>
#include <opencv2/opencv.hpp>
//#include "facethink.grpc.pb.h"
//#include "facethink.pb.h"

namespace PlatForm {
		
	class IPlatForm {
	public:

		/*!
		\brief 多元化命令接口。
		@param [in] strCommand 指定要调用的算法接口。
		@param [in] args 算法接口运行所依赖的参数。
		@param [out] out 运算结果。
		@return
		@remarks 同步调用。
		*/
		virtual int PlatFormInvokeMethod(const char* strId,
			const char*  strCommand,
			const std::map<std::string, std::string>& args,
			std::map<std::string, std::string>& out
		) = 0;

		virtual int PlatFormInvokeMethod(const char*  strId,
			const char*  strCommand,
			const char*  args,
			char*  out
		) = 0;



		/*!
		\brief 申请一块内存区域。
		@param [in] nSize 申请的内存大小。
		@param [out] pBuff 返回申请内存的地址。
		@return
		@remarks 同步调用。
		*/
		virtual int PlatFormMalloc(int nSize,void** pBuff) = 0;


		/*!
		\brief 释放一块内存区域。
		@param [in] pBuff 要释放的内存的地址。
		@return
		@remarks 同步调用。
		*/
		virtual int  PlatFormFree(void* pBuff) = 0;


		/*!
		\brief 获取插件信息。
		@param [in] args 算法接口运行所依赖的参数。
		@param [out] out 运算结果。
		@return
		@remarks 同步调用。
		*/
		virtual int GetPlatFormInterface(
			const std::map<std::string, std::string>& args,
			std::map<std::string, std::string>& out
		) = 0;


	};

}

#endif

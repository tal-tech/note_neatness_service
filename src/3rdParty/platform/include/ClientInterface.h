///////////////////////////////////////////////////////////////////////////////////////
///  Copyright (C) 2017, TAL AILab Corporation, all rights reserved.
///
///  @file: PluginInterface.h
///  @brief 算法平台接口-Client端 
///  @details
//
//
///  @version 0.0.0.1
///  @author Yan hui
///  @date 2018-05-3
///  
///
///////////////////////////////////////////////////////////////////////////////////////

#ifndef __CLIENT_INTERFACE_H__
#define __CLIENT_INTERFACE_H__

#include <string>
#include <map>
#include <math.h>


namespace PlatForm {

	class IClient {
	public:

		/*!
		\brief 多元化命令接口。
		@param [in] strCommand 指定要调用的接口。
		@param [in] args 运算参数。
		@param [out] out 运算结果。
		@return
		@remarks 同步调用。
		*/
		virtual int Invoke(std::string strCommand,
			std::map<std::string, std::string> args,
			std::map<std::string, std::string>& out
		) = 0;


		/*!
		\brief 获取Client信息。
		@param [in] args 获取信息所依赖的参数。
		@param [out] out 平台信息。
		@return
		@remarks 同步调用。
		*/
		virtual int GetInterface(
			std::map<std::string, std::string> args,
			std::map<std::string, std::string>& out
		) = 0;
	

	};

}

#endif

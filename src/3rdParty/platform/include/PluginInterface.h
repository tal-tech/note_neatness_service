///////////////////////////////////////////////////////////////////////////////////////
///  Copyright (C) 2017, TAL AILab Corporation, all rights reserved.
///
///  @file: PluginInterface.h
///  @brief �㷨����ӿ�
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
		\brief ��Ԫ������ӿڡ�
		@param [in] strCommand ָ��Ҫ���õ��㷨�ӿڡ�
		@param [in] args �㷨�ӿ������������Ĳ�����
		@param [out] out ��������
		@return
		@remarks ͬ�����á�
		*/
		virtual  int InvokeMethod(const char* strCommand,
			const std::map<std::string, std::string>& args,
			std::map<std::string, std::string>& out
		) = 0;



		
		/*!
		\brief ��Ԫ������ӿڡ�
		@param [in] strCommand ָ��Ҫ���õ��㷨�ӿڡ�
		@param [in] args �㷨�ӿ������������Ĳ�����
		@param [out] out ��������
		@return
		@remarks ͬ�����á�
		*/
		virtual  int InvokeMethod(const char* strCommand,
			const char* args,
			char* out
		) = 0;

		
		/*!
		\brief ��ȡ�����Ϣ��
		@param [in] args �㷨�ӿ������������Ĳ�����
		@param [out] out ��������
		@return
		@remarks ͬ�����á�
		*/
		virtual  int GetInterface(
			const std::map<std::string, std::string>& args,
			std::map<std::string, std::string>& out
		) = 0;
	

	};

}

#endif

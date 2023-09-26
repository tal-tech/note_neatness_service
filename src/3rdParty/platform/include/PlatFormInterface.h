///////////////////////////////////////////////////////////////////////////////////////
///  Copyright (C) 2017, TAL AILab Corporation, all rights reserved.
///
///  @file: PluginMgrInterface.h
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
		\brief ��Ԫ������ӿڡ�
		@param [in] strCommand ָ��Ҫ���õ��㷨�ӿڡ�
		@param [in] args �㷨�ӿ������������Ĳ�����
		@param [out] out ��������
		@return
		@remarks ͬ�����á�
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
		\brief ����һ���ڴ�����
		@param [in] nSize ������ڴ��С��
		@param [out] pBuff ���������ڴ�ĵ�ַ��
		@return
		@remarks ͬ�����á�
		*/
		virtual int PlatFormMalloc(int nSize,void** pBuff) = 0;


		/*!
		\brief �ͷ�һ���ڴ�����
		@param [in] pBuff Ҫ�ͷŵ��ڴ�ĵ�ַ��
		@return
		@remarks ͬ�����á�
		*/
		virtual int  PlatFormFree(void* pBuff) = 0;


		/*!
		\brief ��ȡ�����Ϣ��
		@param [in] args �㷨�ӿ������������Ĳ�����
		@param [out] out ��������
		@return
		@remarks ͬ�����á�
		*/
		virtual int GetPlatFormInterface(
			const std::map<std::string, std::string>& args,
			std::map<std::string, std::string>& out
		) = 0;


	};

}

#endif

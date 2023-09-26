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

#ifndef __PLUGIN_MGR_INTERFACE_H__
#define __PLUGIN_MGR_INTERFACE_H__

#include <string>
#include <map>
#include <math.h>

namespace Plugin {

	class IPluginMgr {
	public:

		/*!
		\brief ��Ԫ������ӿڡ�
		@param [in] strCommand ָ��Ҫ���õ��㷨�ӿڡ�
		@param [in] args �㷨�ӿ������������Ĳ�����
		@param [out] out ��������
		@return
		@remarks ͬ�����á�
		*/
		int InvokeMethod(std::string strCommand,
			std::map<std::string, std::string> args,
			std::map<std::string, std::string>& out
		);
		

		/*!
		\brief ����һ���ڴ�����
		@param [in] nSize ������ڴ��С��
		@param [out] pBuff ���������ڴ�ĵ�ַ��
		@return
		@remarks ͬ�����á�
		*/
		 int PluginMalloc(int nSize,void** pBuff);


		/*!
		\brief �ͷ�һ���ڴ�����
		@param [in] pBuff Ҫ�ͷŵ��ڴ�ĵ�ַ��
		@return
		@remarks ͬ�����á�
		*/
		 int  PluginFree(void* pBuff);


		/*!
		\brief ��ȡ�����Ϣ��
		@param [in] args �㷨�ӿ������������Ĳ�����
		@param [out] out ��������
		@return
		@remarks ͬ�����á�
		*/
		 int GetInterface(
			std::map<std::string, std::string> args,
			std::map<std::string, std::string>& out
		);


	};

}

#endif

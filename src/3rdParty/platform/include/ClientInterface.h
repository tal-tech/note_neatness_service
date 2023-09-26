///////////////////////////////////////////////////////////////////////////////////////
///  Copyright (C) 2017, TAL AILab Corporation, all rights reserved.
///
///  @file: PluginInterface.h
///  @brief �㷨ƽ̨�ӿ�-Client�� 
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
		\brief ��Ԫ������ӿڡ�
		@param [in] strCommand ָ��Ҫ���õĽӿڡ�
		@param [in] args ���������
		@param [out] out ��������
		@return
		@remarks ͬ�����á�
		*/
		virtual int Invoke(std::string strCommand,
			std::map<std::string, std::string> args,
			std::map<std::string, std::string>& out
		) = 0;


		/*!
		\brief ��ȡClient��Ϣ��
		@param [in] args ��ȡ��Ϣ�������Ĳ�����
		@param [out] out ƽ̨��Ϣ��
		@return
		@remarks ͬ�����á�
		*/
		virtual int GetInterface(
			std::map<std::string, std::string> args,
			std::map<std::string, std::string>& out
		) = 0;
	

	};

}

#endif

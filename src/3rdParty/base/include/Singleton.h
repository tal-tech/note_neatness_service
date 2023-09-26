#ifndef _SINGLETON_H_
#define _SINGLETON_H_
#ifdef WIN32
#include "windows.h"
#else
#endif
#include "CLock.h"
namespace base {
	namespace CSingleton {
		// ������(����ģʽ)  
		template<class T>
		class CSingleton
		{
		private:
			// ������Щ����Ҫ��Ϊprivate  
			CSingleton() {}  // ��ֹ���� new CSingleton()��������ʵ���ķ�ʽ  

							// ��ֹ����  
			CSingleton(const CSingleton&) {}
			CSingleton& operator=(const CSingleton& instance)
			{
				return *this;
			}

			// ��������ҲҪ����Ϊprivate  
			~CSingleton() {}

			// �Զ�������(Ŀ�����ڳ������ʱ �Զ�������������ʵ��)  
			class CGarbo
			{
			public:
				~CGarbo()
				{
					if (CSingleton::m_pInstance != NULL)
					{
						delete CSingleton::m_pInstance;
						m_pInstance = NULL;
					}
				}
			};
		public:
			// �ٽ�����Դ(������̵߳ķ��ʳ�ͻ)  
			//static CCriticalSection m_cs;  
			static T* getInstance()
			{
				if (NULL == m_pInstance)
				{
					base::CLock::Lock lock(&m_cs);

					if (NULL == m_pInstance)
					{
						m_pInstance = new T;
					}
				}

				return m_pInstance;
			}
		private:
			// ����  
			static T* m_pInstance;

			// �Զ�����  
			static CGarbo m_garbo;
			// ������  
			static base::CLock::CCriticalSection m_cs;
		};

		// ��̬������ʵ����
		template<class T> T* CSingleton<T>::m_pInstance = NULL;
		template<class T> base::CLock::CCriticalSection CSingleton<T>::m_cs;
	}
}
#endif  
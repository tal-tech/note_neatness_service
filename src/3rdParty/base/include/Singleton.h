#ifndef _SINGLETON_H_
#define _SINGLETON_H_
#ifdef WIN32
#include "windows.h"
#else
#endif
#include "CLock.h"
namespace base {
	namespace CSingleton {
		// 单例类(懒汉模式)  
		template<class T>
		class CSingleton
		{
		private:
			// 下面这些函数要设为private  
			CSingleton() {}  // 防止出现 new CSingleton()这样生成实例的方式  

							// 禁止拷贝  
			CSingleton(const CSingleton&) {}
			CSingleton& operator=(const CSingleton& instance)
			{
				return *this;
			}

			// 析构函数也要设置为private  
			~CSingleton() {}

			// 自动析构类(目的是在程序结束时 自动析构掉单例的实例)  
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
			// 临界区资源(避免多线程的访问冲突)  
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
			// 单例  
			static T* m_pInstance;

			// 自动析构  
			static CGarbo m_garbo;
			// 互斥锁  
			static base::CLock::CCriticalSection m_cs;
		};

		// 静态变量的实例化
		template<class T> T* CSingleton<T>::m_pInstance = NULL;
		template<class T> base::CLock::CCriticalSection CSingleton<T>::m_cs;
	}
}
#endif  
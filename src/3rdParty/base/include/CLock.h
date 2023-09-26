#ifndef _LOCK_H_
#define _LOCK_H_
#ifdef WIN32
#include "windows.h"
#else
#include <pthread.h>
#endif
#include <iostream>
namespace base {
	namespace CLock {
		struct CCriticalSection
		{
		public:
			CCriticalSection()
			{
#ifdef WIN32
				InitializeCriticalSectionAndSpinCount(&m_sect,
					0x80000400);
#else
				pthread_mutex_init(&m_sect, NULL);
#endif
			}

			~CCriticalSection()
			{
#ifdef WIN32
				::DeleteCriticalSection(&m_sect);
#else
				pthread_mutex_destroy(&m_sect);
#endif
			}
			void Lock()
			{
#ifdef WIN32
				::EnterCriticalSection(&m_sect);
#else
				pthread_mutex_lock(&m_sect);
#endif
			}
			void UnLock()
			{
#ifdef WIN32
				::LeaveCriticalSection(&m_sect);
#else
				pthread_mutex_unlock(&m_sect);
#endif
			}
		private:
#ifdef WIN32
			CRITICAL_SECTION m_sect;
#else
			pthread_mutex_t m_sect;
#endif
		};

		// 自己实现的锁  
		class Lock
		{
		public:
			// 构造函数  
			Lock(CCriticalSection* pCs) :m_pCs(pCs)
			{
				m_pCs->Lock();
			}

			// 析构函数  
			~Lock()
			{
				m_pCs->UnLock();
			}
		private:
			CCriticalSection* m_pCs;
		};
	}
}
#endif  
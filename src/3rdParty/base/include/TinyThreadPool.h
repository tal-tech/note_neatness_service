#ifndef TINY_THREAD_POOL
#define TINY_THREAD_POOL 
#ifdef WIN32
#include "windows.h"
#else
#define WINAPI 
#include <semaphore.h>
#endif
#include <queue>
#include "CLock.h"
namespace base {
	namespace Thread {
		typedef  unsigned long(*WorkProc)(void*);
		struct ThreadToolParam
		{
		public:
			WorkProc pWork;
			void* pThreadFuncParam;

		};

		class TinyThreadPool
		{
		public:
			TinyThreadPool(void);
			~TinyThreadPool(void);
		public:

			void Open(int nNormalNum, int MaxNum);
			void Close();
			void Add(void* pTask);
			void Clear();

		private:

			void* GetTask();
#ifdef WIN32
			static  unsigned long WINAPI  ThreadFunc(void* pParam);
#else
			static	void* ThreadFunc(void* pParam);
#endif

		private:
			void* m_NewTaskHandle;
			int m_ThreadNum; //
			int m_bExit;

			std::deque<void*> m_TaskDeque;
			std::vector<int>  m_ThreadId;
			base::CLock::CCriticalSection m_cs;

#ifndef WIN32
		public:
			sem_t m_binSem;
#endif

		};
	}
}
#endif
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <queue>
#include <memory>

#include "rui.h"
#include <errno.h>
#include <pthread.h>


namespace rui {
	class threadtask
	{
	public:
		virtual ~threadtask(){}
		virtual void run() = 0;
	};

	class threadpool
	{
	private:
		threadpool();
		~threadpool();

		threadpool(const threadpool&);
		threadpool& operator=(const threadpool&);
		
		void entry(void);
		static void* routine(void* args) ;
		
		bool mb_shutdown;
		
		std::queue<std::unique_ptr<threadtask>> mq_task;
		pthread_mutex_t m_mutex_lock;
		pthread_cond_t m_cond_lock;

		int mi_thread_count;

	public:
		static threadpool& _instance()
		{
			static threadpool instance ;
			return instance ;
		}
		
		bool start(const int threadCount = 5000 );
		
		void add_task(std::unique_ptr<threadtask> ptask);
	};
}
#endif


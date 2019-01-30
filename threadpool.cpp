#include "threadpool.h"

namespace rui {
	threadpool::threadpool():
		mb_shutdown(false)
	{
		pthread_mutex_init( &m_mutex_lock, NULL );
		pthread_cond_init( &m_cond_lock, NULL );
	}

	threadpool::~threadpool()
	{
		mb_shutdown = true ;

		pthread_cond_broadcast( &m_cond_lock );

//		对于智能指针，没有删除指针操作。因此std::queue，map，vector等，都不需要循环删除各个元素指针。
//		while( !mq_task.empty( ) )
//		{
//			rlog << "mq_task.size() = " << mq_task.size() << std::endl;
//			mq_task.front().release();
//			mq_task.pop();
//		}

		pthread_mutex_destroy( &m_mutex_lock ) ;
		pthread_cond_destroy( &m_cond_lock ) ;
		
		rlog << "threadpool quit ..." << std::endl;
	}

	bool threadpool::start(const int threadCount)
	{
		mi_thread_count = threadCount;

		for(int i = 0; i < mi_thread_count; ++i)
		{
			pthread_t tid ;
			if ( pthread_create(&tid, NULL, routine, this) != 0 )
			{
				rlog << "pthread_create() error : " << strerror( errno ) << std::endl;
				return false ;
			}
		}

		return true ;
	}

	void* threadpool::routine(void* arg)
	{
		pthread_detach(pthread_self());

		if ( arg == NULL )
		{
			rlog << "argument = NULL" << std::endl ;
			return	NULL ;
		}

		threadpool* p_this = reinterpret_cast<threadpool*>( arg );
		p_this->entry( );

		return NULL;
	}

	void threadpool::entry(void)
	{
		while(true)
		{
			pthread_mutex_lock(&m_mutex_lock);
			while(mq_task.empty() )
			{
				if(mb_shutdown)
				{
					pthread_mutex_unlock(&m_mutex_lock);
					pthread_exit(NULL);
				}

				pthread_cond_wait(&m_cond_lock, &m_mutex_lock);
			}

			auto ptask = std::move(mq_task.front());
			mq_task.pop();

			pthread_mutex_unlock(&m_mutex_lock);

			try
			{
				ptask.get()->run() ;
				ptask.release();
			}
			catch( ... )
			{
				rlog << "exception occurs ..." << std::endl ;
			}
		}
	}

	void threadpool::add_task(std::unique_ptr<threadtask> ptask)
	{
		pthread_mutex_lock(&m_mutex_lock);
		mq_task.push(std::move(ptask));
		pthread_mutex_unlock(&m_mutex_lock);
		pthread_cond_signal(&m_cond_lock);
	}

}

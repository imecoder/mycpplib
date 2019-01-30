#ifndef _TASK_H_
#define _TASK_H_

#include "rui.h"
#include "net.h"
#include "lock.h"
#include "threadpool.h"

namespace rui {
	class task
	{
	public :
		task()
		{
			init();
			pthread_mutex_init(&m_lock, NULL) ;
		}
		
		virtual ~task()
		{
			clear();
			pthread_mutex_destroy(&m_lock) ;
		}

		void init(int i_fd = -1, int i_type = -1, time_t t = time(NULL) )
		{
			mi_fd = i_fd;
			mi_type = i_type;
			m_time=t;
			mv_data.clear();
			mb_closed = false;
		}

		void clear()
		{
			if ( mi_fd != -1 )
			{
				rui_close(mi_fd) ;
				mi_fd = -1;
			}
			mi_type = -1;
			m_time=time(NULL);
			mv_data.clear();
			mb_closed = true;
		}

		bool is_online()
		{
			rui::mutex rlock(m_lock);
			
			if ( mi_fd == -1 )
				return false ;

			return true ;
		}

	public:
		pthread_mutex_t m_lock ;
		int mi_fd;
		int mi_type;
		time_t m_time;
		std::vector<char> mv_data ;
		bool mb_closed ;
	} ;
}
#endif


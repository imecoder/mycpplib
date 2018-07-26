#ifndef _LOCK_H_
#define _LOCK_H_

#include <pthread.h>
#include "rui.h"

namespace rui {

	class rwlock
	{
	public:
		enum READ_WRITE_TYPE
		{
			READ,
			WRITE
		};

	public:
		rwlock( pthread_rwlock_t &lock, READ_WRITE_TYPE type )
			: m_lock( lock )
		{
			switch( type )
			{
				case READ:
				{
					if( pthread_rwlock_rdlock( &lock ) != 0 )
						success = false;
					else
						success = true;
					break;
				}

				case WRITE:
				{
					if( pthread_rwlock_wrlock( &lock ) != 0 )
						success = false;
					else
						success = true;
					break;
				}
			}
		}

		~rwlock( )
		{
			if( success )
			{
				if( pthread_rwlock_unlock( &m_lock ) != 0 )
					rlog << "rwlock.pthread_rwlock_unlock() error" << endl;
			}
		}

	private:
		pthread_rwlock_t &m_lock;
		bool success;
		time_t m_ltime;
	};

	class mutex
	{
	public:
		mutex( pthread_mutex_t &lock )
			: m_lock( lock )
		{
			pthread_mutex_lock( &lock );
		}

		~mutex( )
		{
			pthread_mutex_unlock( &m_lock );
		}

	private:
		pthread_mutex_t& m_lock;
	};
}

#endif


#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <sys/epoll.h>
#include <vector>

using namespace std ;

namespace rui {

	enum STATUS
	{
		UNUSED = -1,
		CLOSED = 0,
		RUNNING = 1,
	} ;

	#define EPOLL_WAIT_MAX_SIZE 100000
	#define EPOLL_WAIT_TIME 500
	#define EPOLL_LISTEN_MAX_SIZE 128

	class epoll
	{
		public:
			epoll();
			virtual ~epoll() ;

			void event_add(const int i_socket);
			void event_del(const int i_socket);

			bool create( const short sh_port = -1 ) ;
			int accept(void);
			int wait(void);

		public:
			short msh_port ;
			int mi_fd;
			int mi_epoll;
			epoll_event mresult_event[EPOLL_WAIT_MAX_SIZE] ;
	} ;
}

#endif


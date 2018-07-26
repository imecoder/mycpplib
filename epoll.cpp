#include <sys/epoll.h>
#include <sys/types.h>
#include "epoll.h"
#include "rui.h"
#include "net.h"

namespace rui {
	epoll::epoll() :
		msh_port(-1),
		mi_fd(-1),
		mi_epoll(-1)
	{
	}

	epoll::~epoll()
	{
		if ( msh_port != -1 )
		{
			if ( mi_fd != -1 )
			{
				close(mi_fd);
				rlog << "close epoll socket(" << mi_fd << ")" << endl ;
			}
		}

		if ( mi_epoll != -1 )
		{
			close(mi_epoll);
			rlog << "close epoll(" << mi_epoll << ")" << endl ;
		}
	}

	void epoll::event_add(const int i_socket )
	{
		epoll_event o_event = {0,{0}};
		o_event.data.fd = i_socket;
		o_event.events = EPOLLIN |EPOLLET |EPOLLERR ;
		::epoll_ctl(mi_epoll, EPOLL_CTL_ADD, i_socket, &o_event);

		rlog << "socket(" << i_socket << ") ---> epoll(" << mi_epoll << ")" << endl ;
	}

	void epoll::event_del(const int i_socket)
	{
		epoll_event o_event = {0,{0}};
		o_event.data.fd = i_socket;
		o_event.events = EPOLLIN |EPOLLET |EPOLLERR ;
		::epoll_ctl(mi_epoll, EPOLL_CTL_DEL, i_socket, &o_event);

		rlog << "socket(" << i_socket << ") -x-> epoll(" << mi_epoll << ")" << endl ;
	}

	bool epoll::create(const short sh_port)
	{
		if( sh_port == -1 )
		{
			rlog << "port error !" << endl ;
			return false;
		}

		msh_port = sh_port ;

		mi_fd = ::socket(AF_INET, SOCK_STREAM, 0) ;
		if( mi_fd == -1 )
		{
			rlog << "socket() error : " << strerror(errno) << endl ;
			return false;
		}

		socklen_t val = 1 ;
		if( ::setsockopt( mi_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof( val ) ) < 0 )
		{
			rlog << "setsockopt() error : " << strerror(errno) << endl ;
			return false;
		}

		sockaddr_in my_addr ;
		::bzero(&my_addr, sizeof(my_addr));
		my_addr.sin_family = AF_INET ;
		my_addr.sin_port = htons( sh_port ) ;
		my_addr.sin_addr.s_addr = htonl( INADDR_ANY ) ;
		if( ::bind( mi_fd, ( sockaddr* )&my_addr, sizeof( sockaddr ) ) == -1 )
		{
			rlog << "bind() error : " << strerror(errno) << endl ;
			return false;
		}

		if( ::listen( mi_fd, EPOLL_LISTEN_MAX_SIZE ) == -1 )
		{
			rlog << "listen() error : " << strerror(errno) << endl ;
			return false;
		}

		mi_epoll = ::epoll_create(EPOLL_WAIT_MAX_SIZE);
		if(mi_epoll < 0)
		{
			rlog << "epoll_create() error : " << strerror(errno) << endl;
			return false;
		}

		return true ;
	}

	int epoll::accept(void)
	{
		if ( msh_port == -1 )
		{
			rlog << "port error !" << endl ;
			return -1;
		}

		sockaddr_in their_addr = { 0x00 } ;
		::bzero(&their_addr, sizeof(their_addr));
		socklen_t sin_size = sizeof( sockaddr_in );

		int i_socket= ::accept( mi_fd, reinterpret_cast<sockaddr*>( &their_addr ), &sin_size );
		if( i_socket < 0 )
		{
			rlog << "accept() error : " << strerror(errno) << endl ;
			return i_socket ;
		}

		string ip = ::inet_ntoa( their_addr.sin_addr ) ;
		unsigned short port = ::ntohs( their_addr.sin_port ) ;

		cout << endl << endl ;
		rlog << ">>>   socket(" << i_socket << ") from (" << ip << ":" << port << ")   <<<" << endl;

		if ( !rui::net::set_nonblocking(i_socket) )
			rlog << "rui::net::set_nonblocking((" << i_socket << ") error" << endl ;

		return i_socket ;
	}

	int epoll::wait(void)
	{
		::memset( mresult_event, 0x00, sizeof(mresult_event) ) ;

		int nfds = ::epoll_wait(mi_epoll, mresult_event, EPOLL_WAIT_MAX_SIZE, EPOLL_WAIT_TIME);
		if ( nfds < 0 )
			rlog << "epoll_wait(" << mi_epoll << ") error : " << strerror(errno) << endl ;

		return nfds ;
	}

}



#ifndef _MAIN_SERVICE_H_
#define _MAIN_SERVICE_H_

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <map>

#include "epoll.h"
#include "task.h"

extern short gsh_main_service_listen_port;
extern int gi_client_idle ;

class main_service
{		
	private:
		main_service() ;
		~main_service( ) ;
		
		main_service(const main_service&);
		main_service& operator=(const main_service&);

		void add_task(const int i_socket ) ;
		void del_task(const int i_socket);

		static void* epoll_accept_routine(void* arg) ;
		void epoll_accept_entry(void);

		static void* epoll_wait_routine(void* arg) ;
		void epoll_wait_entry(void);
		
		static void* epoll_clear_routine(void* arg) ;
		void epoll_clear_entry(void);

		rui::epoll mservice_epoll ;
		
		std::map<int, std::shared_ptr<rui::task>> mm_task ;

	public:
		static main_service& _instance()
		{
			static main_service instance ;
			return instance ;
		}
		
		bool start(void) ;
		void del_task(std::shared_ptr<rui::task> p_task) ;
} ;

#endif


#include "rui.h"
#include "main_service.h"
#include "threadpool.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/wait.h>

#include <string>
#include <iostream>

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/json.h>


#define config_file "config.json"

int gpid_child;

void sig_handler_parent(int sig)
{
	switch(sig) 
	{
		case SIGTERM:
		case SIGINT:
			if(gpid_child > 0)
				kill(gpid_child, sig);
			rlog << "waiting for pid = " << gpid_child <<std::endl;
			waitpid(gpid_child, 0, 0);
			rlog << "waiting ends" << std::endl << std::endl;
			exit(0);
			break;
	}
}

void sig_handler_child(int sig)
{
	switch(sig) 
	{
		case SIGTERM:
		case SIGINT:
			exit(0);
			break;
	}
}

static int g_single_proc_inst_lock_fd = -1;
static const std::string g_sLockFile = "/var/tmp/" + lexical_cast<std::string>( gsh_main_service_listen_port ) + ".lock"  ;

void single_proc_inst_lockfile_cleanup()
{
	if(g_single_proc_inst_lock_fd != -1)
	{
		close(g_single_proc_inst_lock_fd);
		g_single_proc_inst_lock_fd = -1;
		remove(g_sLockFile.c_str()) ;
	}
}

bool is_single_proc_inst_running()
{
	g_single_proc_inst_lock_fd = open( g_sLockFile.c_str(), O_CREAT | O_RDWR, 0644);
	if(-1 == g_single_proc_inst_lock_fd)
	{
		fprintf(stderr, "Fail to open lock file(%s). Error: %s\n", g_sLockFile.c_str(), strerror(errno));
		return false;
	}

	if(0 == flock(g_single_proc_inst_lock_fd, LOCK_EX | LOCK_NB))
	{
		atexit(single_proc_inst_lockfile_cleanup);
		return true;
	}

	close(g_single_proc_inst_lock_fd);
	g_single_proc_inst_lock_fd = -1;
	return false;
}

void work_process( void )
{
	srand( time( NULL ) );

	signal( SIGPIPE, SIG_IGN ) ;
	signal( SIGTERM, sig_handler_child ) ;
	signal( SIGINT,  sig_handler_child ) ;

	if ( !rui::threadpool::_instance().start() )
	{
		rlog << "rui::threadpool::_instance().start() error" << std::endl ;
		return ;
	}
	rlog << "threadpool start success" << std::endl << std::endl ;

	if ( !main_service::_instance().start() )
	{
		rlog << "main_service::_instance().start() error" << std::endl ;
		return ;
	}
	rlog << "main_service start success" << std::endl << std::endl ;

	while(true)
		sleep(5) ;
}

int main( void )
{
	if(!is_single_proc_inst_running())
	{
		rlog << "system is running already. exit..." << std::endl;
		return -1;
	}

	signal(SIGTERM, sig_handler_parent);
	signal(SIGINT,  sig_handler_parent);

	while(true)
	{
		gpid_child = fork();
		if(gpid_child < 0)
		{
			rlog << "fork error ret = " << gpid_child ;
			return -1 ;
		}
		else if(gpid_child == 0) //child
		{
			work_process();
		}
		else
		{
			rlog << "waiting for pid = " << gpid_child <<std::endl;
			waitpid(gpid_child, 0, 0);
			rlog << "waiting ends" << std::endl << std::endl;
		}

		sleep(1);
	}

	return 0 ;
}

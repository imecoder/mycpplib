
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

#include "net.h"

int main() {
	Json::Value root ;
	root[0] = 0 ;
	root[1] = "fuck" ;
	std::string s_write = root.toStyledString() ;

	int i_socket = -1;
	if( !rui::net::connect( i_socket, "127.0.0.1", 5000 ) )
	{
		rlog << "rui::net::connect server(127.0.0.1:5000) error" << std::endl;
		if ( i_socket != -1 )
			rui::net::close(i_socket);
		return false ;
	}

	if( rui::json::write( i_socket, s_write ) < 0 )
	{
		rlog << "rui::json::write server(" << i_socket << ") error" << std::endl ;
		rui::net::close(i_socket);
		return false ;
	}
	sleep(100000);

	std::vector<char> v_read ;
	int ret = rui::json::read( i_socket, v_read, 0 ) ;
	if ( ret != rui::net::SMOOTH )
	{
		rlog << "rui::json::read() failure" << std::endl ;
		rui::net::close(i_socket);
		return false ;
	}

	std::string s_read(v_read.begin(), v_read.end());
	std::cout << "s_read = " << std::endl << s_read << std::endl;

	Json::Value parse_root ;
	Json::Reader reader ;

	if ( !reader.parse( s_read, parse_root ) )
	{
		rlog << "rjson::parse json error" << std::endl ;
		rui::net::close(i_socket);
		return false ;
	}

	if( parse_root[0].asInt() != 1 )
	{
		rlog << "close(" << i_socket << ") now" << std::endl ;
		rui::net::close(i_socket);
		return false ;
	}

	rui::net::close(i_socket);

	return 0;
}

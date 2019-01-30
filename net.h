#ifndef _NET_H_
#define _NET_H_

// linux headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>


#include <string>
#include <iostream>
#include <vector>

#include "rui.h"

#define rui_close(i_fd)	\
{	::close(i_fd); 	rlog << "close(" << i_fd << ")" << std::endl  << std::endl ; }


namespace rui {
	namespace net
	{

#ifndef SOCKET_BUFFER_SIZE
#define SOCKET_BUFFER_SIZE 87380
#endif

		enum STATUS
		{
			PROTOCOL = -4,
			TIMEOUT = -3,
			CLOSE = -2,
			ERROR = -1,
			SMOOTH = 0
		} ;

		//select最大只支持到1024个文件描述符
		int select_rdset( const int i_socket, const int i_second = 20 ) ;
		int select_wrset( const int i_socket, const int i_second = 20 ) ;

		bool set_nonblocking( const int i_socket );

		// 此函数只支持linux系统
		bool connect( int& i_socket, const std::string& ip, const short& port, const int i_second = 20 ) ;
		void close(const int i_socket );

		int read( const int i_socket, std::vector<char>& v_data, const int i_second = 20 ) ;

		int write( const int i_socket, const std::vector<char>& v_data ) ;
		int write( const int i_socket, const std::string& s_data ) ;
		int write( const int i_socket, const char* p_data, const size_t length ) ;

		std::string domain2ip( const std::string& s_domain );
		std::string get_host_ip(void) ;
		std::string get_peer_ip(const int i_socket) ;
		bool is_ip( const char *str );
	}

	namespace http
	{
		const std::string VPATH( "VPath" );

		enum HTTP_TYPE
		{
			GET,
			POST
		};

		int read( const int i_socket, std::vector<char>& v_head, std::vector<char>& v_body, const int i_second = 20 ) ;
		int read_head( const int i_socket, std::vector<char>& v_head, const int i_second = 20 ) ;
		int read_in_length_mode( const int i_socket, const std::vector<char>& v_head, std::vector<char>& v_body, const int i_second = 20 ) ;
		int read_in_chunk_mode( const int i_socket, std::vector<char>& v_body, const int i_second = 20 ) ;

		int write( const int i_socket, const std::string& s_body );
		int write( const int i_socket, const std::vector<char>& v_body ) ;
		bool write_gzip( const int i_socket, const std::vector<char>& v_body ) ;

		int write( const int i_socket, const std::string& s_head, const std::string& s_body );
		int write( const int i_socket, const std::vector<char>& v_head, const std::vector<char>& v_body ) ;

		bool write_failure(const int i_socket, const std::string& s_data) ;
		bool write_success(const int i_socket) ;

		std::pair<std::string, std::string> divide_argument( const std::string& url, const std::string& sDelem );
		void parse_url( URLContainer &urlArgMap, const std::string& s_vpath );
		void get_vpath( std::string& s_vpath, const std::string& s_content, const HTTP_TYPE type );
	}

	namespace json
	{
		enum STATUS
		{
			FAILURE = 0,
			SUCCESS = 1
		};

		int read( const int i_socket, std::vector<char>& v_data, const int i_second = 20 ) ;
		int write( const int i_socket, const std::string& s_data );

		bool write_failure(const int i_socket, const std::string& s_data );
		bool write_success(const int i_socket) ;

		int read_decoded( const int i_socket, std::vector<char>& v_data, const int i_xor, const int encode_key, const int i_second = 20);
		int write_encoded( const int i_socket, const std::string& s_data, const int i_xor, const int encode_key );

		bool write_failure_encoded( const int i_socket, const std::string& s_data, const int i_xor, const int encode_key );
		bool write_success_encoded( const int i_socket, const int i_xor, const int encode_key );
	}
}

#endif

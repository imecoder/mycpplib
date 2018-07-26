#include <string.h>
#include <errno.h>
#include "net.h"
#include "crypto.h"
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/json.h>
using namespace	Json ;

namespace rui {
	namespace net {
		int select_rdset( const int i_socket, const int i_second )
		{
			timeval tv ;
			tv.tv_sec = i_second ;
			tv.tv_usec = 0L ;

			fd_set rdset;
			FD_ZERO( &rdset );
			FD_SET( i_socket, &rdset );

			int ret = ::select( i_socket + 1, &rdset, NULL, NULL, &tv ) ;

			if( ret < 0 )
			{
				rlog << "select(" << i_socket << ") error : " << strerror( errno )  << endl ;
				return	ERROR ;
			}

			if( ret == 0 )
			{
				rlog << "select(" << i_socket << ") timeout" << endl ;
				return	TIMEOUT ;
			}

			return SMOOTH ;
		}

		int select_wrset( const int i_socket, const int i_second )
		{
			timeval tv ;
			tv.tv_sec = i_second ;
			tv.tv_usec = 0L ;

			fd_set wrset;
			FD_ZERO( &wrset );
			FD_SET( i_socket, &wrset );

			int ret = ::select( i_socket + 1, NULL, &wrset, NULL, &tv ) ;

			if( ret < 0 )
			{
				rlog << "select(" << i_socket << ") error : " << strerror(errno)  << endl ;
				return	ERROR ;
			}

			if( ret == 0 )
			{
				rlog << "select(" << i_socket << ") timeout" << endl ;
				return	TIMEOUT ;
			}

			return SMOOTH ;
		}

		bool connect( int& i_socket, const string& ip, const short& port, const int i_second )
		{
			i_socket = ::socket( AF_INET, SOCK_STREAM, 0 ) ;
			if( i_socket < 0 )
			{
				rlog << "socket() error : " << strerror(errno)  << endl ;
				return	false ;
			}

			sockaddr_in serv_addr = { 0x00 } ;
			::bzero(&serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET ;
			serv_addr.sin_port = ::htons( port ) ;
			serv_addr.sin_addr.s_addr = ::inet_addr( ip.c_str( ) ) ;

			timeval tv ;
			tv.tv_sec = i_second ;
			tv.tv_usec = 0L ;
			// 由于非阻塞模式，select最大支持到1024个文件描述符，因此，此处改改用setsockopt来代替
			// 此种方式，只支持linux，其他系统不可
			// 同样可以设置连接(connect)超时：即通过SO_SNDTIMO套节字参数让超时操作跳过select。
			// 原因是：Linux内核源码中connect的超时参数和SO_SNDTIMO操作的参数一致。
			// 因此，在linux平台下，可以通过connect之前设置SO_SNDTIMO来达到控制连接超时的目的。
			::setsockopt( i_socket, SOL_SOCKET, SO_SNDTIMEO, &tv,sizeof(tv) );

			if ( ::connect( i_socket, reinterpret_cast<sockaddr*>( &serv_addr ), sizeof( sockaddr ) ) < 0 )
			{
				rlog << "connect(" << i_socket << ") error : " << strerror(errno)  << endl ;
				return	false ;
			}

			return	true ;
		}

		bool set_nonblocking( const int i_socket )
		{
			int opts;
			opts = ::fcntl( i_socket, F_GETFL );
			if( opts < 0 )
			{
				rlog << "fcntl(" << i_socket << ") error : " << strerror(errno) << endl;
				return	false;
			}
			opts = opts | O_NONBLOCK;
			if( ::fcntl( i_socket, F_SETFL, opts ) < 0 )
			{
				rlog << "fcntl(" << i_socket << ") non-block error : " << strerror(errno) << endl;
				return false ;
			}

			return	true ;
		}

		int read( const int i_socket, vector<char>& v_data, const int i_second )
		{
			while ( true )
			{
				if ( i_second != 0 )
				{
					int ret = select_rdset( i_socket, i_second ) ;
					if ( ret < SMOOTH )
					{
						rlog << "select_rdset(" << i_socket << ") error" << endl ;
						return ret ;
					}
				}

				char buffer[SOCKET_BUFFER_SIZE] = { 0x00 } ;
				int ret = ::read( i_socket, buffer, SOCKET_BUFFER_SIZE ) ;
				if( ret < 0 )
				{
					rlog << "read(" << i_socket << ") error : " << strerror(errno) << endl;

					if( errno == EINTR )
						continue ;

					if( errno == EAGAIN )
						continue;

					return	ERROR ;
				}

				if ( ret == 0 )
				{
					rlog << "read(" << i_socket << ") peer closed" << endl ;
					return	CLOSE ;
				}

				copy( buffer, buffer+ret, back_inserter( v_data ) ) ;
				break ;
			}

			return	SMOOTH ;
		}

		int write( const int i_socket, const vector<char>& v_data )
		{
			if (v_data.empty())
			{
				rlog << "data empty" << endl ;
				return  ERROR;
			}

			return write( i_socket, &v_data[0], v_data.size()) ;
		}

		int write( const int i_socket, const string& s_data )
		{
			if (s_data.empty())
			{
				rlog << "data empty" << endl ;
				return  ERROR;
			}

			return write( i_socket, s_data.c_str(), s_data.size()) ;
		}

		int write( const int i_socket, const char* p_data, const size_t length )
		{
			if (p_data == NULL)
			{
				rlog << "data empty" << endl ;
				return  ERROR;
			}

			std::size_t numbytes = 0 ;
			for( std::size_t i = 0 ; numbytes < length; ++i )
			{
				int ret = ::write( i_socket, p_data+numbytes, length-numbytes ) ;
				if( ret == -1 )
				{
					if( errno == EINTR )
						continue ;

					if( errno == EAGAIN )
						continue ;

					rlog << "write(" << i_socket << ") error : " << strerror( errno )  << endl ;
					return	ERROR ;
				}
				numbytes += ret ;
			}

			return	numbytes ;
		}

		string domain2ip( const string& s_domain )
		{
			hostent *site = ::gethostbyname( s_domain.c_str( ) ) ;
			if( site == NULL )
			{
				rlog << "gethostbyname(" << s_domain << ") error : " << strerror(errno)  << endl ;
				return	"" ;
			}

			for( char **p = site->h_addr_list ; *p != NULL ; ++p )
				return	::inet_ntoa( *( in_addr * )( *p ) ) ;

			return	"" ;
		}

		string get_host_ip(void)
		{
			ifaddrs *ifAddrStruct = NULL ;
			getifaddrs( &ifAddrStruct ) ;

			while( ifAddrStruct != NULL )
			{
				if( ifAddrStruct->ifa_addr->sa_family != AF_INET )
				{
					ifAddrStruct = ifAddrStruct->ifa_next;
					continue ;
				}

				void *tmpAddrPtr = &( ( struct sockaddr_in * )ifAddrStruct->ifa_addr )->sin_addr;
				char addressBuffer[INET_ADDRSTRLEN] = { 0x00 } ;
				::inet_ntop( AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN );
				if( strcmp( addressBuffer, "127.0.0.1" ) != 0 )
					return addressBuffer;

				ifAddrStruct = ifAddrStruct->ifa_next;
			}

			return	"" ;
		}

		string get_peer_ip(const int i_socket)
		{
			sockaddr_in  peer_addr = { 0x00 } ;
			::bzero(&peer_addr, sizeof(peer_addr));
			socklen_t peerlen = sizeof( peer_addr );

			if( ::getpeername( i_socket, ( sockaddr * )&peer_addr, &peerlen ) < 0 )
			{
				rlog << "getpeername(" << i_socket << ") error : " << strerror(errno) << endl ;
				return "";
			}
			return ::inet_ntoa( peer_addr.sin_addr );
		}

		bool is_ip( const char *str )
		{
			const int size = strlen( str ) ;
			for( int i = 0 ; i < size ; ++i )
				// not digit and not .
				if( ::isdigit( str[i] ) == 0 && str[i] != '.' )
					return	false ;

			return	true ;
		}


	}

	namespace http {
		int read( const int i_socket, vector<char>& v_head, vector<char>& v_body, const int i_second )
		{
			int ret = read_head( i_socket, v_head, i_second ) ;
			if ( ret != net::SMOOTH )
				return	ret;

			string s_head( v_head.begin( ), v_head.end( ) ) ;
			transform(s_head.begin(), s_head.end(), s_head.begin(), ::tolower);

			if( s_head.find( "content-length:" ) != string::npos )
				return	read_in_length_mode( i_socket, v_head, v_body, i_second ) ;

			if( s_head.find( "transfer-encoding: chunked" ) != string::npos )
				return	read_in_chunk_mode( i_socket, v_body, i_second ) ;

			rlog << "read(" << i_socket << ") http data mode error" << endl ;

			return	net::PROTOCOL ;
		}

		int	read_head( const int i_socket, vector<char>& v_head, const int i_second )
		{
			v_head.clear() ;
			// 单字节接受，直到接收完http头
			int i = 0 ;
			for( ; i < 10000 ; ++i )
			{
				if ( i_second != 0 )
				{
					int ret = net::select_rdset( i_socket, i_second ) ;
					if ( ret < net::SMOOTH )
					{
						rlog << "net::select_rdset(" << i_socket << ") error" << endl ;
						return ret ;
					}
				}

				char szAchar = 0 ;
				int ret = ::read( i_socket, &szAchar, 1 ) ;
				if( ret < 0 )
				{
					rlog << "read(" << i_socket << ") error : " << strerror( errno ) << endl;

					if( errno == EINTR )
						continue ;

					if( errno == EAGAIN )
						continue ;

					return	net::ERROR ;
				}

				if( ret == 0 )
				{
					rlog << "read(" << i_socket << ") peer closed" << endl ;
					return	net::CLOSE ;
				}

				v_head.push_back( szAchar ) ;

				if( v_head.size( ) < 4 )
					continue ;

				if( ::strncmp( &( *( v_head.end( ) - 4 ) ), "\r\n\r\n", 4 ) == 0 )
					break ;
			}

			if( i >= 9999 )
			{
				rlog << "read(" << i_socket << ") protocol error" << endl ;
				return	net::PROTOCOL ;
			}

			return	net::SMOOTH ;
		}


		int	read_in_length_mode(const int i_socket, const vector<char>& v_head, vector<char>& v_body, const int i_second)
		{
			v_body.clear() ;
			string s_length ;
			string s_head( v_head.begin(), v_head.end() ) ;
			transform(s_head.begin(), s_head.end(), s_head.begin(), ::tolower);

			if( Substr( s_length, s_head, "content-length:", "\r\n" ) == string::npos )
			{
				rlog << "Substr() error" << endl ;
				return	net::ERROR ;
			}

			string::size_type pos = 0U ;
			while( true )
			{
				pos = s_length.find( " " ) ;
				if( pos == string::npos )
					break ;

				s_length.erase( pos, 1 ) ;
			}

			const std::size_t i_content_length = atoi( s_length.c_str( ) ) ;

			if( i_content_length <= 0 )
				return	net::SMOOTH ;

			for( int i = 0 ; true ; ++i )
			{
				if ( i_second != 0 )
				{
					int ret = net::select_rdset( i_socket, i_second ) ;
					if ( ret < net::SMOOTH )
					{
						rlog << "net::select_rdset(" << i_socket << ") error" << endl ;
						return ret ;
					}
				}

				char buffer[SOCKET_BUFFER_SIZE] = { 0x00 } ;
				int ret = ::read( i_socket, buffer, SOCKET_BUFFER_SIZE ) ;
				if( ret < 0 )
				{
					rlog << "read(" << i_socket << ") error : " << strerror( errno ) << endl;

					if( errno == EINTR )
						continue ;

					if( errno == EAGAIN )
						continue ;

					return	net::ERROR ;
				}

				if( ret == 0 )
				{
					rlog << "read(" << i_socket << ") peer closed" << endl ;
					return	net::CLOSE ;
				}

				for( int i = 0 ; i < ret ; ++i )
					v_body.push_back( buffer[i] ) ;

				if( v_body.size( ) >= i_content_length )
					break ;
			}

			return	net::SMOOTH ;
		}

		int read_in_chunk_mode( const int i_socket, vector<char>& v_body, const int i_second )
		{
			v_body.clear() ;
			for( int cnt = 0 ; cnt < 10000000 ; ++cnt )
			{
				vector<char> chunckData ;
				string sChunckLengthWithEnd ;
				unsigned int iChunckLength = 0 ;
				for( int i = 0 ; i < 1000 ; ++i )
				{
					if ( i_second != 0 )
					{
						int ret = net::select_rdset( i_socket, i_second ) ;
						if ( ret < net::SMOOTH )
						{
							rlog << "net::select_rdset(" << i_socket << ") error" << endl ;
							return ret ;
						}
					}

					char szAchar[1] = "" ;
					int ret = ::read( i_socket, szAchar, 1 ) ;
					if( ret < 0 )
					{
						rlog << "read(" << i_socket << ") error : " << strerror( errno ) << endl;

						if( errno == EINTR )
							continue ;

						if( errno == EAGAIN )
							continue ;

						return	net::ERROR ;
					}

					if( ret == 0 )
					{
						rlog << "read(" << i_socket << ") peer closed" << endl ;
						return	net::CLOSE;
					}

					sChunckLengthWithEnd += szAchar[0] ;

					if( sChunckLengthWithEnd.rfind( "\r\n\r\n" ) != string::npos )
					{
						rlog << "get head success!" << endl ;
						break ;
					}

					// 去掉开始的\r\n
					if( sChunckLengthWithEnd.size( ) >= 2 )
					{
						string headBreak( sChunckLengthWithEnd.substr( 0, 2 ) ) ;
						if( headBreak == "\r\n" )
							sChunckLengthWithEnd.erase( 0, 2 ) ;
					}

					string::size_type pos = sChunckLengthWithEnd.find( "\r\n" ) ;
					if( pos != string::npos )
					{
						iChunckLength = strtol( sChunckLengthWithEnd.c_str( ), NULL, 16 ) ;
						rlog << "iChunckLength = " << iChunckLength << endl ;
						break ;
					} // end if
				} // end for

				if( iChunckLength <= 0 )
					break ;

				for( int i = 0 ; true ; ++i )
				{
					if ( i_second != 0 )
					{
						int ret = net::select_rdset( i_socket, i_second ) ;
						if ( ret < net::SMOOTH )
						{
							rlog << "net::select_rdset(" << i_socket << ") error" << endl ;
							return ret ;
						}
					}

					const int ilast = iChunckLength - chunckData.size( ) ;

					int currecv = ilast ;
					if( currecv > SOCKET_BUFFER_SIZE )
						currecv = SOCKET_BUFFER_SIZE ;

					char buffer[SOCKET_BUFFER_SIZE] = { 0x00 } ;
					int ret = ::read( i_socket, buffer, currecv ) ;
					if( ret < 0 )
					{
						rlog << "read(" << i_socket << ") error : " << strerror( errno ) << endl;

						if( errno == EINTR )
							continue ;

						if( errno == EAGAIN )
							continue ;

						return	net::ERROR ;
					}

					if( ret == 0 )
					{
						rlog << "read(" << i_socket << ") peer closed" << endl ;
						return	net::CLOSE ;
					}

					for( int i = 0 ; i < ret ; ++i )
						chunckData.push_back( buffer[i] ) ;

					if( chunckData.size( ) >= iChunckLength )
						break ;
				}

				for( unsigned int i = 0 ; i < chunckData.size( ) ; ++i )
					v_body.push_back( chunckData[i] ) ;
			}

			return	net::SMOOTH ;
		}

		int write( const int i_socket, const string& s_body )
		{
			if (s_body.empty())
			{
				rlog << "body empty" << endl ;
				return  net::ERROR;
			}

			char length[10] = { 0x00 } ;
			sprintf( length, "%d", (int)s_body.size() );

			string s_head("HTTP/1.1 200 OK\r\n");
			s_head += "Content-Length: " + string(length) + "\r\n";
			s_head += "Content-Type: text/html\r\n\r\n" ;

			return net::write(i_socket, s_head+s_body) ;
		}

		int write( const int i_socket, const vector<char>& v_body )
		{
			if (v_body.empty())
			{
				rlog << "boday empty" << endl ;
				return  net::ERROR;
			}

			char length[10] = { 0x00 } ;
			sprintf( length, "%d", (int)v_body.size() );

			string s_head("HTTP/1.1 200 OK\r\n");
			s_head += "Content-Length: " + string(length) + "\r\n";
			s_head += "Content-Type: text/html\r\n\r\n" ;

			if ( net::write( i_socket, s_head) == net::ERROR )
			{
				rlog << "net::write(" << i_socket << ") error" << endl ;
				return	net::ERROR ;
			}

			return net::write(i_socket, v_body) ;
		}

		bool write_gzip( const int i_socket, const vector<char>& v_body )
		{
			char length[10] = { 0x00 } ;
			sprintf( length, "%d", (int)v_body.size() );

			string s_head("HTTP/1.1 200 OK\r\n") ;
			s_head += "Content-Length: " + string(length) +"\r\n" ;
			s_head += "Content-Type: text/html\r\n" ;
			s_head += "Content-Encoding: gzip\r\n\r\n" ;

			if ( net::write( i_socket, s_head) == net::ERROR )
			{
				rlog << "net::write(" << i_socket << ") error" << endl ;
				return	false ;
			}

			return  net::write( i_socket, v_body) ;
		}

		int write( const int i_socket, const string& s_head, const string& s_body )
		{
			if (s_head.empty())
			{
				rlog << "head empty" << endl ;
				return  net::ERROR;
			}

			if (s_body.empty())
			{
				rlog << "body empty" << endl ;
				return  net::ERROR;
			}

			if ( net::write( i_socket, s_head) == net::ERROR )
			{
				rlog << "net::write(" << i_socket << ") error" << endl ;
				return	net::ERROR ;
			}

			return net::write(i_socket, s_body) ;
		}

		int write( const int i_socket, const vector<char>& v_head, const vector<char>& v_body )
		{
			if (v_head.empty())
			{
				rlog << "head empty" << endl ;
				return  net::ERROR;
			}

			if (v_body.empty())
			{
				rlog << "body empty" << endl ;
				return  net::ERROR;
			}

			if ( net::write( i_socket, v_head) == net::ERROR )
			{
				rlog << "net::write(" << i_socket << ") error" << endl ;
				return	net::ERROR ;
			}

			return net::write(i_socket, v_body) ;
		}

		bool write_failure( const int i_socket, const string& s_data )
		{
			if (s_data.empty())
			{
				rlog << "data empty" << endl ;
				return false;
			}

			Value root;
			root[0] = json::FAILURE;
			root[1] = s_data ;
			string json = root.toStyledString();
			if ( write( i_socket, json ) < 0  )
			{
				rlog << "write(" << i_socket << ") error" << endl ;
				return false ;
			}

			rlog << "write(" << i_socket << ") failure data" << endl << json << endl;

			return true ;
		}

		bool write_success( const int i_socket )
		{
			Value root;
			root[0] = json::SUCCESS;
			string json = root.toStyledString();
			if ( write( i_socket, json ) < 0  )
			{
				rlog << "write(" << i_socket << ") error" << endl ;
				return false ;
			}

			rlog << "write(" << i_socket << ") success data" << endl << json << endl;

			return true ;
		}

		pair<string, string> divide_argument( const string& url, const string& sDelem )
		{
			string::size_type pos = url.find( sDelem );
			if( pos != string::npos )
				return make_pair( url.substr( 0, pos ), url.substr( pos + sDelem.size( ) ) );

			return make_pair( "", "" );
		}

		void parse_url( URLContainer &urlArgMap, const string& s_vpath )
		{
			urlArgMap.clear( );

			size_t pos1 = s_vpath.find( "?" );
			if( pos1 == string::npos )
			{
				urlArgMap.insert( URLContainer::value_type( VPATH, s_vpath ) );
			}
			else
			{
				urlArgMap.insert( URLContainer::value_type( VPATH, s_vpath.substr( 0, pos1 ) ) );

				string::size_type pos2 = pos1 + 1;
				string::size_type pos3 = pos1 + 1;
				while( true )
				{
					pos3 = s_vpath.find( "&", pos2 );
					if( pos3 != string::npos )
					{
						string sArgSec( s_vpath, pos2, pos3 - pos2 );

						urlArgMap.insert( divide_argument( sArgSec, "=" ) );
						pos2 = pos3 + 1;
					}
					else
					{
						string sArgSec( s_vpath, pos2 );
						urlArgMap.insert( divide_argument( sArgSec, "=" ) );
						break;
					}
				}
			}
		}

		void get_vpath( string& s_vpath, const string& s_content, const HTTP_TYPE type )
		{
			s_vpath.clear( );

			if( type == GET )
				Substr( s_vpath, s_content, "GET ", " HTTP" );
			else if( type == POST )
				Substr( s_vpath, s_content, "POST ", " HTTP" );
			else	return;

			if( s_vpath.size( ) < 7 )
				return;

			// 虚拟路径是完整url的情况, 去http头
			if( s_vpath.substr( 0, 7 ) == "http://" )
			{
				string::size_type pos = s_vpath.find( '/', 7 );
				if( pos == string::npos )
				{
					// 访问主页的情况
					s_vpath.clear( );
					return;
				}

				s_vpath.erase( 0, pos );
			}

			// 去掉多'/'的情况
			while( s_vpath[0] == '/' || s_vpath[0] == ' ' )
				s_vpath.erase( 0, 1 );

			// 过滤&amp;的情况
			const string sKeyWord( "&amp;" );
			while( true )
			{
				string::size_type pos = s_vpath.find( sKeyWord );
				if( pos == string::npos )
					break ;

				s_vpath.replace( pos, sKeyWord.size( ), "&" );
			}
		}
	}

	namespace json {
		int read( const int i_socket, vector<char>& v_data, const int i_second )
		{
			v_data.clear() ;

			int length = 0 ;

			while ( true )
			{
				if ( i_second != 0 )
				{
					int ret = net::select_rdset( i_socket, i_second ) ;
					if ( ret < net::SMOOTH )
					{
						rlog << "net::select_rdset(" << i_socket << ") error" << endl ;
						return ret ;
					}
				}

				char buffer[5] = { 0x00 } ;
				int ret = ::read( i_socket, buffer, 4 ) ;
				if( ret < 0 )
				{
					rlog << "read(" << i_socket << ") error : " << strerror( errno ) << endl;

					if( errno == EINTR )
						continue ;

					if( errno == EAGAIN )
						continue ;

					return	net::ERROR ;
				}

				if ( ret == 0 )
				{
					rlog << "read(" << i_socket << ") peer closed" << endl ;
					return	net::CLOSE ;
				}

				memcpy( (char*)&length, buffer, 4 ) ;
				break ;
			}


			while ( true )
			{
				if ( i_second != 0 )
				{
					int ret = net::select_rdset( i_socket, i_second ) ;
					if ( ret < net::SMOOTH )
					{
						rlog << "net::select_rdset(" << i_socket << ") error" << endl ;
						return ret ;
					}
				}

				char buffer[SOCKET_BUFFER_SIZE] = { 0x00 } ;
				int ret = ::read( i_socket, buffer, length ) ;
				if( ret < 0 )
				{
					rlog << "read(" << i_socket << ") error : " << strerror( errno ) << endl;

					if( errno == EINTR )
						continue ;

					if( errno == EAGAIN )
						continue ;

					return	net::ERROR ;
				}

				if ( ret == 0 )
				{
					rlog << "read(" << i_socket << ") peer closed" << endl ;
					return	net::CLOSE ;
				}

				copy( buffer, buffer+ret, back_inserter( v_data ) ) ;

				length -= ret ;
				if ( length > 0 )
					continue ;

				break ;
			}

			return	net::SMOOTH ;
		}

		int write( const int i_socket, const string& s_data )
		{
			if (s_data.empty())
			{
				rlog << "data empty" << endl ;
				return  net::ERROR;
			}

			char buffer[SOCKET_BUFFER_SIZE] = { 0x00 } ;
			unsigned int length = s_data.size() ;
			memcpy( buffer, (char*)&length, 4 ) ;
			memcpy( buffer+4, s_data.c_str(), length ) ;

			return net::write( i_socket, buffer, length+4 ) ;
		}

		int read_decoded( const int i_socket, vector<char>& v_data, const int i_xor, const int decode_key, const int i_second )
		{
			v_data.clear() ;

			vector<char> v_in;

			int length = 0 ;

			while ( true )
			{
				if ( i_second != 0 )
				{
					int ret = net::select_rdset( i_socket, i_second ) ;
					if ( ret < net::SMOOTH )
					{
						rlog << "net::select_rdset(" << i_socket << ") error" << endl ;
						return ret ;
					}
				}

				char buffer[5] = { 0x00 } ;
				int ret = ::read( i_socket, buffer, 4 ) ;
				if( ret < 0 )
				{
					rlog << "read(" << i_socket << ") error : " << strerror( errno ) << endl;

					if( errno == EINTR )
						continue ;

					if( errno == EAGAIN )
						continue ;

					return	net::ERROR ;
				}

				if ( ret == 0 )
				{
					rlog << "read(" << i_socket << ") peer closed" << endl ;
					return	net::CLOSE ;
				}

				memcpy( (char*)&length, buffer, 4 ) ;
				break ;
			}


			while ( true )
			{
				if ( i_second != 0 )
				{
					int ret = net::select_rdset( i_socket, i_second ) ;
					if ( ret < net::SMOOTH )
					{
						rlog << "net::select_rdset(" << i_socket << ") error" << endl ;
						return ret ;
					}
				}

				char buffer[SOCKET_BUFFER_SIZE] = { 0x00 } ;
				int ret = ::read( i_socket, buffer, length ) ;
				if( ret < 0 )
				{
					rlog << "read(" << i_socket << ") error : " << strerror( errno ) << endl;

					if( errno == EINTR )
						continue ;

					if( errno == EAGAIN )
						continue ;

					return	net::ERROR ;
				}

				if ( ret == 0 )
				{
					rlog << "read(" << i_socket << ") peer closed" << endl ;
					return	net::CLOSE ;
				}

				copy( buffer, buffer+ret, back_inserter( v_in ) ) ;
				break ;
			}

			crypto o_crypto(i_xor) ;
			if ( !o_crypto.decode(v_in, v_data, decode_key) )
			{
				rlog << "o_crypto.decode() error" << endl ;
				return net::PROTOCOL;
			}

			return	net::SMOOTH ;
		}

		int write_encoded( const int i_socket, const string& s_data, const int i_xor, const int encode_key )
		{
			if (s_data.empty())
			{
				rlog << "data empty" << endl ;
				return  net::ERROR;
			}

			vector<char> v_in(s_data.begin(), s_data.end()) ;
			vector<char> v_out;
			crypto o_crypto(i_xor) ;
			if ( !o_crypto.encode(v_in, v_out, encode_key) )
			{
				rlog << "o_crypto.encode() error" << endl ;
				return -1 ;
			}

			char buffer[SOCKET_BUFFER_SIZE] = { 0x00 } ;
			unsigned int length = s_data.size() ;
			::memcpy( buffer, (char*)&length, 4 ) ;
			::memcpy( buffer+4, &v_out[0], length ) ;

			return net::write( i_socket, buffer, length+4 ) ;
		}

		bool write_failure( const int i_socket, const string& s_data )
		{
			if (s_data.empty())
			{
				rlog << "data empty" << endl ;
				return false;
			}

			Value root;
			root[0] = FAILURE;
			root[1] = s_data ;
			string json = root.toStyledString();
			if ( write( i_socket, json ) < 0  )
			{
				rlog << "write(" << i_socket << ") error" << endl ;
				return false ;
			}

			rlog << "write(" << i_socket << ") failure data" << endl << json << endl;

			return true ;
		}


		bool write_success( const int i_socket )
		{
			Value root;
			root[0] = SUCCESS;
			string json = root.toStyledString();
			if ( write( i_socket, json ) < 0  )
			{
				rlog << "write(" << i_socket << ")error" << endl ;
				return false ;
			}

			rlog << "write(" << i_socket << ") success data" << endl << json << endl;

			return true ;
		}

		bool write_failure_encoded( const int i_socket, const string& s_data, const int i_xor, const int encode_key )
		{
			if (s_data.empty())
			{
				rlog << "data empty" << endl ;
				return false;
			}

			Value root;
			root[0] = json::FAILURE;
			root[1] = s_data ;
			string json = root.toStyledString();
			if ( write_encoded( i_socket, json, i_xor, encode_key ) < 0  )
			{
				rlog << "net::write_encoded(" << i_socket << ") error" << endl ;
				return false ;
			}

			rlog << "write_encoded(" << i_socket << ") failure data" << endl << json << endl;

			return true ;
		}

		bool write_success_encoded( const int i_socket, const int i_xor, const int encode_key )
		{
			Value root;
			root[0] = SUCCESS;
			string json = root.toStyledString();
			if ( write_encoded( i_socket, json, i_xor, encode_key ) < 0  )
			{
				rlog << "write_encoded(" << i_socket << ")error" << endl ;
				return false ;
			}

			rlog << "write_encoded(" << i_socket << ") success data" << endl << json << endl;

			return true ;
		}

	}
}






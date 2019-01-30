#ifndef _RUI_H_
#define _RUI_H_

#include <sys/syscall.h>
#include <zlib.h>
#include <uuid/uuid.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <openssl/md5.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#define MAX_SOCKET_SIZE	65536

typedef std::map<std::string, std::string> URLContainer;

inline std::string getTime(const char *format)
{
	time_t second = time( NULL ) ;
	char szTime[50] = "";
	strftime( szTime, 50, format, localtime( &second ) );

	return szTime;
}

#define	rlog	std::cout << "[" << syscall( SYS_gettid ) << " " << getTime( "%Y-%m-%d %H:%M:%S") << " " << __FILE__ << "," << __LINE__ << "] "


/**
 *    不同类型的数据转换成字符串的函数
 */
	template<typename T1, typename T2>
inline T1 lexical_cast( const T2 &t )
{
	std::stringstream ss ;

	ss << t ;
	T1 tReturn ;

	ss >> tReturn ;

	return	tReturn ;
}

inline bool is_digital( const std::string& s_data )
{
	for(size_t i=0;i<s_data.size();i++)
		if (!isdigit( s_data.at(i)))
			return false ;

	return true;
}


// 字符串截取
// 输出参数，输入参数，头，尾
inline std::string::size_type Substr( std::string& sSub, const std::string& s, const std::string& s_head, const std::string& sTail, const std::string::size_type pos = 0 )
{
	if( pos >= s.size( ) )
	{
		return	std::string::npos ;
	}

	std::string::size_type pos1 = 0 ;
	std::string::size_type pos2 = 0 ;

	if( ( pos1 = s.find( s_head, pos ) ) == std::string::npos )
	{
		return	std::string::npos ;
	}

	if( ( pos2 = s.find( sTail, pos1 + s_head.size( ) ) ) == std::string::npos )
	{
		std::string sErr = "cant find word: " + sTail ;
		return	std::string::npos ;
	}

	sSub = s.substr( pos1 + s_head.size( ), pos2 - pos1 - s_head.size( ) ) ;

	return	pos2 + sTail.size( ) ;
}

inline std::string::size_type SubstrF( std::string& sSub, const std::string& s, const std::string& s_head, const std::string& sTail, const std::string::size_type pos = 0 )
{
	if( pos >= s.size( ) )
	{
		return std::string::npos;
	}

	std::string s1( s );
	transform(s1.begin(), s1.end(), s1.begin(), ::tolower);

	std::string sHead1( s_head );
	transform(sHead1.begin(), sHead1.end(), sHead1.begin(), ::tolower);

	std::string sTail1( sTail );
	transform(sTail1.begin(), sTail1.end(), sTail1.begin(), ::tolower);

	std::string::size_type pos1 = 0;
	std::string::size_type pos2 = 0;

	if( ( pos1 = s1.find( sHead1, pos ) ) == std::string::npos )
		return std::string::npos;

	if( ( pos2 = s1.find( sTail1, pos1 + sHead1.size( ) ) ) == std::string::npos )
	{
		std::string sErr = "cant find word: " + sTail;
		return std::string::npos;
	}

	sSub = s.substr( pos1 + s_head.size( ), pos2 - pos1 - s_head.size( ) );
	return pos2 + sTail.size( );
}

inline int findSz( const char *sz1, const char *sz2, int size1, int size2 )
{
	if( sz1 == NULL || sz2 == NULL )
		return	-1;

	if( size1 < size2 )
		return	-1;

	for( int i = 0; i < size1 - size2 + 1; ++i )
	{
		for( int j = 0; j < size2; ++j )
		{
			if( sz1[i + j] != sz2[j] )
				break;

			if( j == size2 - 1 )
				return i;
		}
	}

	return	-1;
}


	template <typename T1, typename T2>
inline T2 get_map_value( const std::map<T1, T2> &mp, const T1 &Key )
{
	URLContainer::const_iterator citer = mp.find( Key );
	if( citer != mp.end( ) )
		return citer->second;

	return T2( );
}


	template <typename T1, typename T2>
inline std::ostream &operator <<( std::ostream &os, const std::map<T1, T2> &mp )
{
	for( typename std::map<T1, T2>::const_iterator iter = mp.begin( ); iter != mp.end( ); ++iter )
		os << iter->first << " " << iter->second << std::endl;

	return os;
}

inline unsigned long long string_to_ullong(const std::string& str )
{
	std::stringstream strIn;
	strIn<<str;
	unsigned long long ullNum;
	strIn>>ullNum;
	return ullNum;
}

inline std::ostream &operator <<( std::ostream &os, const std::vector<char>& cevc )
{
	copy( cevc.begin( ), cevc.end( ), std::ostreambuf_iterator<char>( os ) );
	return os;
}

inline void sleepx( const long int second, const long int msecond = 0L )
{
	timeval tv;
	tv.tv_sec = second;
	tv.tv_usec = msecond;

	::select( 0, NULL, NULL, NULL, &tv );
}

inline std::pair<std::string, std::string> divideString( const std::string& sLine, const std::string& sDelem )
{
	std::string::size_type pos = sLine.find( sDelem );
	if( pos != std::string::npos )
	{
		std::string s_key( sLine.substr( 0, pos ) );
		std::string s_value( sLine.substr( pos + sDelem.size( ) ) );

		return std::make_pair( s_key, s_value );
	}

	return std::make_pair( "", "" );
}

inline int gzcompress(const unsigned char *data, unsigned long ndata, unsigned char *zdata, unsigned long *nzdata)
{
	z_stream c_stream;
	int err = 0;

	if(data && ndata > 0)
	{
		c_stream.zalloc = NULL;
		c_stream.zfree = NULL;
		c_stream.opaque = NULL;

		// 只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
		if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
		{
			return -1;
		}

		c_stream.next_in = (unsigned char*)data;
		c_stream.avail_in = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out = *nzdata;
		while(c_stream.avail_in != 0 && c_stream.total_out < *nzdata)
		{
			if(deflate(&c_stream, Z_NO_FLUSH) != Z_OK)
			{
				return -1;
			}
		}

		if(c_stream.avail_in != 0)
		{
			return c_stream.avail_in;
		}

		while(true)
		{
			if((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END)
			{
				break;
			}

			if(err != Z_OK)
			{
				return -1;
			}
		}

		if(deflateEnd(&c_stream) != Z_OK)
		{
			return -1;
		}

		*nzdata = c_stream.total_out;
		return 0;
	}

	return -1;
}

inline int ungzipxp( const char *source, const int len, std::vector<char>& desvec )
{
	desvec.clear( );

	const unsigned long int uncomprLen = len * 4;

	Byte *uncompr = new Byte[uncomprLen];
	memset( uncompr, 0, uncomprLen );

	//但是gzip或者zlib数据里\0很多。

	strcpy( ( char* )uncompr, "garbage" );

	z_stream d_stream;
	d_stream.zalloc = Z_NULL;
	d_stream.zfree = Z_NULL;
	d_stream.opaque = Z_NULL;
	d_stream.next_in = Z_NULL;//inflateInit和inflateInit2都必须初始化next_in和avail_in
	d_stream.avail_in = 0;//deflateInit和deflateInit2则不用

	int ret = inflateInit2( &d_stream, 47 );
	if( ret != Z_OK )
	{
		rlog << "inflateInit2() error" << std::endl ;
		return ret;
	}

	d_stream.next_in = ( unsigned char * )source;
	d_stream.avail_in = len;

	do
	{
		d_stream.next_out = uncompr;
		d_stream.avail_out = uncomprLen;

		ret = inflate( &d_stream, Z_NO_FLUSH );
		if( ret == Z_STREAM_ERROR )
		{
			return -99;
		}

		switch( ret )
		{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd( &d_stream );
				return ret;
		}
		int have = uncomprLen - d_stream.avail_out;

		copy( uncompr, uncompr + have, back_inserter( desvec ) );
	}
	while( d_stream.avail_out == 0 );
	inflateEnd( &d_stream );
	delete []uncompr;
	return ret;
}

inline std::string genVid( bool isUpperCase = false )
{
	unsigned char out[16] ;
	memset( out, 0, sizeof( out ) ) ;
	uuid_generate_random( out ) ;

	std::string sRv ;
	for( std::size_t i = 0 ; i < sizeof( out ) ; ++i )
	{
		char buff[3] = "" ;
		if( isUpperCase )
			sprintf( buff, "%02X", out[i] & 0xff ) ;
		else
			sprintf( buff, "%02x", out[i] & 0xff ) ;

		sRv.push_back( buff[0] ) ;
		sRv.push_back( buff[1] ) ;
	}

	sRv.insert( 20, 1, '-' ) ;
	sRv.insert( 16, 1, '-' ) ;
	sRv.insert( 12, 1, '-' ) ;
	sRv.insert( 8, 1, '-' ) ;

	return	sRv ;
}



inline bool calc_md5(const std::string& s_path, std::string& s_md5 )
{
	if ( s_path.empty() )
	{
		rlog << "s_path empty" << std::endl ;
		return false ;
	}

	int	i_fd = open( s_path.c_str(), O_RDONLY ) ;
	if ( i_fd < 0 )
	{
		rlog << "open(" << s_path << ") " << strerror( errno )  << std::endl ;
		return false;
	}

	std::vector<char> v_md5_data ;

	while ( true )
	{
		char	buffer[4096] = { 0x00 } ;
		int size = ::read( i_fd, buffer, 4096 ) ;
		if ( size < 0 )
		{
			rlog << "read(" << s_path << ") " << strerror( errno )  << std::endl ;
			close( i_fd ) ;
			return -1 ;
		}

		if ( size == 0 )	break ;

		copy( buffer, buffer+size, back_inserter( v_md5_data ) ) ;
	}

	close( i_fd ) ;

	std::string s_data( v_md5_data.begin(), v_md5_data.end() ) ;
	unsigned char md[16] = { 0x00 };
	MD5((unsigned char*)s_data.c_str(), s_data.size(), md ) ;

	char tmp[3]={'\0'};
	char buf[33]={'\0'};
	for ( int i = 0 ; i < 16; i++ )
	{
		sprintf(tmp,"%2.2x",md[i]);
		strcat(buf,tmp);
	}

	s_md5 += buf;

	return true ;
}


#endif



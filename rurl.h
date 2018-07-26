#ifndef	_RURL_H_
#define	_RURL_H_

#include <stdlib.h>
#include <string>
#include <iostream>
using namespace std ;

#include "rui.h"

class rurl
{
	public:
		rurl() {}

		rurl( const string& s_url )
		{
			init( s_url ) ;
		}

		void clear( )
		{
			ms_host = "" ;
			ms_path = "" ;
			msh_port = 0 ;
		}

		void init( const string& s_url )
		{
			const string s_http_word( "http://" ) ;
			const string s_http_word1( "https://" ) ;

			bool isHttps = false ;
			string::size_type pos1 = s_url.find( s_http_word ) ;

			string s_data ;
			if( pos1 == string::npos )
			{
				pos1 = s_url.find( s_http_word1 ) ;
				if( pos1 == string::npos )
				{
					s_data = s_url ;
				}
				else {
					s_data = s_url.substr( pos1 + s_http_word1.size( ) ) ;
					isHttps = true ;
				}
			}
			else {
				s_data = s_url.substr( pos1 + s_http_word.size( ) ) ;
			}

			pos1 = 0u ;
			string sData1 ;
			string::size_type pos2 = s_data.find( "/" ) ;
			if( pos2 == string::npos )
			{
				sData1 = s_data ;
			}
			else {
				sData1 = s_data.substr( 0, pos2 ) ;
				ms_path = s_data.substr( pos2 ) ;
			}

			string::size_type pos3 = sData1.find( ":" ) ;
			if( pos3 == string::npos )
			{
				ms_host = sData1 ;

				if( !isHttps )	msh_port = 80 ;
				else	msh_port = 443 ;
			}
			else {
				ms_host = sData1.substr( 0, pos3 ) ;
				msh_port = atoi( sData1.substr( pos3 + 1 ).c_str( ) ) ;
			}
		}

		const string& host( ) const
		{
			return ms_host ;
		}

		const string& path( ) const
		{
			return ms_path ;
		}

		const unsigned short port( ) const
		{
			return msh_port ;
		}

		const string whole_url( ) const
		{
			string s_whole_url = "http://" + ms_host + ":" + lexical_cast<string>( msh_port ) + "/" + ms_path ;
			return s_whole_url ;
		}

	private:
		string ms_host ;
		string ms_path ;
		short msh_port ;
} ;
#endif

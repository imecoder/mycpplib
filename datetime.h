#ifndef _DATETIME_H_
#define _DATETIME_H_

#include <time.h>
#include <iostream>

namespace rui {
	class datetime
	{
		public:
			datetime( const time_t second = time( NULL ) );

			void load_from_second( const time_t second );
			void load_from_string( const std::string& s, const char *format );

			std::string to_string( const char *format );
			time_t to_time_t( );

			int get_day_of_week( ) const;

			int get_hour( ) const;
			int get_minute( ) const;
			int get_second( ) const;

			std::string to_http_string( );


	#ifdef OTL_STL
			void load_from_otl_datetime( const otl_datetime &date );
	#endif

		private:
			tm m_tm;
	};
}
#endif


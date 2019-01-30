#include "datetime.h"

namespace rui {

	datetime::datetime( const time_t second )
	{
		// 转换GTM
		load_from_second( second );
	}

	std::string datetime::to_http_string( )
	{
		//"Fri, 09 Jul 2010 09:14:09 GMT"
		return to_string( "%a, %d %b %Y %H:%M:%S GTM" );
	}

	void datetime::load_from_second( const time_t second )
	{
		// 转换GTM
		time_t t = second;
		tm *ptm = localtime( &t );
		m_tm = *ptm;
	}

	void datetime::load_from_string( const std::string& s, const char *format )
	{
		strptime( s.c_str( ), format, &m_tm );
	}

	std::string datetime::to_string( const char *format )
	{
		char szTime[50] = "";
		strftime( szTime, 50, format, &m_tm );

		return szTime;
	}

	time_t datetime::to_time_t( )
	{
		return mktime( &m_tm );
	}

	int datetime::get_day_of_week( ) const
	{
		return m_tm.tm_wday;	/* Days since Sunday ( 0-6 ) */
	}

	int datetime::get_hour( ) const
	{
		return m_tm.tm_hour;
	}

	int datetime::get_minute( ) const
	{
		return m_tm.tm_min;
	}

	int datetime::get_second( ) const
	{
		return m_tm.tm_sec;
	}

#ifdef OTL_STL
	void datetime::load_from_otl_datetime( const otl_datetime &date )
	{
		m_tm.tm_year = date.year - 1900;
		m_tm.tm_mon = date.month - 1;
		m_tm.tm_mday = date.day;
		m_tm.tm_hour = date.hour;
		m_tm.tm_min = date.minute;
		m_tm.tm_sec = date.second;
	}
#endif

}

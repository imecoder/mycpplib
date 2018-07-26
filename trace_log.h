#ifndef _TRACE_LOG_H_
#define _TRACE_LOG_H_

#include <time.h>
#include <string>
#include <iostream>
using namespace std ;

namespace rui {
	class trace_log
	{
	public:
		trace_log(const string& s, ostream &os = cout)  : m_os(os), m_s(s)
		{
			m_ltime = time(NULL);
			os << "{ " << s << endl;
		}

		~trace_log()
		{
			time_t m_ltime1 = time(NULL);
			m_os << "} (" << m_ltime1 - m_ltime  << ") " << m_s << endl << endl;
		}

	private:
		ostream &m_os;
		string m_s;
		time_t m_ltime;
	};
}

#endif

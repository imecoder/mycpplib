#ifndef _TRACE_LOG_H_
#define _TRACE_LOG_H_

#include <time.h>
#include <string>
#include <iostream>

namespace rui {
	class trace_log
	{
	public:
		trace_log(const std::string& s, std::ostream &os = cout)  : m_os(os), m_s(s)
		{
			m_ltime = time(NULL);
			os << "{ " << s << std::endl;
		}

		~trace_log()
		{
			time_t m_ltime1 = time(NULL);
			m_os << "} (" << m_ltime1 - m_ltime  << ") " << m_s << std::endl << std::endl;
		}

	private:
		std::ostream &m_os;
		std::string m_s;
		time_t m_ltime;
	};
}

#endif

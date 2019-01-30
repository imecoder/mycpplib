#include "crypto.h"
#include "task.h"
#include "net.h"
#include "parse_task.h"
#include "read_task.h"
#include "main_service.h"

#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/json.h>

void read_task::run()
{
	rui::mutex(mp_task.get()->m_lock);

	mp_task.get()->m_time = time(NULL) ;
	
	int ret = 0 ;
	
	rlog << "read(" << mp_task.get()->mi_fd << ")" << std::endl ;
	while ( true )
	{
		if ( mp_task.get()->mb_closed )
			goto quit;

		char buffer[SOCKET_BUFFER_SIZE] = { 0x00 } ;
		ret = ::read( mp_task.get()->mi_fd, buffer, SOCKET_BUFFER_SIZE ) ;
		if( ret < 0 )
		{
			if( errno == EINTR )
				continue ;

			if ( errno == EAGAIN )
				break;

			rlog << "read(" << mp_task.get()->mi_fd << ") error : " << strerror( errno ) << std::endl;
			mp_task.get()->mb_closed = true ;
			break ;
		}

		if ( ret == 0 )
		{
			rlog << "read(" << mp_task.get()->mi_fd << ") peer closed" << std::endl ;
			mp_task.get()->mb_closed = true ;
			break ;
		}

		copy( buffer, buffer+ret, back_inserter( mp_task.get()->mv_data ) ) ;
	}

	while(true)
	{
		if ( (int)mp_task.get()->mv_data.size() < 4 )
		{
			if ( mp_task.get()->mb_closed )
				goto quit;
			return ;
		}

		int length = 0 ;
		memcpy( (char*)&length, &mp_task.get()->mv_data[0], 4 ) ;

		if ( (int)mp_task.get()->mv_data.size() < length + 4 )
		{
			if ( mp_task.get()->mb_closed )
				goto quit;
			return;
		}

		std::string s_json(mp_task.get()->mv_data.begin()+4, mp_task.get()->mv_data.begin()+4+length) ;
		mp_task.get()->m_time = time(NULL) ;
		mp_task.get()->mv_data.erase(mp_task.get()->mv_data.begin(), mp_task.get()->mv_data.begin()+4+length) ;

		rlog << "read main(" << mp_task.get()->mi_fd << ") data " << std::endl << s_json << std::endl ;

		rui::threadpool::_instance().add_task(std::unique_ptr<threadtask>(new parse_task(mp_task, s_json))) ;
	}
	
quit:
	main_service::_instance().del_task(mp_task);
}


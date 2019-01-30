#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/json.h>

#include "task.h"
#include "net.h"
#include "parse_task.h"
#include "main_service.h"

void parse_task::run( )
{
	rui::mutex(mp_task.get()->m_lock);

	Json::Value root;
	Json::Reader reader ;

	if ( mp_task.get()->mb_closed )
		goto quit;

	try
	{
		if ( !reader.parse( ms_json, root ) )
		{
			rlog << "rui::json::parse(" << mp_task.get()->mi_fd << ") json error" << std::endl ;
			rui::json::write_failure(mp_task.get()->mi_fd, "json error" ) ;
			goto quit;
		}

		rlog << root[0].asInt() << "," << root[1].asString() << std::endl ;

		return ;
	}
	catch( ... )
	{
		rlog << "task(" << mp_task.get()->mi_fd << ") json error" << std::endl ;
		rui::json::write_failure( mp_task.get()->mi_fd, "json error" ) ;
		goto quit;
	}

quit:
	main_service::_instance().del_task(mp_task);
}



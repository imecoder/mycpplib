#ifndef _PROTOCOL_PARSE_TASK_H_
#define _PROTOCOL_PARSE_TASK_H_

#include <memory>
#include <string>

#include "threadpool.h"
#include "task.h"

class parse_task : public rui::threadtask
{
	public:
		parse_task( std::shared_ptr<rui::task> p_task, std::string& s_json) :
			mp_task(p_task),
			ms_json(s_json)
		{}

		virtual void run( );
		
		std::shared_ptr<rui::task> mp_task ;
		std::string ms_json;
};

#endif


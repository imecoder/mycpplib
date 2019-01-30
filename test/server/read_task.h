#ifndef	_READ_TASK_H_
#define	_READ_TASK_H_

#include <memory>

#include "threadpool.h"
#include "task.h"

class read_task : public rui::threadtask
{
	public:
		read_task( std::shared_ptr<rui::task> p_task ) : mp_task(p_task) {}
		virtual void run();

		std::shared_ptr<rui::task> mp_task ;
};

#endif

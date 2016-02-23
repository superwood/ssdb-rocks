/*
 * reactor.cpp
 *
 *  Created on: 2016年2月4日
 *      Author: woodsuper
 */

#include "reactor.h"

reactor::reactor() {
	// TODO Auto-generated constructor stub

}

reactor::~reactor() {
	// TODO Auto-generated destructor stub
}

bool reactor::isset(int fd, int flag){
	return fdevents.isset(fd, flag);
}

int reactor::add(int fd, int flags, int data_num, void *data_ptr){
	return fdevents.set(fd, flags, data_num, data_ptr);
}

int reactor::del(int fd){
	return fdevents.del(fd);
}

int reactor::update(int fd, int flags){
	return fdevents.clr(fd, flags);
}

int reactor::handle_err_events(const Fdevent *fde, event_handler* eh){
	//if ( fde->events & FDEVENT_ERR){
	//	del(fde->fd);
	//	log_warn("fd event err. remote ip:%s:%d",eh->get_link()->remote_ip
	//}			eh->get_link()->remote_port);
	del(fde->fd);
	delete eh;
	return 0;
}


int reactor::do_events(int time_out){
	const Fdevents::events_t *events;
	events = fdevents.wait(time_out);
	for(int i=0; i<(int) ( events->size()); i++){
		const Fdevent *fde = events->at(i);
		event_handler* eh = (event_handler*) (fde->data.ptr);
		if(fde->events & FDEVENT_ERR ){//当前fd事件异常
			handle_err_events(fde, eh);
			continue;
		}

		int ret = eh->process(fde);

		if( 0 != ret){//处理当前fd事件时候，出现异常现象
			handle_err_events(fde, eh);
			continue;
		}
	}
	return 0;
}

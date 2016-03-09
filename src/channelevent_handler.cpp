/*
 * workerevent_handler.cpp
 *
 *  Created on: 2016年2月14日
 *      Author: woodsuper
 */

#include "channelevent_handler.h"

channelevent_handler::channelevent_handler() {
	// TODO Auto-generated constructor stub

}

channelevent_handler::~channelevent_handler() {
	// TODO Auto-generated destructor stub
}

int channelevent_handler::process(const Fdevent *fde){
	Link* link = NULL;
	int ret = queue_->pop(&link);
	if(  -1 == ret || NULL == link){
		return 0;
	}
	if( link->error() ){
		delete link;
		link = NULL;
		return 0;
	}
	log_debug("worker reactor %d receive link, remote host %s:%d",
			rtr_->thread_id, link->remote_ip, link->remote_port);

	workerevent_handler* w_handler = new workerevent_handler();
	w_handler->set_dbserver(rtr_->serverdb);
	w_handler->set_link(link);
	w_handler->set_reacotr(rtr_);
	rtr_->add(link->fd(), FDEVENT_IN, 1,(void*)w_handler);

	return 0;
}

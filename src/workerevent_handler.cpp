/*
 * workerevent_handler.cpp
 *
 *  Created on: 2016年2月14日
 *      Author: woodsuper
 */

#include "workerevent_handler.h"

workerevent_handler::workerevent_handler() {
	// TODO Auto-generated constructor stub

}

workerevent_handler::~workerevent_handler() {
	// TODO Auto-generated destructor stub
}


int workerevent_handler::process_write(){
	int len = link_->write();
	if(len <= 0){
		log_debug("fd: %d, write: %d, delete link", link_->fd(), len);
		link_->mark_error();
		return -1;
	}
	if(link_->output->empty()){
		rtr_->update(link_->fd(), FDEVENT_OUT);
	}

	return 0;
}


int workerevent_handler::process_read(){
	int len = link_->read();
	if(len <= 0){
		log_debug("fd: %d, read: %d, delete link, remote %s:%d", link_->fd(), len,
				link_->remote_ip, link_->remote_port);
		link_->mark_error();
		return -1;
	}
	const Request *req = link_->recv();
	if( NULL == req ){
		log_debug("fd: %d, read: %d, delete link, remote %s:%d", link_->fd(), len,
						link_->remote_ip, link_->remote_port);
		return -1;
	}

	if(req->empty()){
		rtr_->add(link_->fd(), FDEVENT_IN, 1, this);
		return 0;
	}
	link_->active_time = millitime();
	ProcJob job;
	job.link = link_;
	int thread_id = rtr_->thread_id;
	serv_->proc(&job, thread_id);

	return 0;
}

int workerevent_handler::process(const Fdevent *fde){
	if( NULL == link_ || link_->error() ){
		return -1;
	}

	if(fde->events & FDEVENT_IN){//可读事件触发
		return process_read();
	}

	if(fde->events & FDEVENT_OUT){
		return process_write();
	}
	return 0;
}

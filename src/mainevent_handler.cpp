/*
 * mainevent_handler.cpp
 *
 *  Created on: 2016年2月14日
 *      Author: woodsuper
 */

#include "mainevent_handler.h"

mainevent_handler::mainevent_handler() {
	// TODO Auto-generated constructor stub
	index_ = 0;
}

mainevent_handler::~mainevent_handler() {
	// TODO Auto-generated destructor stub
}

void mainevent_handler::set_link(Link* link){
	link_ = link;
}

void mainevent_handler::set_queues(SelectableQueue<Link*>* queues){
	quws_ = queues;
}

int mainevent_handler::process(const Fdevent *fde){
	Link *link = link_->accept();
	if( NULL == link ){
		return -1;
	}
	link->nodelay();
	link->noblock();
	link->create_time = millitime();
	link->active_time = link->create_time;


	dbserv_->link_count++;
	log_debug("new link from %s:%d, fd: %d, links: %d",
								link->remote_ip, link->remote_port, link->fd(), dbserv_->link_count);
	index_=(index_+1)%total_;
	quws_[index_++].push(link);
	return 0;
}

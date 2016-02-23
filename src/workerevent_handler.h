/*
 * workerevent_handler.h
 *
 *  Created on: 2016年2月14日
 *      Author: woodsuper
 */

#ifndef WORKEREVENT_HANDLER_H_
#define WORKEREVENT_HANDLER_H_

#include "event_handler.h"
#include "include.h"
#include "link.h"
#include "reactor.h"
class workerevent_handler: public event_handler {
public:
	workerevent_handler();
	virtual ~workerevent_handler();
	void set_link(Link* link){
		link_ = link;
	}
	void set_reacotr( reactor* rtr){
		rtr_ = rtr;
	}
	void set_dbserver(Server* server){
		serv_ = server;
	}
	int process(const Fdevent *fde);
	int process_read();
	int process_write();
private:
	Link* link_;
	reactor* rtr_;
	Server* serv_;
};

#endif /* WORKEREVENT_HANDLER_H_ */

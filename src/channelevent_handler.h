/*
 * channelevent_handler.h
 *
 *  Created on: 2016年2月14日
 *      Author: woodsuper
 */

#ifndef CHANNELEVENT_HANDLER_H_
#define CHANNELEVENT_HANDLER_H_

#include "event_handler.h"
#include "reactor.h"
#include "include.h"
#include "workerevent_handler.h"
#include "log.h"
class channelevent_handler: public event_handler {
public:
	channelevent_handler();
	virtual ~channelevent_handler();
	void set_queue(SelectableQueue<Link *>* queue){
		queue_ = queue;
	}
	void set_reactor(reactor* rtr){
		rtr_ = rtr;
	}
	int process(const Fdevent *fde);
private:
	SelectableQueue<Link *>* queue_;
	reactor* rtr_;
};

#endif /* CHANNELEVENT_HANDLER_H_ */

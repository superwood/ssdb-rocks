/*
 * mainevent_handler.h
 *
 *  Created on: 2016年2月14日
 *      Author: woodsuper
 */

#ifndef MAINEVENT_HANDLER_H_
#define MAINEVENT_HANDLER_H_

#include "event_handler.h"
#include "util/log.h"
#include "util/thread.h"
#include "include.h"
class mainevent_handler: public event_handler {
public:
	mainevent_handler();
	virtual ~mainevent_handler();
	void set_link(Link* link);
	void set_queues(SelectableQueue<Link*>* queues);
	void set_dbsever(Server* dbserv){
		dbserv_ = dbserv;
	}
	void set_total(int total){
		total_ = total;
	}
	int process(const Fdevent *fde);
private:
	int   index_;
	int   total_;
	Server* dbserv_;
	Link* link_;
	SelectableQueue<Link *>* quws_;
};

#endif /* MAINEVENT_HANDLER_H_ */

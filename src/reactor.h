/*
 * reactor.h
 *
 *  Created on: 2016年2月4日
 *      Author: woodsuper
 */

#ifndef REACTOR_H_
#define REACTOR_H_
#include "util/fde.h"
#include "link.h"
#include "event_handler.h"
#include "mainevent_handler.h"
#include "util/log.h"
#include "serv.h"
class reactor {
public:
	reactor();
	virtual ~reactor();
	bool isset(int fd, int flag);
	int add(int fd, int flags, int data_num, void *data_ptr);
	int del(int fd);
	int update(int fd, int flags);
	int handle_err_events(const Fdevent *fde, event_handler* eh);
	int do_events(int time_out=-1);
public:
	int thread_id;
	Server* serverdb;
private:
	Fdevents fdevents;
};

#endif /* REACTOR_H_ */

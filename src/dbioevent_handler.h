/*
 * dbioevent_handler.h
 *
 *  Created on: 2016年2月19日
 *      Author: woodsuper
 */

#ifndef DBIOEVENT_HANDLER_H_
#define DBIOEVENT_HANDLER_H_

#include "event_handler.h"
#include "event_handler.h"
#include "reactor.h"
#include "include.h"
#include "workerevent_handler.h"

class dbioevent_handler: public event_handler {
public:
	dbioevent_handler();
	virtual ~dbioevent_handler();
	void set_queue(WorkerPool<Server::ProcWorker, ProcJob>* queue){
		queue_ = queue;
	}
	void set_reactor(reactor* rtr){
		rtr_ = rtr;
	}
	void set_dbserver(Server* server){
		serv_ = server;
	}
	int process_read(ProcJob &job);
	int process(const Fdevent *fde);
	int process_res(ProcJob &job);
	int process_err(ProcJob &job);
private:
	WorkerPool<Server::ProcWorker, ProcJob> * queue_;
	reactor* rtr_;
	Server* serv_;
};

#endif /* DBIOEVENT_HANDLER_H_ */

/*
 * worker_reactor_pool.h
 *
 *  Created on: 2016年2月18日
 *      Author: woodsuper
 */

#ifndef WORKER_REACTOR_POOL_H_
#define WORKER_REACTOR_POOL_H_
#include "include.h"
#include <map>
#include <vector>
#include <string>
#include "util/thread.h"
#include "ttl.h"
#include "reactor.h"

class WorkerReactorPool {
public:
	WorkerReactorPool(){}
	WorkerReactorPool(int workernumber);

	virtual ~WorkerReactorPool();

	class ProReactor:public WorkerPool<ProReactor, reactor*>::Worker{
	public:
		ProReactor(){}
		ProReactor(const std::string &name){}
		~ProReactor(){}
		int proc(reactor ** rtr);
	};
	WorkerPool<ProReactor, reactor*> *  reactors_;
private:
	int nums_ ;

};

#endif /* WORKER_REACTOR_POOL_H_ */

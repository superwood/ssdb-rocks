/*
 * worker_reactor_pool.cpp
 *
 *  Created on: 2016年2月18日
 *      Author: woodsuper
 */

#include "worker_reactor_pool.h"

WorkerReactorPool::WorkerReactorPool(int workernumber) {
	nums_ = workernumber;
	reactors_ = new WorkerPool<ProReactor, reactor*>("reactors");
	reactors_->start(nums_);
}

WorkerReactorPool::~WorkerReactorPool() {
	if( NULL != reactors_){
		delete reactors_;
	}
}
int WorkerReactorPool::ProReactor::proc(reactor** rtr){
	while(1){
		(*rtr)->do_events();
	}
}

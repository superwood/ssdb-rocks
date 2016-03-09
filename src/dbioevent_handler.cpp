/*
 * dbioevent_handler.cpp
 *
 *  Created on: 2016年2月19日
 *      Author: woodsuper
 */

#include "dbioevent_handler.h"

dbioevent_handler::dbioevent_handler() {
	// TODO Auto-generated constructor stub

}

dbioevent_handler::~dbioevent_handler() {
	// TODO Auto-generated destructor stub
}
int dbioevent_handler::process_err(ProcJob &job){
	rtr_->del(job.link->fd());
	delete job.eh;
	job.eh = NULL;
	return -1;
}
int dbioevent_handler::process_read(ProcJob &job){//处理fd上剩余的请求
	Link *link = job.link;
	int len = link->read();
	if(len <= 0){
		log_debug("fd: %d, read: %d, delete link, remote %s:%d", link->fd(), len,
				link->remote_ip, link->remote_port);
		link->mark_error();
		process_err(job);
		return 0;
	}
	const Request *req = link->recv();
	if( NULL == req ){
		log_debug("fd: %d, read: %d, delete link, remote %s:%d", link->fd(), len,
						link->remote_ip, link->remote_port);
		process_err(job);
		return 0;
	}

	if(req->empty()){
		rtr_->add(link->fd(), FDEVENT_IN, 1, this);
		return 0;
	}
	link->active_time = millitime();
	ProcJob job1;
	job1.link = link;
	job1.eh = job.eh;
	int thread_id = rtr_->thread_id;
	serv_->proc(&job1, thread_id);

	return 0;

}
int dbioevent_handler::process_res(ProcJob &job){
	Link *link = job.link;
	log_debug("worker reactor %d receive link, remote host %s:%d",
			rtr_->thread_id, link->remote_ip, link->remote_port);
	int len;
	double stime = millitime();
	double time_wait =stime -job.tmp_stime;
	double stime1 = 0.0;
	double time_proc = 0.0;
	double time_wait_for_q = stime -job.tmp_stime2;
	if(job.cmd){
		job.cmd->calls += 1;
		job.cmd->time_wait += job.time_wait;
		job.cmd->time_proc += job.time_proc;
	}

	do{
		if(  PROC_ERROR == job.result ){
			continue; //出错跳出
		}

		len = link->write();
		if( len < 0 ){
			log_debug("fd: %d, write: %d, delete link", link->fd(), len);
			continue;
		}
		if(!link->output->empty()){
			rtr_->add(link->fd(), FDEVENT_OUT, 1, job.eh);
		}

		if( link->input->empty()){
			rtr_->add(link->fd(), FDEVENT_IN, 1, job.eh);
		}else{
			rtr_->update(link->fd(), FDEVENT_IN);//去掉可读事件
			process_read(job);
		}

		return 0;
	}while(false);

	process_err(job);
	return -1;
}
int dbioevent_handler::process(const Fdevent *fde){
	ProcJob job;
	if( 0 == queue_->pop(&job) ){
		log_fatal("reading result from workers error!");
		return -1;
	}

	process_res(job);
	return 0;
}

/*
 * event_handler.h
 *
 *  Created on: 2016年2月14日
 *      Author: woodsuper
 */

#ifndef EVENT_HANDLER_H_
#define EVENT_HANDLER_H_
#include "link.h"
#include "util/fde.h"
#include "serv.h"
#include "include.h"
class event_handler {
public:
	event_handler();
	virtual ~event_handler();
	virtual int process(const Fdevent *fde);
};

#endif /* EVENT_HANDLER_H_ */

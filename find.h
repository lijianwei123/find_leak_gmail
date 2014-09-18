/*
 * find.h
 *
 *  Created on: 2014-9-17
 *  Author: lijianwei
 */

#ifndef FIND_H_
#define FIND_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/bufferevent_struct.h>
#include <event2/buffer_compat.h>

#include "uthash.h"

struct gmail
{
	char *email;
	UT_hash_handle hh;
};


#endif /* FIND_H_ */

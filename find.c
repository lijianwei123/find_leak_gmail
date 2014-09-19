/*
 * find.c
 *
 *  Created on: 2014-9-17
 *  Author: lijianwei
 */


#include "find.h"

#define SOCK_PORT   9999

struct gmail *head = NULL;
char *gmail_filename = NULL;

struct event_base *base = NULL;
struct evconnlistener *listener = NULL;
struct event *signal_event1= NULL, *signal_event2 = NULL;

//添加
void insert_email(char *email)
{
	struct gmail *add, *tmp = NULL;
	add = calloc(1, sizeof(struct gmail));
	add->email = strdup(email);

	//先查找是否已经存在
	HASH_FIND_STR(head, email, tmp);

	if (tmp == NULL) {
		HASH_ADD_STR(head, email, add);
	} else {
		free(add->email);
		free(add);
	}
}

//查找
int find_email(char *email)
{
	struct gmail *tmp = NULL;
	HASH_FIND_STR(head, email, tmp);

	return (tmp != NULL);
}

void free_hash()
{
	struct gmail *current = NULL, *tmp = NULL;
	HASH_ITER(hh, head, current, tmp) {
		HASH_DEL(head, current);
		free(current->email);
		free(current);
	}
}

static void  finish()
{
	sleep(1);

	if (gmail_filename)
		free(gmail_filename);

	if (head)
		free_hash();

	if (listener)
		evconnlistener_free(listener);

	if (signal_event1)
		event_free(signal_event1);

	if (signal_event2)
		event_free(signal_event2);


	if (base)
		event_base_free(base);

	printf("quit!!!\n");
	exit(EXIT_FAILURE);
}


static void conn_errorcb(struct bufferevent *bev, short events, void *ctx)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n", strerror(errno));
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
}



static void conn_readcb(struct bufferevent *bev, void *ctx)
{
#ifdef DEBUG
	printf("conn_readcb starting\n");
#endif
	char *email = NULL;
	int retCode = -1;
	struct evbuffer *evreturn = NULL;

	email = evbuffer_readline(bev->input);

	if (email != NULL) {
#ifdef DEBUG
	printf("%s\n", email);
#endif
		retCode = find_email(email);
		evreturn = evbuffer_new();
		evbuffer_add_printf(evreturn, "%d", retCode);
		bufferevent_write_buffer(bev, evreturn);

#ifdef DEBUG
	printf("bev output length:%d, %d\n", evbuffer_get_length(bufferevent_get_output(bev)), retCode);
#endif

		evbuffer_free(evreturn);
		free(email);
	}
}

//@see http://blog.csdn.net/feitianxuxue/article/details/9386843
//可以看出struct bufferevent内置了两个event（读/写）和对应的缓冲区。
//当有数据被读入(input)的时候，readcb被调用，当output被输出完成的时候，writecb被调用，当网络I/O出现错误，如链接中断，超时或其他错误时，errorcb被调用。
static void conn_writecb(struct bufferevent *bev, void *user_data)
{
#ifdef DEBUG
	printf("conn_writecb starting\n");
#endif
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0) {
		printf("flushed answer\n");
		bufferevent_free(bev);
	}
}


static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *sa, int socklen, void *user_data)
{
	struct event_base *base = user_data;
	struct bufferevent *bev = NULL;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}
	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_errorcb, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	//bufferevent_disable(bev, EV_READ);
	//bufferevent_write(bev, MESSAGE, strlen(MESSAGE));
}

static void signal_cb(evutil_socket_t sig, short events, void *user_data)
{
	struct event_base *base = user_data;
	struct timeval delay = { 1, 0 };

	fprintf(stderr, "signo:%d, %s\n", sig, "Caught an interrupt signal; exiting cleanly in one seconds.");
	event_base_loopexit(base, &delay);

	finish();
}




int main(int argc, char **argv)
{
	FILE *fp = NULL;
	char gmail[100] = {0};
	int lineNum = 0;

	if (argc < 2) {
		fprintf(stderr, "%s\n", "argc num < 1");
		exit(EXIT_FAILURE);
		return -1;
	}

	gmail_filename = strdup(argv[1]);
	if (access(gmail_filename, F_OK) < 0) {
		fprintf(stderr, "%s don't exist!", gmail_filename);
		exit(EXIT_FAILURE);
		return -1;
	}

	fp = fopen(gmail_filename, "r");
	while(fgets(gmail, 100, fp) != NULL) {
		//处理多余的\n字符
		if (gmail[strlen(gmail) - 1] == '\n')
			gmail[strlen(gmail) - 1] = '\0';
		lineNum++;
#ifdef DEBUG
		printf("lineNum:%d, gmail:%s\n", lineNum, gmail);
#endif
		insert_email(gmail);
		memset(gmail, 0, 100);
	}
	fclose(fp);


#ifdef DEBUG
	printf("find lijianwei: %d\n", find_email("lijianwei"));
#endif

	//libevent 提供socket服务
	struct sockaddr_in sin;

	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return -1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SOCK_PORT);

	listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
		LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));

	if (!listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return -1;
	}

	signal_event1 = evsignal_new(base, SIGINT, signal_cb, (void *)base);
	signal_event2 = evsignal_new(base, SIGTERM, signal_cb, (void *)base);


	if (!signal_event1 || event_add(signal_event1, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return -1;
	}

	if (!signal_event2 || event_add(signal_event2, NULL)<0) {
		fprintf(stderr, "Could not create/add a signal event!\n");
		return -1;
	}


	event_base_dispatch(base);

	finish();

	return 0;
}



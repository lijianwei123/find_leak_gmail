/**
 * 多线程测试 find.c  -lpthread
 * 使用./find_test  10  100   并发10  请求数100
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <sys/time.h>

#define SERVER_PORT  9999
#define SERVER_IP   "127.0.0.1"

int flag = 0;

void *test(void *arg)
{
	sleep(3);
	while(!flag);

	int perRequest = (int)*(int *)arg;
	int i = 0;

	int clientSocket;
	int retCode;
	char sendBuf[100];
	char recvBuf[100];

	struct sockaddr_in serverAddr;

#ifdef DEBUG
	printf("%s\n", "thread starting...");
#endif

	for (i = 0; i < perRequest; i++) {
		clientSocket = 0;
		retCode = -1;

		memset(sendBuf, 0, sizeof(sendBuf));
		memset(recvBuf, 0, sizeof(recvBuf));
		memset(&serverAddr, 0, sizeof(struct sockaddr_in));

		//创建个socket
		if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("create socket error");
			exit(EXIT_FAILURE);
		}

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERVER_PORT);
		serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

		//连接服务端
		if(connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
			perror("connect server error");
			exit(EXIT_FAILURE);
		}

		strcpy(sendBuf, "vijayalaxmip@gmail.com\n");
		retCode = send(clientSocket, sendBuf, strlen(sendBuf), 0);
		if (retCode < 0) {
			perror("send data error");
			exit(EXIT_FAILURE);
		}

		retCode = recv(clientSocket, recvBuf, sizeof(recvBuf), 0);
		if (retCode < 0) {
			perror("recv data error");
			exit(EXIT_FAILURE);
		}

		sendBuf[strlen(sendBuf) - 1] = '\0';
		printf("pthread id: %ld, i: %d, send email: %s, recv data:%s\n", pthread_self(), i, sendBuf, recvBuf);

		close(clientSocket);
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	//并发
	int c = 0;
	//请求数
	int n = 0;
	int perRequest = 0;
	int i = 0;


	if (argc < 3) {
		fprintf(stderr, "argc num error!\n");
		return -1;
	}

	c = atoi(argv[1]);
	n = atoi(argv[2]);

	if (c < 0 || n < 0 || n < c) {
		fprintf(stderr, "args error\n");
		return -1;
	}

	perRequest = n / c;

#ifdef DEBUG
	printf("request num:%d, concurrency num:%d, perrequest:%d\n", n, c, perRequest);
#endif
	//线程

	pthread_t threadIds[c];
	memset(threadIds, 0, c * sizeof(pthread_t));

	//线程属性
	pthread_attr_t attr;
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED); //参考http://www.cnblogs.com/bits/archive/2009/12/04/no_join_or_detach_memory_leak.html

	//线程锁
	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);
	//条件
	pthread_cond_t cond;
	pthread_cond_init(&cond, NULL);
	
	int temp = 0;

	//启动线程
	for (i = 0; i < c; i++) {
		temp = pthread_create(&threadIds[i], NULL, test, (void *)&perRequest);
		if (temp != 0) {
		     fprintf(stderr, "pthread_create error");
		}
	}
	
	//start
	flag = 1;
	
	//等待结束
	for (i = 0; i < c; i++) {
		temp = pthread_join(threadIds[i], NULL);
		if (temp != 0) {
			perror("pthread_join");
		}
	}

#ifdef DEBUG
	printf("%s\n", "ending...");
#endif


	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);

	return 0;
}

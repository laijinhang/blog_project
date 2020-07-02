#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

const int port = 8001;
int thread_count = 0;
const int thread_num = 1024;
#define MAX_THREAD_NUM 1024
#define BUFFER_LENGTH 1024

pthread_t threads[MAX_THREAD_NUM];

/**
 * 接收数据的执行函数
 * @param conn_socket
 */
void handler(void *conn_socket);

int main() {
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
    	perror("socket");
        return 1;
	}
    struct sockaddr_in server_addr;
    bzero(&(server_addr), sizeof(server_addr));

    pthread_t a_thread;
    pthread_attr_t thread_attr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    	perror("bind");
        exit(0);
    }

    if(listen(server_socket, 10) < 0) {
        perror("listen");
        return 0;
    }
    printf("listen....\n");

	bzero(&threads, sizeof(pthread_t) * MAX_THREAD_NUM);
	while(1) {
		struct sockaddr_in client_addr;
		int length = sizeof(client_addr);

		// 建立客户端连接
		int client_conn = accept(server_socket, (struct sockaddr*) &client_addr, &length);
		if (client_conn < 0) {
			printf("Server Accept Failed!\n");
			return EXIT_FAILURE;
		}

		// 新建线程, 使用新线程与客户端交互
		int pthread_err = pthread_create(threads + (thread_count++), NULL, (void *)handler, (void *)&client_conn);
		if (pthread_err != 0) {
			printf("Create thread Failed!\n");
			return EXIT_FAILURE;
		}
	}
    return 0;
}

void handler(void *conn_socket) {
    fflush(NULL);
    int fd = *((int *)conn_socket);
    int i_recvBytes;
    char data_recv[BUFFER_LENGTH];
    const char * data_send = "Server has received your request!\n";
    while(1)
    {
        //Reset data.
        memset(data_recv,0,BUFFER_LENGTH);

        i_recvBytes = read(fd,data_recv,BUFFER_LENGTH);
        if(i_recvBytes == 0)
        {
            printf("Maybe the client has closed\n");
            break;
        }
        if(i_recvBytes == -1)
        {
            fprintf(stderr,"read error!\n");
            break;
        }
        printf("read from client : %s\n",data_recv);
        if(write(fd,data_send,strlen(data_send)) == -1)
        {
            break;
        }
    }
     close(fd);            //close a file descriptor.
     pthread_exit(NULL);   //terminate calling thread
};

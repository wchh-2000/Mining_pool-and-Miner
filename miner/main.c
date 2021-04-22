//https://www.cnblogs.com/fisherss/p/12085123.html 
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "wsock32.lib")
#include <errno.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include<ws2tcpip.h>
#include <stdio.h>
#define SERVER_PORT 6666

#include "mining.h"
pthread_t mining_p;
int find=0,nextjob=0;
int main()
{
	//客户端只需要一个套接字文件描述符，用于和服务器通信
	int serverSocket;
	//描述服务器的socket
	struct sockaddr_in serverAddr;
	 
	char sendbuf[200]; //存储 发送的信息 
	char recvbuf[joblength]; //存储 接收到的信息 
	int len,i;
	unsigned long ul=1;//用于设置非阻塞 允许非阻塞模式则非零
	WSADATA wsaData;
	Arg arg;
	arg.isrun=0;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) !=2){
	    printf("require version fail!");
	    return -1;
	}
	if((serverSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
	{
		perror("socket");
		return 1;
	}
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	//指定服务器端的ip，本地测试：127.0.0.1
	//inet_addr()函数，将点分十进制IP转换成网络字节序IP
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");// 本机ip 192.168.1.105 
	printf("connecting...\n");
	if(connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect");
		return 1;
	}
	len=recv(serverSocket, recvbuf, joblength, 0);
	printf("miner%s connected\n",recvbuf);
	if(SOCKET_ERROR==ioctlsocket(serverSocket,FIONBIO,(unsigned long *)&ul))//设置成非阻塞模式
		puts("nonblock failed\n");
	while(1)
	{
		len = recv(serverSocket, recvbuf, joblength, 0); //接收服务端发来的消息
		//if(len>0)
		//printf("%d ",len);
		if(strstr(recvbuf,"find")!=NULL) 
		{
			if(arg.isrun)
				pthread_cancel(mining_p);//终止挖矿线程 
			else
				nextjob=0;
		}
		if(strstr(recvbuf,"finish")!=NULL) //所有区块生成完毕 
			break;
		if(len==joblength)//接收到任务 
		{
			for(i=0;i<blockBytes;i++)
				arg.str[i]=recvbuf[i];
			arg.str[blockBytes]='\0';
			arg.n0=(recvbuf[joblength-2]-'0')*10+recvbuf[joblength-1]-'0'; 
			printf("\nstart minining,N0=%d\n",arg.n0);
			pthread_create(&mining_p,NULL,Mining,(void *)&arg);//创建挖矿线程
		}
		//else if(WSAGetLastError()==WSAEWOULDBLOCK) continue;//接收缓冲区无数据 
		else if(len==8 && strstr(recvbuf,"find")==NULL)//收到nonce1 且没有矿工找到时 &&strstr(recvbuf,"find")==NULL
		{
			//printf("\nnonce1:");
			for(i=0;i<8;i++)
			{
				arg.str[40+i]=recvbuf[i];//更新nonce1 
				//printf("%x ",recvbuf[i]);
			}
			//printf("nonce1=%lld\n",str2long((BYTE *)recvbuf));//问题 
			pthread_create(&mining_p,NULL,Mining,(void *)&arg);//创建挖矿线程
		} 
		
		if(find)
		{
			find=0;
			strcpy(sendbuf,"find");
			for(i=0;i<16;i++)
				sendbuf[i+4]=arg.result[i];//16bytes的nonce1 nonce2加在find后
			send(serverSocket, sendbuf, 20, 0); 
		}
		if(nextjob)
		{
			strcpy(sendbuf,"nextjob");
			send(serverSocket, sendbuf, 7, 0); 
			nextjob=0;
		} 
		
	}

	close(serverSocket);
    system("pause");
	return 0;
}


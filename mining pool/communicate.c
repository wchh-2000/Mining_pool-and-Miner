//https://blog.csdn.net/S_O_L_O_R/article/details/79069741
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#include "communicate.h"
#include "server.h"
static client_list_node_st client_list[MAX_CLIENT_NUMS] = {0}; //客户端列表 communicate.h中定义 
static SOCKET socket_of_server;  //服务端（本地）的socket
static struct sockaddr_in s_sin; //用于存储本地创建socket的基本信息
pthread_t miner_p[MAX_CLIENT_NUMS];//服务器与miner通信线程指针 

extern int newblock, newblockdone,find,finish,newNonce;//main中定义 
extern long long n1,n2,nonce1;
extern BYTE str[];
static int    client_nums = 0;
void* connection(void)
{
	int port = 6666;
    int i = 0;
    WORD socket_version = MAKEWORD(2, 2);
    WSADATA wsadata;
    unsigned long ul=1;//用于设置非阻塞 允许非阻塞模式则非零
    if (WSAStartup(socket_version, &wsadata) != 0)
    {
        return 0;
    }
    socket_of_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    //创建socket 并判断是否创建成功
    if (socket_of_server == INVALID_SOCKET)
    {
        printf("socket error\n");
        return 0;
    }
    s_sin.sin_family = AF_INET;  //定义协议族为IPV4
    s_sin.sin_port = htons(port);//规定端口号
    s_sin.sin_addr.S_un.S_addr = INADDR_ANY;
    /************************************************************************
    s_sin.sin_addr.S_un.S_addr = INADDR_ANY; 是在设定绑定在本机的哪个IP地址上，
    在哪个IP地址上进行监听。设定为INADDR_ANY代表0.0.0.0就是默认IP。
    正常个人编程时 这个地方无关紧要。但若真正应用时这个地方最好设置清楚。
    因为好多服务器是多个网卡，本机有多个IP。哪个网卡是连接服务所在局域网的就要
    设置为哪个。
    ************************************************************************/
    if (bind(socket_of_server, (LPSOCKADDR)&s_sin, sizeof(s_sin)) == SOCKET_ERROR)//绑定
    {
        printf("bind error\n");
    }
    if (listen(socket_of_server, 5) == SOCKET_ERROR)//监听
    {
        printf("listen error\n");
        return 0;
    }
    printf("服务已经开启 PORT：%d，正在等待连接... ...\n", port);
    while (!finish)//全局finish 
    {
        SOCKET socket_of_client;  //客户端（远程）的socket
        struct sockaddr_in c_sin; //用于存储已连接的客户端的socket基本信息
        int    c_sin_len;         //函数accept的第三个参数，c_sin的大小。
        c_sin_len = sizeof(c_sin);
        socket_of_client = accept(socket_of_server, (SOCKADDR *)&c_sin, &c_sin_len);
        /************************************************************************
        没有新的连接是 程序不会一直在这里循环。此时accept会处于阻塞状态。
        直到有新的连接，或者出现异常。
        ************************************************************************/
        if (socket_of_client == INVALID_SOCKET)
        {
            //printf("accept error\n");
            continue; //继续等待下一次连接
        }
        else
        {
        	if (client_nums + 1 > MAX_CLIENT_NUMS)
            {
                send(socket_of_client, "连接超限制，您已被断开 \n", strlen("连接超限制，您已被断开 \n"), 0);
                printf("有新的矿机请求连入，但由于已经达到最大连接数，该设备已经被强行断开\n");
                Sleep(1000);//?
                closesocket(socket_of_client);
                continue;
            }
            else
            {
            	if(SOCKET_ERROR==ioctlsocket(socket_of_client,FIONBIO,(unsigned long *)&ul))//设置成非阻塞模式用于循环收发 
						puts("nonblock failed\n");
	        	int j=client_nums; 
	        	client_list[j].is_run = 1;
	        	client_list[j].num=j+1;//编号从1开始 
	        	client_list[j].sendjob=0;//初始化未发送job 
	            client_list[j].socket_client = socket_of_client;
	            client_list[j].c_sin;
	            memcpy(&(client_list[j].c_sin), &c_sin, sizeof(c_sin));
	            if (client_list[j].h)
	            {
	                CloseHandle(client_list[j].h);
	            }
	            pthread_create(&miner_p[j],NULL,communicate,(void *)&(client_list[j]));
	            //为新的client创建新的线程 
	            client_nums++;
	        }
		} 
	}
	closesocket(socket_of_server); 
    WSACleanup();
    return 0;
}

void* communicate(void* arg)//服务器与某一矿机的通信函数 
{
    char revData[20];//这个地方一定要酌情设置大小，这决定了每次能获取多少数据
    int  ret;//recv函数的返回值 有三种状态每种状态的含义在下方有解释
    int i;
	BYTE nonce1str[8];
    client_list_node_t  node = (client_list_node_t)arg;
    char c=node->num+'0';
    send(node->socket_client,&c,1,0);
    //printf("\nnew miner, IP = %s PORT = %d \n", inet_ntoa(node->c_sin.sin_addr), node->c_sin.sin_port);
    puts("\n-------------------------------------");
	printf("miner%d connected, max miner number %d\n", client_nums,MAX_CLIENT_NUMS);
	puts("-------------------------------------\n");
    //while(!allconnected);//在没有都连接好前不开始 阻塞 
    while (1)
    {
        if(finish)
        {
			send(node->socket_client,"finish",6,0);
        	break;
        }
		if(!node->sendjob && newblockdone && !find)//该矿机未接到job,新的任务打包好，且没找到时 
        {
        	long long n;
        	node->sendjob=1; 
        	if(nonce1>node->num-1 && node->num!=1)
			//nonce1>node->num-1如矿工2接入时矿工1已经挖到nonce1=2 
			//1号矿工只能被分到nonce1=0 
        		n=nonce1;
        	else
        		n=node->num-1;
        	nonce1++;
			long2str(n,&str[40]);//根据该矿工编号num分配nonce1值，改在str中  
        	send(node->socket_client,str,blockBytes+2,0);//发送长度不能为strlen 因为遇到0会判为结尾 
        	node->getStopMsg=0;
        	printf("\nsend new job done for miner%d,nonce1=%lld\n",node->num,n); 
        }
        if(find && !node->getStopMsg)//全局变量find,可能是其他矿工找到
		{
            node->sendjob=0; 
            node->getStopMsg=1;//标志置1，不会再发送 
            if(node->num!=find)//
            {
            	send(node->socket_client,"find",4,0);
            	//printf("send \"find\" done to miner%d\n",node->num);
            }
			
		} 
        ret = recv(node->socket_client, revData, 20, 0);//第三个参数设置为revData的大小
        if (ret > 0)//接收长度 
        {
            if(!find && strstr(revData, "find")!=NULL)//目前没有挖到的条件下 收到本矿工挖到消息 
            {
				find=node->num;  //find记录哪个矿工挖到 编号从1开始 要立刻更新find,
            	n1=str2long(&revData[4]);
            	n2=str2long(&revData[12]);
            	newNonce=1;//nonce更新完毕信号 
            	printf("miner %d find nonce1=%lld, nonce2=%lld\n",find,n1,n2);
            }
            if(strstr(revData, "nextjob") !=NULL && !find)
            {
            	long long n=nonce1++;
				printf("	send next job to miner %d,nonce1=%lld\n",node->num,n);
            	long2str(n,nonce1str);
            	//for(i=0;i<8;i++)
            	//	printf("%x ",nonce1str[i]);
            	send(node->socket_client,nonce1str,8,0);
            }
        }
        else if(WSAGetLastError()==WSAEWOULDBLOCK)//接收缓冲区无数据 
			continue;
		else//ret <= 0
        {
            printf("an error occurs or disconnected IP = %s PORT = %d \n", inet_ntoa(node->c_sin.sin_addr), node->c_sin.sin_port);
            closesocket(node->socket_client);
            break;//断开和客户端的链接，然后跳出循环去等待新的连接。
        }
    }
    node->is_run = 0;
    client_nums--;
    printf("max miner number %d, now %d miners connected\n", MAX_CLIENT_NUMS, client_nums);
    return;
}


#include <winsock2.h>
#include <windows.h>  
#include <pthread.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"x64/pthreadVC2.lib")

#define MAX_CLIENT_NUMS 2
typedef struct client_list_node
{
    SOCKET  socket_client; //客户端的socket
    struct sockaddr_in c_sin; //用于存储已连接的客户端的socket基本信息
    int     is_run;        //标记这个节点的socket是否正在被使用
    HANDLE  h;             //为这个socket创建的线程 的句柄
    int num;  //编号 从1开始 
    int sendjob;//标记是否给矿机发送过job 0未发送 
    int getStopMsg;//标记是否给矿机发送过停止挖矿信号 
}client_list_node_st, *client_list_node_t;

void* connection(void);//连接client(矿机)线程函数 
void* communicate(void* arg);//server client通信线程函数 

#include <winsock2.h>
#include <windows.h>  
#include <pthread.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"x64/pthreadVC2.lib")

#define MAX_CLIENT_NUMS 2
typedef struct client_list_node
{
    SOCKET  socket_client; //�ͻ��˵�socket
    struct sockaddr_in c_sin; //���ڴ洢�����ӵĿͻ��˵�socket������Ϣ
    int     is_run;        //�������ڵ��socket�Ƿ����ڱ�ʹ��
    HANDLE  h;             //Ϊ���socket�������߳� �ľ��
    int num;  //��� ��1��ʼ 
    int sendjob;//����Ƿ��������͹�job 0δ���� 
    int getStopMsg;//����Ƿ��������͹�ֹͣ�ڿ��ź� 
}client_list_node_st, *client_list_node_t;

void* connection(void);//����client(���)�̺߳��� 
void* communicate(void* arg);//server clientͨ���̺߳��� 

//https://blog.csdn.net/S_O_L_O_R/article/details/79069741
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#include "communicate.h"
#include "server.h"
static client_list_node_st client_list[MAX_CLIENT_NUMS] = {0}; //�ͻ����б� communicate.h�ж��� 
static SOCKET socket_of_server;  //����ˣ����أ���socket
static struct sockaddr_in s_sin; //���ڴ洢���ش���socket�Ļ�����Ϣ
pthread_t miner_p[MAX_CLIENT_NUMS];//��������minerͨ���߳�ָ�� 

extern int newblock, newblockdone,find,finish,newNonce;//main�ж��� 
extern long long n1,n2,nonce1;
extern BYTE str[];
static int    client_nums = 0;
void* connection(void)
{
	int port = 6666;
    int i = 0;
    WORD socket_version = MAKEWORD(2, 2);
    WSADATA wsadata;
    unsigned long ul=1;//�������÷����� ���������ģʽ�����
    if (WSAStartup(socket_version, &wsadata) != 0)
    {
        return 0;
    }
    socket_of_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    //����socket ���ж��Ƿ񴴽��ɹ�
    if (socket_of_server == INVALID_SOCKET)
    {
        printf("socket error\n");
        return 0;
    }
    s_sin.sin_family = AF_INET;  //����Э����ΪIPV4
    s_sin.sin_port = htons(port);//�涨�˿ں�
    s_sin.sin_addr.S_un.S_addr = INADDR_ANY;
    /************************************************************************
    s_sin.sin_addr.S_un.S_addr = INADDR_ANY; �����趨���ڱ������ĸ�IP��ַ�ϣ�
    ���ĸ�IP��ַ�Ͻ��м������趨ΪINADDR_ANY����0.0.0.0����Ĭ��IP��
    �������˱��ʱ ����ط��޹ؽ�Ҫ����������Ӧ��ʱ����ط�������������
    ��Ϊ�ö�������Ƕ�������������ж��IP���ĸ����������ӷ������ھ������ľ�Ҫ
    ����Ϊ�ĸ���
    ************************************************************************/
    if (bind(socket_of_server, (LPSOCKADDR)&s_sin, sizeof(s_sin)) == SOCKET_ERROR)//��
    {
        printf("bind error\n");
    }
    if (listen(socket_of_server, 5) == SOCKET_ERROR)//����
    {
        printf("listen error\n");
        return 0;
    }
    printf("�����Ѿ����� PORT��%d�����ڵȴ�����... ...\n", port);
    while (!finish)//ȫ��finish 
    {
        SOCKET socket_of_client;  //�ͻ��ˣ�Զ�̣���socket
        struct sockaddr_in c_sin; //���ڴ洢�����ӵĿͻ��˵�socket������Ϣ
        int    c_sin_len;         //����accept�ĵ�����������c_sin�Ĵ�С��
        c_sin_len = sizeof(c_sin);
        socket_of_client = accept(socket_of_server, (SOCKADDR *)&c_sin, &c_sin_len);
        /************************************************************************
        û���µ������� ���򲻻�һֱ������ѭ������ʱaccept�ᴦ������״̬��
        ֱ�����µ����ӣ����߳����쳣��
        ************************************************************************/
        if (socket_of_client == INVALID_SOCKET)
        {
            //printf("accept error\n");
            continue; //�����ȴ���һ������
        }
        else
        {
        	if (client_nums + 1 > MAX_CLIENT_NUMS)
            {
                send(socket_of_client, "���ӳ����ƣ����ѱ��Ͽ� \n", strlen("���ӳ����ƣ����ѱ��Ͽ� \n"), 0);
                printf("���µĿ���������룬�������Ѿ��ﵽ��������������豸�Ѿ���ǿ�жϿ�\n");
                Sleep(1000);//?
                closesocket(socket_of_client);
                continue;
            }
            else
            {
            	if(SOCKET_ERROR==ioctlsocket(socket_of_client,FIONBIO,(unsigned long *)&ul))//���óɷ�����ģʽ����ѭ���շ� 
						puts("nonblock failed\n");
	        	int j=client_nums; 
	        	client_list[j].is_run = 1;
	        	client_list[j].num=j+1;//��Ŵ�1��ʼ 
	        	client_list[j].sendjob=0;//��ʼ��δ����job 
	            client_list[j].socket_client = socket_of_client;
	            client_list[j].c_sin;
	            memcpy(&(client_list[j].c_sin), &c_sin, sizeof(c_sin));
	            if (client_list[j].h)
	            {
	                CloseHandle(client_list[j].h);
	            }
	            pthread_create(&miner_p[j],NULL,communicate,(void *)&(client_list[j]));
	            //Ϊ�µ�client�����µ��߳� 
	            client_nums++;
	        }
		} 
	}
	closesocket(socket_of_server); 
    WSACleanup();
    return 0;
}

void* communicate(void* arg)//��������ĳһ�����ͨ�ź��� 
{
    char revData[20];//����ط�һ��Ҫ�������ô�С���������ÿ���ܻ�ȡ��������
    int  ret;//recv�����ķ���ֵ ������״̬ÿ��״̬�ĺ������·��н���
    int i;
	BYTE nonce1str[8];
    client_list_node_t  node = (client_list_node_t)arg;
    char c=node->num+'0';
    send(node->socket_client,&c,1,0);
    //printf("\nnew miner, IP = %s PORT = %d \n", inet_ntoa(node->c_sin.sin_addr), node->c_sin.sin_port);
    puts("\n-------------------------------------");
	printf("miner%d connected, max miner number %d\n", client_nums,MAX_CLIENT_NUMS);
	puts("-------------------------------------\n");
    //while(!allconnected);//��û�ж����Ӻ�ǰ����ʼ ���� 
    while (1)
    {
        if(finish)
        {
			send(node->socket_client,"finish",6,0);
        	break;
        }
		if(!node->sendjob && newblockdone && !find)//�ÿ��δ�ӵ�job,�µ��������ã���û�ҵ�ʱ 
        {
        	long long n;
        	node->sendjob=1; 
        	if(nonce1>node->num-1 && node->num!=1)
			//nonce1>node->num-1���2����ʱ��1�Ѿ��ڵ�nonce1=2 
			//1�ſ�ֻ�ܱ��ֵ�nonce1=0 
        		n=nonce1;
        	else
        		n=node->num-1;
        	nonce1++;
			long2str(n,&str[40]);//���ݸÿ󹤱��num����nonce1ֵ������str��  
        	send(node->socket_client,str,blockBytes+2,0);//���ͳ��Ȳ���Ϊstrlen ��Ϊ����0����Ϊ��β 
        	node->getStopMsg=0;
        	printf("\nsend new job done for miner%d,nonce1=%lld\n",node->num,n); 
        }
        if(find && !node->getStopMsg)//ȫ�ֱ���find,�������������ҵ�
		{
            node->sendjob=0; 
            node->getStopMsg=1;//��־��1�������ٷ��� 
            if(node->num!=find)//
            {
            	send(node->socket_client,"find",4,0);
            	//printf("send \"find\" done to miner%d\n",node->num);
            }
			
		} 
        ret = recv(node->socket_client, revData, 20, 0);//��������������ΪrevData�Ĵ�С
        if (ret > 0)//���ճ��� 
        {
            if(!find && strstr(revData, "find")!=NULL)//Ŀǰû���ڵ��������� �յ������ڵ���Ϣ 
            {
				find=node->num;  //find��¼�ĸ����ڵ� ��Ŵ�1��ʼ Ҫ���̸���find,
            	n1=str2long(&revData[4]);
            	n2=str2long(&revData[12]);
            	newNonce=1;//nonce��������ź� 
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
        else if(WSAGetLastError()==WSAEWOULDBLOCK)//���ջ����������� 
			continue;
		else//ret <= 0
        {
            printf("an error occurs or disconnected IP = %s PORT = %d \n", inet_ntoa(node->c_sin.sin_addr), node->c_sin.sin_port);
            closesocket(node->socket_client);
            break;//�Ͽ��Ϳͻ��˵����ӣ�Ȼ������ѭ��ȥ�ȴ��µ����ӡ�
        }
    }
    node->is_run = 0;
    client_nums--;
    printf("max miner number %d, now %d miners connected\n", MAX_CLIENT_NUMS, client_nums);
    return;
}


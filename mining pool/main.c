
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#include "sha256.h"
#include "server.h"
#include "communicate.h"

#define MAX_CLIENT_NUMS 2
#define N0 18  //hash开头有N0个2进制0 


pthread_t connect_p;
int newblock=1, newblockdone=0,find=0,finish=0,newNonce=0;
long long n1,n2,nonce1=0;//信号量 n1存最终找到的nonce1 
//变量nonce1为当前试到的  初始化-1 每连接一个矿机+1 
BYTE str[blockBytes+2];//后两位为N0 

void writefile(BLOCK *block,int num);//num为块序号 
int main(int argc, char* argv[])
{
	int i=0,j,N=10;//区块数N 
	char n[2];//存N0字符串 
	BLOCK *pre,*h;//前区块指针 区块链头指针
	BLOCK block[N];
	BYTE hash[32]={0}; //256 bits hash  0用于给第一个区块赋值 
	SHA256_CTX ctx;
	sha256_init(&ctx);
	
	pthread_create(&connect_p,NULL,connection,NULL);//创建连接线程 连完后为了保持通信 该线程不能断 动态连接 
	while(i<N)//次生成N个区块 
	{
		if(newblock)
		{
			puts("-------------------------");
			printf("creating %d th block:\n",i+1);
			Block_init(&block[i],hash);//创建初始区块 hash为上一个区块的 第一个区块hash=0 
			//print_block(&block[i]);
			struct2str((BYTE *)&block[i],str,blockBytes);
			//printf("len:%d\n",strlen(str));
			//itoa(N0,n,10);
			n[0]=N0/10+'0';//十位 注意用0占位 
			n[1]=N0%10+'0';
			str[blockBytes]=n[0];
			str[blockBytes+1]=n[1];//str最后两位为N0 
			//printf("%s\n",n);
			newblockdone=1;
			newblock=0;
		}
		if(find&&newNonce)//找到且全局变量nonce更新完毕 
		{
			newNonce=0;
			newblock=1;
			newblockdone=0;
			block[i].nonce1=n1;
			block[i].nonce2=n2;
			long2str(n1,&str[40]);//直接修改字符数组内的值
			long2str(n2,&str[48]);
			
			sha256_init(&ctx);
			sha256_update(&ctx, str, blockBytes);
			sha256_final(&ctx, hash);//更新hash 
			//printf("if verified %d",verify(&ctx,str,hash,N0));
			printf("\ninfo of %d th block:\n",i+1);
			print_block(&block[i]);
			printf("hash:");
			for(j=0;j<4;j++)
				printf("%x ",hash[j]);
			puts("\n");
			writefile(&block[i],i+1);
			nonce1=0;//为下一个区块初始化 
			if(i==0)//第一个区块 
			{
				h=&block[i];//记录链头地址 
				pre=&block[i];
			}
			else
			{
				pre->next=&block[i];//新区块加入链，上一个区块记录新区块地址 
				pre=&block[i];//用最新的区块地址更新pre
			}
			i++;
			find=0;//放在最后，确保给communicate线程中每个矿机发完find了 
		}
	}
    //malloc free
    finish=1;
    Sleep(100);//等待每个communicate线程给矿机发完结束消息
	pthread_cancel(connect_p);
    system("pause");
    return 0; 
}

void writefile(BLOCK *block,int num)//num为块序号 
{
	char s[]="blockchain_data/block";
	char n[4];
	itoa(num,n,10);
	strcat(s,n);
	FILE *fp=fopen(s,"wb");
	if(fp==NULL)
	{
	printf("can't open the file\n");
	exit(0);
	}
	fwrite(block,blockBytes,1,fp);//每个区块信息存储到二进制文件 block->next不存，无意义 	
	//blockBytes=sizeof(BLOCK)-8  block->next(4bytes) 由于struct对齐，占8 
	fclose(fp);
}


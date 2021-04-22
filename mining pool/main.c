
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <pthread.h>
#include "sha256.h"
#include "server.h"
#include "communicate.h"

#define MAX_CLIENT_NUMS 2
#define N0 18  //hash��ͷ��N0��2����0 


pthread_t connect_p;
int newblock=1, newblockdone=0,find=0,finish=0,newNonce=0;
long long n1,n2,nonce1=0;//�ź��� n1�������ҵ���nonce1 
//����nonce1Ϊ��ǰ�Ե���  ��ʼ��-1 ÿ����һ�����+1 
BYTE str[blockBytes+2];//����λΪN0 

void writefile(BLOCK *block,int num);//numΪ����� 
int main(int argc, char* argv[])
{
	int i=0,j,N=10;//������N 
	char n[2];//��N0�ַ��� 
	BLOCK *pre,*h;//ǰ����ָ�� ������ͷָ��
	BLOCK block[N];
	BYTE hash[32]={0}; //256 bits hash  0���ڸ���һ�����鸳ֵ 
	SHA256_CTX ctx;
	sha256_init(&ctx);
	
	pthread_create(&connect_p,NULL,connection,NULL);//���������߳� �����Ϊ�˱���ͨ�� ���̲߳��ܶ� ��̬���� 
	while(i<N)//������N������ 
	{
		if(newblock)
		{
			puts("-------------------------");
			printf("creating %d th block:\n",i+1);
			Block_init(&block[i],hash);//������ʼ���� hashΪ��һ������� ��һ������hash=0 
			//print_block(&block[i]);
			struct2str((BYTE *)&block[i],str,blockBytes);
			//printf("len:%d\n",strlen(str));
			//itoa(N0,n,10);
			n[0]=N0/10+'0';//ʮλ ע����0ռλ 
			n[1]=N0%10+'0';
			str[blockBytes]=n[0];
			str[blockBytes+1]=n[1];//str�����λΪN0 
			//printf("%s\n",n);
			newblockdone=1;
			newblock=0;
		}
		if(find&&newNonce)//�ҵ���ȫ�ֱ���nonce������� 
		{
			newNonce=0;
			newblock=1;
			newblockdone=0;
			block[i].nonce1=n1;
			block[i].nonce2=n2;
			long2str(n1,&str[40]);//ֱ���޸��ַ������ڵ�ֵ
			long2str(n2,&str[48]);
			
			sha256_init(&ctx);
			sha256_update(&ctx, str, blockBytes);
			sha256_final(&ctx, hash);//����hash 
			//printf("if verified %d",verify(&ctx,str,hash,N0));
			printf("\ninfo of %d th block:\n",i+1);
			print_block(&block[i]);
			printf("hash:");
			for(j=0;j<4;j++)
				printf("%x ",hash[j]);
			puts("\n");
			writefile(&block[i],i+1);
			nonce1=0;//Ϊ��һ�������ʼ�� 
			if(i==0)//��һ������ 
			{
				h=&block[i];//��¼��ͷ��ַ 
				pre=&block[i];
			}
			else
			{
				pre->next=&block[i];//���������������һ�������¼�������ַ 
				pre=&block[i];//�����µ������ַ����pre
			}
			i++;
			find=0;//�������ȷ����communicate�߳���ÿ���������find�� 
		}
	}
    //malloc free
    finish=1;
    Sleep(100);//�ȴ�ÿ��communicate�̸߳�������������Ϣ
	pthread_cancel(connect_p);
    system("pause");
    return 0; 
}

void writefile(BLOCK *block,int num)//numΪ����� 
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
	fwrite(block,blockBytes,1,fp);//ÿ��������Ϣ�洢���������ļ� block->next���棬������ 	
	//blockBytes=sizeof(BLOCK)-8  block->next(4bytes) ����struct���룬ռ8 
	fclose(fp);
}


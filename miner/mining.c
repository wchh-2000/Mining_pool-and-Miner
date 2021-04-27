#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "mining.h"
extern int find,nextjob;
long long str2long(BYTE *s)
{
	long long n=s[7];//���ݵĸ�λ�������ڴ�ĸߵ�ַ��
	int i;
	for(i=6;i>=0;i--)
		n=n*0x100+s[i];
	return n;
}
void long2str(long long n,BYTE *s)//long long 64bits �ֳ�8��BYTE 
{
	int i;
	long long a=n;
	for(i=0;i<8;i++)
		s[i]=*((BYTE *)&a+i);
}
void* Mining(void *arg)
//�Բ�ͬnonce2 ʹ��block��hash���㿪ͷ��n0��2����0 �ڵ���nonce1 nonce2
{
	SHA256_CTX ctx;
	Arg *a=(Arg*)arg;
	//arg=(Arg*)arg;
	a->isrun=1;
	BYTE *str=a->str;
	long long j;
	int v=0,i;
	BYTE hash[32]={0};
	printf("	nonce1=%lld\n",str2long(&str[40]));
	for(j=0;v==0 && j<MaxNum;j++)
	{
		pthread_testcancel();//�߳̽����� 
		long2str(j,&str[48]);//ֱ���޸��ַ�������nonce2��ֵ
		v=verify(&ctx,str,hash,a->n0);
		//if(j%500==0)
			//printf("%lld ",j); 
	} 
	if(v)//�ڵ� 
	{
		for(i=0;i<16;i++)
			a->result[i]=str[40+i];//nonce1 nonce2��16bytes  
		printf("find,nonce1=%lld,nonce2=%lld\n",str2long(&str[40]),j-1);
		find=1;
		printf("hash:");
		for(i=0;i<4;i++)//���ǰ4λ 
			printf("%x ",hash[i]);
		puts("");
	}
	else//�Ե����ֵ 
	{
		//puts("not find\n");
		nextjob=1;
	}
	a->isrun=0;
	return;
}


int verify(SHA256_CTX *ctx,BYTE *str,BYTE *hash,int n)
// �ַ�������str ʹ��hashǰn��2������Ϊ0 ���㷵��1 
{
	int i,m,r;
	sha256_init(ctx);
	sha256_update(ctx, str, blockBytes);//
	sha256_final(ctx, hash);
	m=n/8;//0ռBYTE���� 
	r=n%8;//���� 
	for(i=0;i<m;i++)
	{
		if(hash[i]!=0)
			return 0;
	}
	switch(r)//hashǰ��m��BYTE��Ϊ0ʱ �жϵ�m+1��BYTE�Ƿ�ǰ�溬r��0
	{
		case 0: return 1;
		case 1: if(hash[i]<=0x7f) return 1; break;
		case 2: if(hash[i]<=0x3f) return 1; break;
		case 3: if(hash[i]<=0x1f) return 1; break;
		case 4: if(hash[i]<=0x0f) return 1; break;
		case 5: if(hash[i]<=0x07) return 1; break;
		case 6: if(hash[i]<=0x03) return 1; break;
		case 7: if(hash[i]<=0x01) return 1; break;
	}
	return 0;
}



void print_block(BLOCK *p)
{
	int j,n;
	int N=5;
	puts("previous hash:");
	for(j=0;j<32;j++)
		printf("%x ",p->prev_hash[j]);
	printf("\nnonce1:%lld nonce2:%lld\n",p->nonce1,p->nonce2);//ע��lld������ȷ��ʾlong long 
	/*
	for(n=0;n<N;n++)
	{
		printf("\n%d th transation signature:",n+1);
		for(j=0;j<64;j++)
			printf("%x ",p->trans[n].signature[j]);
	}
	*/ 
}

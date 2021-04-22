#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "server.h"
typedef unsigned char BYTE;             // 8-bit byte
void struct2str(BYTE *p,BYTE *s,int structBytes)
//����ṹ��תΪ�ַ����� ���ڼ���hash 
//�������ʱ�ѽṹ��ָ��תΪ�ַ�ָ�� structBytesΪ�ṹ����ռ�ֽ���,��С��ʵ�ʽṹ��������������ת�� 
//ÿ�������ַ�����ᱻ���� 
{
	int i;
	for(i=0;i<structBytes;i++)
		s[i]=*((BYTE *)p+i);
	//s[i]="\0";//�Կ��ַ���β �����Ժ�õ��ַ����� 
}
void long2str(long long n,BYTE *s)//long long 64bits �ֳ�8��BYTE 
{
	int i;
	long long a=n;
	for(i=0;i<8;i++)
		s[i]=*((BYTE *)&a+i);
}
long long str2long(BYTE *s)
{
	long long n=s[7];//���ݵĸ�λ�������ڴ�ĸߵ�ַ��
	int i;
	for(i=6;i>=0;i--)
		n=n*0x100+s[i];
	return n;
}
void Block_init(BLOCK *p,BYTE *hash)
{
	int i,n;
	int N=5;//����N�����׼�¼ 
	BYTE str[transBytes];//���ڴ洢���׼�¼ �����ַ�������hash 
	SHA256_CTX ctx;
	sha256_init(&ctx);
	
	p->chain_version=101;
	for(i=0;i<32;i++)//��¼ǰһ��hash 
		p->prev_hash[i]=hash[i];
	p->nonce1=0;
	p->nonce2=0;
	for(i=0;i<32;i++)
		p->coinbase[i]=rand()%256;//0-255����� 
	p->award=100;
	
	for(n=0;n<N;n++)//����N�����׼�¼ 
	{
		BYTE privateKey_f[32], privateKey_t[32],publicKey_f[33],publicKey_t[33],trans_hash[32],signature[64];
		Transaction trans;//��ʱ�洢���׼�¼ 
		for(i=0;i<32;i++)//�������˽Կ 
		{
			privateKey_f[i]=rand()%256;
			privateKey_t[i]=rand()%256;
		}
		ecc_make_key(publicKey_f,privateKey_f);//ecc���ɹ�Կ 
		ecc_make_key(publicKey_t,privateKey_t);
		
		trans.amount=rand()%10000;//������ɽ��׽�� 
		for(i=0;i<33;i++)//��¼��Կ 
		{
			trans.from[i]=publicKey_f[i]; 
			trans.to[i]=publicKey_t[i]; 
		}
		//printf("sizeof trans:%d\n",sizeof(trans));
		struct2str((BYTE *)&trans,str,transBytes); //���׼�¼ת�ַ��� 
		
		sha256_update(&ctx, str, transBytes);
		sha256_final(&ctx, trans_hash);//���ɽ��׼�¼hash 
		/* 
		if(n==4)//����ʽ��� 
		{
			privateKey_f[3]+=1;//�޸�˽Կ ģ�¼ٵ�˽Կ ��֤����ǩ��Ӧ�ò�ͨ�� 
		}*/
		ecdsa_sign(privateKey_f,trans_hash,signature);//ʹ��from��˽Կ�Խ��׼�¼��������ǩ�� 
		if(ecdsa_verify(publicKey_f,trans_hash,signature))
		{
			printf("transaction %d verified\n",n+1);
			for(i=0;i<64;i++)//��¼����ǩ�� 
			{
				trans.signature[i]=signature[i];
			}
			p->trans[n]=trans;//���������� 
		}
		else
			printf("transaction %d verification failed\n",n+1);
		
	}
}

int verify(SHA256_CTX *ctx,BYTE *str,BYTE *hash,int n)
// �ַ�������str ʹ��hashǰn��2������Ϊ0 ���㷵��1 
{
	int i,m,r;
	sha256_init(ctx);
	sha256_update(ctx, str, blockBytes);
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
	for(j=0;j<4;j++)
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


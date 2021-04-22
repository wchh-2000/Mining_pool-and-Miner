#ifndef mining_h
#define mining_h
#include <pthread.h>
#include "sha256.h"
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"x64/pthreadVC2.lib")

#define blockBytes 816 //ÿ��������ռ�ֽ��� 96+transaction*n ����ָ��next���������hash  
#define transBytes 80 //ÿ�ʽ��׼�¼��ռ�ֽ��� ��������ǩ�� 
#define joblength blockBytes+2 //���������blockתΪstr���ֽ��� 
#define MaxNum 100000 //��nonce1,nonce2������� 
typedef unsigned char BYTE;             // 8-bit byte

typedef struct {
	BYTE from[33];//264bits ��Դ�߹�Կ 
	BYTE to[33];//264 ȥ���߹�Կ 
	double amount;//64 ת�Ʊ���
	//char time[16];//eg: 2021/03/30/16:05 //��������ע���transBytes
	BYTE signature[64];//512 ecc����ǩ��  
}Transaction; //1104 bits 138 ʵ��ռ144bytes(33+33+6Ϊ8�ı��� 64λ���ݿ��64bits 8 bytes) 
typedef struct BLOCK{
	BYTE prev_hash[32];//256
	long long chain_version;//64 bits
	long long nonce1;//64
	long long nonce2;//64
	double award;//64 �ڿ��� 
	BYTE coinbase[32];//256 �ڿ��߹�Կ
	Transaction trans[5];//n�ʽ��״��Ϊһ������  
	struct BLOCK *next;//(4bytes) ����struct���룬ռ8bytes  
} BLOCK;//824 bytes
typedef struct{
	BYTE str[blockBytes+1];
	int n0;
	char result[16];
	int isrun;
}Arg;//�����̺߳������ݲ��� 
void print_block(BLOCK *p);

void* Mining(void *arg);//�ڿ��̺߳��� 
#endif  

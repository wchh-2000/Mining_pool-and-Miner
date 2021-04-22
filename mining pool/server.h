#ifndef server_h
#define server_h

#include "sha256.h"
#include "ecc.h"

#define blockBytes 816 //ÿ��������ռ�ֽ��� 96+transaction*n ����ָ��next���������hash  
#define transBytes 80 //ÿ�ʽ��׼�¼��ռ�ֽ��� ��������ǩ�� 
//typedef unsigned char BYTE;             // 8-bit byte

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

void Block_init(BLOCK *p,BYTE *hash);
int verify(SHA256_CTX *ctx,BYTE *str,BYTE *hash,int n);
void print_block(BLOCK *p);
void struct2str(BYTE *p,BYTE *s,int structBytes);
//����ṹ��תΪ�ַ����� ���ڼ���hash 
//�������ʱ�ѽṹ��ָ��תΪ�ַ�ָ�� structBytesΪ�ṹ����ռ�ֽ���,��С��ʵ�ʽṹ��������������ת�� 
void long2str(long long n,BYTE *s);//long long 64bits �ֳ�8��BYTE 
long long str2long(BYTE *s);
#endif  

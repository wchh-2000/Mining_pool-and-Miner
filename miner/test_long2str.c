typedef unsigned char BYTE;
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
int main(void)
{
	long long n=0x87459e24f3,n2;
	BYTE s[9];
	int i;
	printf("long long:%llx\n",n);
	long2str(n,s);
	puts("str:");
	for(i=0;i<8;i++)
		printf("%x ",s[i]);//С���ֽ���
	n2=str2long(s);
	printf("\nresult long long:%llx\n",n2);
	return 0;
} 

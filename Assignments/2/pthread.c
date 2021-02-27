#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
int global=2;
int isPrime(unsigned long long a)
{
	unsigned long long b =sqrt(a)+1;
	for (int i = 2; i < b; ++i)
	{
		if (a%i==0)
			return 0;
	}
	return 1;
}
struct args {
    unsigned long long a;
    unsigned long long b;
};

void *checkPrime(void *arg)
{
			
	struct args *param = (struct args *) arg;
	unsigned long long a=param->a;
	unsigned long long b=param->b;
	// printf("%llu -a\n", a);
	// printf("%llu -b\n", b);
	for (unsigned long long i = a; i <b; ++i)
	{
		// if (isPrime(i)==1)
		// 	printf("%llu\n", i);
	}
}
unsigned long long min(unsigned long long a , unsigned long long b)
{
	if (a>b)
		return b;
	else
		return a;
}
int main(int argc, char const *argv[])
{
	char* s;
	if (argc!=3)
	{
		printf("Error! Please enter the values of N and t. Exiting\n");
		return -1;
	}
	unsigned long long int n=strtoull(argv[1],&s,10);
	// printf("%llu\n", n);
	unsigned int t=atoi(argv[2]);
	unsigned long long int v= (n/t)+1;//to handle the case when n is not multiple of t
	// printf("%llu\n", v);
	//FIRST APPROACH-------------------------------
	int start =2;
	pthread_t threadArr[t+1];
	struct args * argArr[t+1];
	int j=0;
	for (unsigned long long int i =v; ; i+=v)
	{
		struct args *arg=malloc(sizeof(struct args));
		arg->a=start;
		arg->b=min(i,n); 
		argArr[j++]=arg;
		// printf("%d start\n",start );
		start=i;
		if(i>n)
			break;
		
	}
	for (int i = 0; i < j; ++i)
		pthread_create(&threadArr[i], NULL,checkPrime,(void *)argArr[i] );
	for (int i = 0; i < j; ++i)
		pthread_join(threadArr[i], NULL);
	for (int i = 0; i < j; ++i)
		free(argArr[i]);


	//SECOND APPROACH----------------------
	return 0;

}
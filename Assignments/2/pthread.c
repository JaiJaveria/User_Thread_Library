#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/resource.h>
// #include <sys/stdint.h>

int counter=2;
pthread_mutex_t lock;
unsigned long long int n;

unsigned int cpu_time ( ) {
   struct rusage r;
   struct timeval t;
   getrusage(RUSAGE_SELF,&r);
   t = r.ru_utime;
   return (unsigned int) t.tv_sec*1000 + t.tv_usec/1000;//milli seconds
}

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
    bool *val;
    unsigned int *seq_time;
};
struct args_dynamic {
    bool *val;
    unsigned int *par_time;
};
void *checkPrime(void *arg)
{
			
	struct args *param = (struct args *) arg;
	unsigned long long a=param->a;
	unsigned long long b=param->b;
	// printf("%llu -a\n", a);
	// printf("%llu -b\n", b);
	unsigned int start= cpu_time();
	for (unsigned long long i = a; i <b; ++i)
	{
		// if (isPrime(i)==1)
		// 	printf("%llu\n", i);
		(param->val)[i]=isPrime(i);
	}
	*(param->seq_time)=cpu_time()-start;
}
void *checkPrime_dynamic(void *arg)
{
	struct args_dynamic *param = (struct args_dynamic *) arg;
	// bool *val = (bool *) arg;
	int i;
	bool stop=0;
	unsigned int start= cpu_time();
	while(true)
	{
		pthread_mutex_lock(&lock);
		if(counter<=n)
		{
			i=counter;
			counter++;
		}
		else
			{stop=1;}
		pthread_mutex_unlock(&lock);
		if (stop==1)
		{
			break;
		}
		(param->val)[i]=isPrime(i);
	}		
	*(param->par_time)=cpu_time()-start;
	
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
	n=strtoull(argv[1],&s,10);
	// printf("%llu\n", n);
	//contains all the answers
	bool *val=malloc(sizeof(bool)*(n+1));
	unsigned int t=atoi(argv[2]);
	unsigned long long int v= (n/t)+1;//to handle the case when n is not multiple of t
	// printf("%llu\n", v);
	//FIRST APPROACH-------------------------------
	int start =2;
	pthread_t threadArr[t];
	unsigned int seq_time[t];
	unsigned int par_time[t];
	struct args * argArr[t];
	int j=0;
	for (unsigned long long int i =v; ; i+=v)
	{
		struct args *arg=malloc(sizeof(struct args));
		arg->a=start;
		arg->b=min(i,n+1); 
		(arg->val)=val;
		argArr[j++]=arg;
		// printf("%d start\n",start );
		start=i;
		if(i>n)
			break;
	}
	printf("%d\n",j );
	for (int i = 0; i < j; ++i)
		pthread_create(&threadArr[i], NULL,checkPrime,(void *)argArr[i] );
	for (int i = 0; i < j; ++i)
		pthread_join(threadArr[i], NULL);
	for (int i = 0; i < j; ++i)
		free(argArr[i]);
	for (int i = 2; i <= n; ++i)
	{
		// printf("%d\n", i);
		if (val[i]==1)
			printf("%d\n", i);
		
	}
	printf("\n");
	printf("\n");
	//SECOND APPROACH----------------------
	if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init unsuccessful\n"); 
        return -1; 
    } 
	// struct args * argDy;

    for (int i = 0; i < t; ++i)
		pthread_create(&threadArr[i], NULL,checkPrime_dynamic,(void*)val );
	for (int i = 0; i < t; ++i)
		pthread_join(threadArr[i], NULL);
	for (int i = 2; i <= n; ++i)
	{
		if (val[i]==1)
			printf("%d\n", i);
	}
	free(val);
	return 0;


}
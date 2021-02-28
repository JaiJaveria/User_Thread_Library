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

// unsigned int cpu_time ( ) {
//    struct rusage r;
//    struct timeval t;
//    getrusage(RUSAGE_SELF,&r);
//    t = r.ru_utime;
//    return (unsigned int) t.tv_sec*1000 + t.tv_usec/1000;//milli seconds
// }

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
    double *seq_time;
};
struct args_dynamic {
    bool *val;
    double *par_time;
};
void *checkPrime(void *arg)
{
			
	struct args *param = (struct args *) arg;
	unsigned long long a=param->a;
	unsigned long long b=param->b;
	// printf("%llu -a\n", a);
	// printf("%llu -b\n", b);
	double start= clock();
	// printf("%f\n",start );
	for (unsigned long long i = a; i <b; ++i)
	{
		// if (isPrime(i)==1)
		// 	printf("%llu\n", i);
		(param->val)[i]=isPrime(i);
	}
	*(param->seq_time)=((double)clock()-start)/CLOCKS_PER_SEC;
}
void *checkPrime_dynamic(void *arg)
{
	struct args_dynamic *param = (struct args_dynamic *) arg;
	// bool *val = (bool *) arg;
	int i;
	bool stop=0;
	double start= clock();
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
	*(param->par_time)=((double)clock()-start)/CLOCKS_PER_SEC;
	// *(param->par_time)=cpu_time()-start;
	
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
	FILE * fp;
	//FIRST APPROACH-------------------------------
	int start =2;
	pthread_t threadArr[t];
	double *seq_time=malloc(sizeof(double)*t);
	double *par_time=malloc(sizeof(double)*t);
	struct args * argArr[t];
	int j=0;
	for (unsigned long long int i =v; ; i+=v)
	{
		struct args *arg=malloc(sizeof(struct args));
		arg->a=start;
		arg->b=min(i,n+1); 
		(arg->val)=val;
		arg->seq_time=&seq_time[j];
		argArr[j++]=arg;
		// printf("%d start\n",start );
		start=i;
		if(i>n)
			break;
	}
	// printf("%d\n",j );
	fp = fopen ("output/staticSchedule.txt", "w");
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
    		fprintf(fp, "%d\n", i);
			// printf("%d\n", i);
		
	}
	//SECOND APPROACH----------------------
	struct args_dynamic * argArrDy[t];
	if (pthread_mutex_init(&lock, NULL) != 0) { 
        printf("\n mutex init unsuccessful\n"); 
        return -1; 
    } 
    for (int i = 0; i < t; ++i)
    {
    	struct args_dynamic *arg=malloc(sizeof(struct args_dynamic));
		arg->par_time=&par_time[i]; 
		(arg->val)=val;
		argArrDy[i]=arg;
    }
    for (int i = 0; i < t; ++i)
		pthread_create(&threadArr[i], NULL,checkPrime_dynamic,(void*)argArrDy[i] );
	for (int i = 0; i < t; ++i)
		pthread_join(threadArr[i], NULL);
	for (int i = 0; i < t; ++i)
		free(argArrDy[i]);
	fclose(fp);
	 fp = fopen ("output/dynamicSchedule.txt", "w");
	for (int i = 2; i <= n; ++i)
	{ 
		if (val[i]==1)
    		fprintf(fp, "%d\n", i);
			// printf("%d\n", i);
	}
	free(val);
	fclose(fp);
	fp = fopen ("output/time.txt", "w");
    fprintf(fp," Thread_No Static_Time Dynamic_Time\n" );
	for (int i = 0; i < t; ++i)
    {
    // fprintf(gnuplotPipe, "%d %f %f\n",i,seq_time[i],par_time[i] );
    fprintf(fp," %d %f %f\n",i,seq_time[i],par_time[i] );
    	/* code */
    }
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    fprintf(gnuplotPipe, "set term pdf\n");
    fprintf(gnuplotPipe, " set output \"%s\" \n", "plots/graph.pdf");
    fprintf(gnuplotPipe, " set style data histogram \n");
    fprintf(gnuplotPipe, " set key autotitle columnhead \n");
    fprintf(gnuplotPipe, " set style fill solid border -1\n");
    fprintf(gnuplotPipe, " set style histogram cluster gap 1\n");
    fprintf(gnuplotPipe, " set boxwidth 0.7\n");
    fprintf(gnuplotPipe, " set xlabel 'Thread Number'\n");
    fprintf(gnuplotPipe, " set ylabel 'Execution Time (in secs)'\n");
    // fprintf(gnuplotPipe, " plot '-' smooth freq with boxes\n");//,  '' using 2 title 'Static Version',  '' using 3 title 'Dynamic Version'\n" );
    fprintf(gnuplotPipe, " plot 'output/time.txt' using 2:xtic(1) title 'Static'  ,'output/time.txt' using 3 title 'Dynamic'\n");//,  '' using 2 title 'Static Version',  '' using 3 title 'Dynamic Version'\n" );
    // fprintf(gnuplotPipe, " plot 'time.txt'  using 1:2 with boxes ls 1, 'time.txt' using 1:2 with boxes ls 2\n" );
    // fprintf(gnuplotPipe, " Thread_No Static_Version Dynamic_Version\n" );
    
    // fprintf(gnuplotPipe, " 0 addS %f\n",addtime );
    // fprintf(gnuplotPipe, " 1 addMy %f\n",addtimem );
    // fprintf(gnuplotPipe, " 2 subS %f\n", subtime );
    // fprintf(gnuplotPipe, " 3 subMy  %f\n",subtimem );
    // fprintf(gnuplotPipe, " 4 prodS %f\n", prodtime);
    // fprintf(gnuplotPipe, " 5 prodMy %f\n ",prodtimem );
    // fprintf(gnuplotPipe, " 6 quotS %f\n", quottime );
    // fprintf(gnuplotPipe, " 7 quotMy %f\n", quottimem );
    // fprintf(gnuplotPipe, "e" );
	free(seq_time);
	free(par_time);
	fclose(fp);
	return 0;


}
#include "../header/myOwnthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct matmulArg
{
	int a_x;
	int a_y;
	int b_y;
	int *c;
	int *a;
	int *b;
	int j;
	int i;

} matmulArg;
void matmulElem(void *args)
{
	matmulArg *arg=args;
	int ans=0;
	int i=arg->i;
	int j=arg->j;
	printf("100--arg a_y %d\n", arg->a_y);
	printf("b matrix\n");
	for (int i = 0; i < arg->a_y; ++i)
	{
		printf("i %d ",i );
		for (int j = 0; j < arg->b_y; ++j)
		{
			printf("%d ",(arg->b)[i*(arg->b_y)+j] );
		}
		printf("\n");
	}
	for (int k = 0; k < arg->a_y; ++k)
	{
		printf("i %d j %d k %d\n",i,j,k );
		printf("a[i][k] %d\n", (arg->a)[i*(arg->a_y)+k]);
		printf("b[k][j] %d\n",(arg->b)[k*(arg->a_y)+j]);
		ans+=(arg->a)[i*(arg->a_y)+k]*((arg->b)[k*(arg->a_y)+j]);
		printf("101--ans %d\n",ans );

	}
	*(arg->c)=ans;
	printf("103--ans %d\n",ans );
	// free(arg);
}


void matmul(int a_x, int a_y, int b_y, int **c,int *a, int *b)
{

	mythread_t arr[a_x][b_y];//=malloc(a_x*b_y*sizeof(mythread_t*));
	printf("a_x %d\n", a_x);
	printf("a_y %d\n", a_y);
	printf("b_y %d\n", b_y);
	printf("b matrix\n");
	for (int i = 0; i < a_y; ++i)
	{
		printf("i %d ",i );
		for (int j = 0; j < b_y; ++j)
		{
			printf("%d ",b[i*b_y+j] );
		}
		printf("\n");
	}
	for (int i = 0; i < a_x; ++i)
	{
		for (int j = 0; j < b_y; ++j)
		{
			matmulArg *arg=malloc(sizeof(matmulArg));
			arg->a=a;
			arg->b=b;
			arg->c=&((*c)[i*b_y+j]);//c[i][j]
			arg->j=j;
			arg->i=i;
			arg->a_y=a_y;
			printf("153--b matrix\n");
			for (int i = 0; i < a_y; ++i)
			{
				printf("i %d ",i );
				for (int j = 0; j < b_y; ++j)
				{
					printf("%d ",arg->b[i*b_y+j] );
				}
				printf("\n");
			}
			// myThread_create(&arr[i][j],NULL,&matmulElem, arg);
			matmulElem(arg);
		}
	}
	// for (int i = 0; i < a_x; ++i)
	// {
	// 	for (int j = 0; j < b_y; ++j)
	// 	{
	// 		myThread_join(arr[i][j]);
	// 	}
	// }
}
void myFunc(void *a)
{
	// static int i=0;
	for (int i = 0; i < 1000; ++i)
	{
		sleep(0.9);
		// sleep(1);
		if (i==500)
		{
			myThread_yield();
		}
		printf("%d ", i);
		/* code */
	}
	printf("f finished.\n");
}
void myFuncMain()
{
	// static int i=0;
	for (int i = 0; i < 1000; ++i)
	{
		sleep(0.9);
		// sleep(1);
		printf("M%d ", i);
		/* code */
	}
	printf("f finished.\n");
}
void myFuncSync(mythread_mutex_t *p)
{
	static int a=1;
	mythread_mutex_lock(p);
	printf("103--%d started\n",a );
	sleep(1);
	printf("105--%d ended\n",a++ );
	mythread_mutex_unlock(p);
}
int main(int argc, char const *argv[])
{
	void *a;
	if (argc!=2)
	{
		printf("specify the count of threads. exiting \n");
		exit(-1);
	}
	int count=atoi(argv[1]);
	mythread_t t[count];
	mythread_mutex_t p;
	mythread_mutex_init(&p, NULL);

	for (int i = 0; i < count; ++i)
	{
		// myThread_create(&(t[i]),NULL,(&myFunc),a);
		myThread_create(&(t[i]),NULL,(&myFuncSync),&p);
	}
	// printf("443--Thread creation done. in main again.\n");
	// i=myThread_create(&t2,NULL,(&f),a);
	// printf("196--%u\n", ((t.tb))->status);
	// myThread_join(t);
	// myFuncMain();
	for (int i = 0; i < count; ++i)
	{
		myThread_join(t[i]);
		/* code */
	}

	// int *a,*
	return 0;
}

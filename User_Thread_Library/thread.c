#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

typedef struct tcb
{
	enum {
        CREATED,
        RUNNING,
        READY,
        EXITED,
    } status;
	int read;//exist status read or not 
	int id;
	// jmp_buf startenv;
	jmp_buf currentenv;
	// jmp_buf SVenv;
	unsigned long *stack_orig;
	// unsigned long *stack_ptr;
	int stack_size;
	void (*func) (void *a);
	void *arg;
}tcb ;
// typedef struct scheduler_val
// {
// 	jmp_buf env;
// 	jmp_buf handlerEnv;
// }scheduler_val;
typedef struct mythread_t
{
	tcb* tb;
} mythread_t;
typedef struct mythread_mutex_t
{
	int id;//id of thread who acquired it.
	//-1 if none present.
} mythread_mutex_t;
typedef tcb * TCB_PTR;
typedef struct cirQueue
{
	//circular queue
	TCB_PTR *q_orig;
	int queueSize;
	int queueLen;
	int head;
	int tail;
} cirQueue;

#define STACK_SIZE 16*1024
#ifndef JB_SP
#define JB_SP 6
#endif
#ifndef JB_PC
#define JB_PC 7
#endif
cirQueue *queue;//=malloc()
int thread_lib_init=0;
// scheduler_val *sv;
tcb *currentThread=NULL;
int insideMTC=0;//mythreadcreate
int insideSchduler=0;//mythreadcreate

void myThread_schedule();
void alarmHandlr (int a);
void thread_start_wrapper(TCB_PTR t);
void myThread_yield(void);
void mythread_mutex_lock(mythread_mutex_t *m);
void mythread_mutex_unlock(mythread_mutex_t *m);
int myThread_create(mythread_t *th,void *attr, void *start_routine, void *args);
void myThread_join(mythread_t mtt);

typedef struct matmulArg
{
	int a_x;
	int a_y;
	int b_y;
	int* c;
	int *a;
	int *b;
	int j;
	// int*** c;
	// int **a;
	// int **b;
	// int *i;
	// int *j;
	// int a_y;
	// int
} matmulArg;

void matmulElem(void *a)
{
	matmulArg *arg=a;
	int ans=0;
	int j=arg->j;
	for (int k = 0; k < arg->a_y; ++k)
	{
		ans+=(arg->a)[k]*((arg->b)[k*(arg->a_y)+j]);
	}
	*(arg->c)=ans;
	free(arg);
}

void matmul(int a_x, int a_y, int b_y, int **c,int *a, int *b)
{

	mythread_t arr[a_x][b_y];//=malloc(a_x*b_y*sizeof(mythread_t*));
	for (int i = 0; i < a_x; ++i)
	{
		for (int j = 0; j < b_y; ++j)
		{
			matmulArg *arg=malloc(sizeof(matmulArg));
			arg->a=&(a[i*a_y]);
			arg->b=(b);
			arg->c=&((*c)[i*b_y+j]);//c[i][j]
			arg->j=j;
			arg->a_y=a_y;
			myThread_create(&arr[i][j],NULL,&matmulElem, arg);
			// matmulElem(arg);
		}
	}
	for (int i = 0; i < a_x; ++i)
	{
		for (int j = 0; j < b_y; ++j)
		{
			myThread_join(arr[i][j]);
		}
	}
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
	static int a;
	mythread_mutex_lock(p);
	printf("103--%d started\n",a );
	sleep(5);
	printf("105--%d ended\n",a++ );
	mythread_mutex_unlock(p);
}
void cirQueueInit(cirQueue **q)
{
	cirQueue *queue;
	queue=malloc(sizeof(cirQueue));
	queue->queueSize=100;
	queue->queueLen=0;
	queue->q_orig=malloc(sizeof(TCB_PTR)*(queue->queueSize));
	queue->head=-1;
	queue->tail=-1;
	*q=queue;
}
void cirEnqueue(cirQueue **queue, TCB_PTR t)
{
	cirQueue *q=*queue;
	if (q->queueLen < q->queueSize)
	{
		if (q->head==-1)
		{
			q->head=0;
			q->tail=0;
			
		}
		else if (q->tail==q->queueSize-1)
		{
			q->tail=0;
		}
		else
			{q->tail++;}
		q->queueLen++;
		// printf("q tail %d\n",q->tail );
		q->q_orig[q->tail]=t;
		*queue=q;
	} 
	else
	{
		printf("Queue overflow not handled. exiting\n");
		exit(-1);
	}
}
void cirDequeue(cirQueue **queue)
{
	cirQueue *q=*queue;
	if (q->queueLen!=0)
	{
		if (q->head==q->queueSize-1)
		{
			q->head=0;
		}
		else
		{
			q->head++;
		}
		q->queueLen--;
	}
	*queue=q;
}
// static int growsLower (void *fromaddr)
// {
// 	int toaddr;
// 	return fromaddr > (void *) &toaddr;
// }
static long int manglex64(long int p) {
    long int ret;
    asm(" mov %1, %%rax;\n"
        " xor %%fs:0x30, %%rax;"
        " rol $0x11, %%rax;"
        " mov %%rax, %0;"
        : "=r"(ret)
        : "r"(p)
        : "%rax");
    return ret;
}
void wrapper() 
{
    // printf("function started\n");
    // printf("currentThread id %d\n", currentThread->id);
    currentThread->func(currentThread->arg);
    //put exit thread here
    // myThread_exit();
	currentThread->status=EXITED;
	printf("176--exited thread %d\n", currentThread->id);
	// printf("145--%u\n",t->status);
    // ualarm(50000,50000);
	myThread_schedule();
}
void libInit()
{
	// printf("libInit Called\n"); 
	cirQueueInit(&queue);
	// sv=malloc(sizeof(scheduler_val));
	// myThread_schedule();//set the sv->env variable
	struct sigaction *s=malloc(sizeof(struct sigaction));
	s->sa_handler=&alarmHandlr;
	s->sa_flags=SA_NODEFER;
    sigaction(SIGALRM,s,NULL);
    free(s);
	thread_lib_init=1;//initialization done
}

int mythread_mutex_init(mythread_mutex_t* m, void* a)
{
	m->id=-1;
	return 0;
}
void mythread_mutex_lock(mythread_mutex_t *m)
{
	while(1)
	{
		if (m->id!=-1)
		{
			printf("Wiating for lock to be freed. Thread t %d\n", currentThread->id);
			myThread_yield();
		}
		else
		{
			m->id=currentThread->id;
			break;
		}
	}
}
void mythread_mutex_unlock(mythread_mutex_t *m)
{
	m->id=-1;
}
void alarmHandlr (int a)
{
	ualarm(0,0);
	if (a==SIGALRM)
	{
		printf("120--SIGALRM signal recieved\n");
		if (currentThread==NULL)
		{
			printf("current thread null. assertion error\n");
			exit(-1);
			// if (setjmp(sv->handlerEnv)==0)
			// {
			// 	longjmp(sv->env,1);
			// 	/* code */
			// }
			// myThread_schedule();
		}
		else
		{
			if (setjmp(currentThread->currentenv)==0)
			{
				printf("Signal recieved in execution of thread %d. Saving context.\n", currentThread->id );
				// printf("140--currentThread current enviornment %d\n", currentThread->currentenv);
				// printf("%d\n", currentThread->currentenv);
				currentThread->status=READY;
            	// printf("233-stk ptr %lu\n",currentThread->currentenv[0].__jmpbuf[JB_SP] );
            	myThread_schedule();
				
			}
			else
			{
				printf("147--Called Again thread %d\n", currentThread->id);
            	// printf("240-stk ptr %lu\n",currentThread->currentenv[0].__jmpbuf[JB_SP] );
				ualarm(50000,50000);
				return;
			}
		}
		
	}
	else
	{
		printf("some other signal: %d. Not handled. Exiting\n", a );
		exit(-1);
	}
}

int myThread_create(mythread_t *th,void *attr, void *start_routine, void *args)
{
	ualarm(0,0);
	insideMTC=1;
	
	static int id=1;
	
	int index;//=getQueueLoc();

	tcb *t=malloc(sizeof(tcb));
	t->read=0;
	t->id=id++;
	t->status=CREATED;
	// printf("%u\n",t->status );
	t->stack_size=STACK_SIZE;
	t->stack_orig=malloc(STACK_SIZE);
	// if (growsLower(&index))
	// 	t->stack_ptr=t->stack_orig+t->stack_size;
	// else
	// 	t->stack_ptr=t->stack_orig;
	t->func=start_routine;
	t->arg=args;
	th->tb=t;

	if (thread_lib_init==0)
	{
		libInit();
		tcb *t=malloc(sizeof(tcb));
		t->read=0;
		t->id=0;//MAIN THREAD
		t->status=RUNNING;
		currentThread=t;
	

	}
	cirEnqueue(&queue,t);
	// printf("201--%d\n", queue->head);
	// TCB_PTR a= (queue->q_orig)[queue->head];
	// printf("215--t %d\n", a);
	// printf("215--t status %d\n", a->status);
	printf("214--Thread created\n");
	// insideMTC=0;
    ualarm(50000,50000);
	return t->id;
}
void myThread_yield(void)
{
	printf("Inside yield function\n");
	if (setjmp(currentThread->currentenv)==0)
	{
		currentThread->status=READY;
    	myThread_schedule();
		
	}
}
void thread_start_wrapper(TCB_PTR t)
{
    // printf("236--Inside thread start wrapper. t id %d\n", t->id);
    t->status=RUNNING;
    if(setjmp(t->currentenv) == 0)
        {
        	// printf("330--t id %d\n", t->id);
            // t->currentenv[0].__jmpbuf[JB_SP] = manglex64((unsigned long)(t->stack_ptr));
            t->currentenv[0].__jmpbuf[JB_SP] = manglex64((unsigned long)(t->stack_orig + (STACK_SIZE-8) / 8 - 2));
            t->currentenv[0].__jmpbuf[JB_PC] = manglex64((unsigned long) wrapper);
            // wrapper();
            // printf("437--wrapper called\n");
			ualarm(50000,50000);
            longjmp(t->currentenv,1);
        }
 
}
void myThread_schedule()
{

	ualarm(0,0);

		if (queue->queueLen==0)
		//if there is not other thread just return to the existing thread only or anywhere where it was called from 
		{
			printf("283--Nothing wating in the ready queue. Returning\n");
			if (currentThread!=NULL)
			{
				currentThread->status=RUNNING;
			}
			ualarm(50000,50000);
			longjmp(currentThread->currentenv,1);
		}
		else
		{
			if (currentThread==NULL)
			{
				printf("393--Current Thread null. assertion error\n"); 
				exit(-1);
			}
			else
			{
				TCB_PTR t=queue->q_orig[queue->head];
				// printf("436--t current thread STATUS %d\n", currentThread->status);
				// TCB_PTR t=queue[0];
				if (currentThread->status!=EXITED)
				{
					currentThread->status=READY;
					cirEnqueue(&queue,currentThread);
					/* code */
				}
				currentThread=t;
				cirDequeue(&queue);
				// thread_start_wrapper(t);
				if (currentThread->status==READY)
				{
					t->status==RUNNING;
					ualarm(50000,50000);
					longjmp(currentThread->currentenv,1);
					/* code */
				}
				else if (currentThread->status==CREATED)
				{
					// if (setjmp(currentThread->SVenv)==0)
					// {
						thread_start_wrapper(currentThread);
					// }
				}
				else
				{
					printf("status of thread not at queeue is %d, which is not RUNNING or CREATED. asseryion error. exiting\n", currentThread->status );
					exit(-1);
				}
				
				
			}
			
		}
}
void myThread_join(mythread_t mtt)
{
	printf("428--Inside Join\n");
	while(1)
	{
		if (((mtt.tb))->status==EXITED)
		{
			break;
		}
		else
		{
			myThread_yield();
		}
	}
	free(((mtt.tb))->stack_orig);
	free((mtt.tb));
}
int main(int argc, char const *argv[])
{
	// void *a;
	// int count=atoi(argv[1]);
	// mythread_t t[count];
	// // mythread_t t2;
	// mythread_mutex_t p;
	// mythread_mutex_init(&p, NULL);

	// for (int i = 0; i < count; ++i)
	// {
	// 	// myThread_create(&(t[i]),NULL,(&myFunc),a);
	// 	myThread_create(&(t[i]),NULL,(&myFuncSync),&p);
	// }
	// // printf("443--Thread creation done. in main again.\n");
	// // i=myThread_create(&t2,NULL,(&f),a);
	// // printf("196--%u\n", ((t.tb))->status);
	// // myThread_join(t);
	// // myFuncMain();
	// for (int i = 0; i < count; ++i)
	// {
	// 	myThread_join(t[i]);
	// 	/* code */
	// }
	// int a[2][2];
	// int b[2][2];
	// int c[2][2];
	int *a,*b,*c;
	int a_x=5,a_y=5,b_y=2;
	a=malloc(sizeof(int)*a_x*a_y);
	b=malloc(sizeof(int)*b_y*a_y);
	c=malloc(sizeof(int)*a_x*b_y);
	for (int i = 0; i < a_x; ++i)
	{
		for (int j = 0; j < a_y; ++j)
		{
			// a[i][j]=1;
			a[i*a_y+j]=1;
			// b[i][j]=1;
		}
	}
	for (int i = 0; i < a_y; ++i)
	{
		for (int j = 0; j < b_y; ++j)
		{
			// a[i][j]=1;
			b[i*b_y+j]=1;
			// b[i][j]=1;
		}
	}
	printf("matmul callinig\n");
	matmul(a_x,a_y,b_y,&c,a,b);
	for (int i = 0; i < a_x; ++i)
	{
		printf(" c i %d ",i );
		for (int j = 0; j < b_y; ++j)
		{
			printf("%d ",c[i*b_y+j] );
		}
		printf("\n");
	}
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
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
	jmp_buf startenv;
	jmp_buf currentenv;
	void *stack_orig;
	void *stack_ptr;
	int stack_size;
	void (*func) (void *a);
	void *arg;
}tcb ;
typedef struct scheduler_val
{
	jmp_buf env;
}scheduler_val;
typedef struct mythread_t
{
	tcb* tb;
} mythread_t;
typedef tcb * TCB_PTR;
typedef struct cirQueue
{
	//circular queue
	TCB_PTR *q_orig;
	int queueSize;
	int queueLen;
	TCB_PTR *head;
	TCB_PTR *tail;
} cirQueue;
#define STACK_SIZE 8192
// TCB_PTR *queue;
// int queueSize=5;
// int queueLen=0;
cirQueue *queue;//=malloc()
int thread_lib_init=0;
scheduler_val *sv;
tcb *currentThread=NULL;
int insideMTC=0;//mythreadcreate
int insideSchduler=0;//mythreadcreate

void myThread_schedule();

void cirQueueInit(cirQueue **q)
{
	cirQueue *queue= *q;
	queue=malloc(sizeof(cirQueue));
	queue->queueSize=5;
	queue->queueLen=0;
	queue->q_orig=malloc(sizeof(TCB_PTR)*(queue->queueSize));
	queue->head=queue->q_orig;
	queue->tail=queue->q_orig;
}
void alarmHandlr (int a)
{
	if (a==SIGALRM)
	{
		printf("SIGALRM signal recieved\n");
		if (insideMTC)
		{
			printf("Thread creation is progress. Resuming\n");
			return;//continue what you were doing.
		}
		if (insideSchduler)
		{
			printf("Thread scheduler already called. Resuming that\n");
			return;
		}
		printf("thread creation not happening nor a scheduler already working. calling the scheduler\n"); 
		if (currentThread==NULL)
		{
			myThread_schedule();
		}
		else
		{
			if (setjmp(currentThread->currentenv)==0)
			{
				currentThread->status=READY;
				myThread_schedule();
			}
			// else
			// {
			// 	currentThread->status=RUNNING;

			// }
		}
		
	}
	else
	{
		printf("some other signal: %d. Not handled. Exiting\n", a );
		exit(-1);
	}
}
void libInit()
{
	cirQueueInit(&queue);
	
	sv=malloc(sizeof(scheduler_val));
	struct sigaction *s=malloc(sizeof(struct sigaction));
	s->sa_handler=&alarmHandlr;
	// s->sa_flags=SA_NODEFER;
    sigaction(SIGALRM,s,NULL);
    ualarm(50000,50000);
    // free(s);
	thread_lib_init=1;//initialization done
	exit(-1);
}
// int getQueueLoc(cirQueue)
// {
// 	if (queueLen<queueSize)
// 		return queueLen++;
// 	else
// 	{
// 		printf("getQueueLoc not handled. thread queue overflow. Exitinng\n");
// 		exit(-1);
// 		// return 0;//to be handled later
// 	}
// }
// void incQueueLen()
// {
// 	queueLen++;
// }
void f(void *a)
{
	int i=0;

	printf("Hello World %d\n", i);
}
static int growsLower (void *fromaddr)
{
	int toaddr;
	return fromaddr > (void *) &toaddr;
}
int myThread_create(mythread_t *th,void *attr, void *start_routine, void *args)
{
	insideMTC=1;
	if (thread_lib_init==0)
	{
		libInit();
	}
	static int id=1;
	
	int index;//=getQueueLoc();

	tcb *t=malloc(sizeof(tcb));
	t->read=0;
	t->id=id++;
	t->status=CREATED;
	// printf("%u\n",t->status );
	t->stack_size=STACK_SIZE;
	t->stack_orig=malloc(STACK_SIZE);
	if (growsLower(&index))
		t->stack_ptr=t->stack_orig+t->stack_size;
	else
		t->stack_ptr=t->stack_orig;
	t->func=start_routine;
	t->arg=args;
	th->tb=t;
	
	index=getQueueLoc();
	queue[index]=t;
	// incQueueLen();
	printf("151--Thread created\n");
	insideMTC=0;
	return t->id;

}
void thread_exit(TCB_PTR t)
{
	t->status=EXITED;
	printf("exited\n");
	printf("145--%u\n",t->status);
	longjmp(sv->env,1);
}
void thread_start_wrapper(TCB_PTR t)
{
	// insideSchduler=0;//assumption only called at the end of my thread scheduler
	register void *top = t->stack_ptr;
    asm volatile(
        "mov %[rs], %%rsp \n"
        : [ rs ] "+r" (top) ::
    );
  //   if (t->status==READY)
  //   {
  //   	t->status=RUNNING;
		// insideSchduler=0;
		// longjmp(t->currentenv,1);
  //   }
  //   else
    {
    	if (setjmp(t->startenv)==0)
		{
			insideSchduler=0;
    		t->status=RUNNING;
			(t->func)(t->arg);
			longjmp(t->startenv,1);
		}
    }
	
	// return;
	thread_exit(t);
	// printf("190--%d\n",t->status );
}
void myThread_schedule()
{
	insideSchduler=1;
	if (queueLen==0)
	//if there is not other thread just return to the existing thread only or anywhere where it was called from 
	{
		// free(queue);
		// free(sv);
		printf("Nothing wating in the ready queue. returning\n");
		if (currentThread)
		{
			currentThread->status=RUNNING;
			/* code */
		}
		insideSchduler=0;
		return;
	}
	else
	{
		if (currentThread==NULL)
		{
			/* code */
			TCB_PTR t=queue[0];
			currentThread=t;
			queueLen--;
			if (setjmp(sv->env)==0)
			{
				thread_start_wrapper(t);
			}
			else
			{
				printf("218--%d\n",t->status);
			}

		}
		else
		{
			TCB_PTR t=queue[0];
			currentThread=t;
			queueLen--;
			longjmp(t->currentenv,1);
			// if (setjmp(sv->env)==0)
			// {
			// 	thread_start_wrapper(t);
			// }
		}
		
	}
}
void myThread_join(mythread_t mtt)
{
	while(((mtt.tb))->status!=EXITED)
	{
		// printf("%u\n",((mtt.tb))->status );
		// exit(-1);
	}
	free(((mtt.tb))->stack_orig);
	free((mtt.tb));
}
int main(int argc, char const *argv[])
{
	void *a;
	mythread_t t;
	int i=myThread_create(&t,NULL,(&f),a);
	// printf("196--%u\n", ((t.tb))->status);
	myThread_join(t);
	return 0;
}
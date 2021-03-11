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
	jmp_buf SVenv;
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
	int head;
	int tail;
} cirQueue;
#define STACK_SIZE 16*1024
#define JB_SP 6
#define JB_PC 7
// TCB_PTR *queue;
// int queueSize=5;
// int queueLen=0;
cirQueue *queue;//=malloc()
int thread_lib_init=0;
// scheduler_val *sv;
tcb *currentThread=NULL;
int insideMTC=0;//mythreadcreate
int insideSchduler=0;//mythreadcreate

void myThread_schedule();

void cirQueueInit(cirQueue **q)
{
	cirQueue *queue;
	queue=malloc(sizeof(cirQueue));
	queue->queueSize=5;
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
		TCB_PTR a= ((*queue)->q_orig)[(*queue)->head];
		// printf("89--t status %d\n", t->status);
		// printf("89--t status %d\n", a->status);
		// queue=&q;
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
			q->head++;
		q->queueLen--;
	}
	*queue=q;
}
void alarmHandlr (int a)
{
	ualarm(0,0);
	if (a==SIGALRM)
	{
		// printf("SIGALRM signal recieved\n");
		// if (insideMTC)
		// {
		// 	printf("Thread creation is progress. Resuming\n");
		// 	return;//continue what you were doing.
		// }
		// if (insideSchduler)
		// {
		// 	printf("Thread scheduler already called. Resuming that\n");
		// 	return;
		// }
		// printf("thread creation not happening nor a scheduler already working. calling the scheduler\n"); 
		if (currentThread==NULL)
		{
			ualarm(50000,50000);
			myThread_schedule();
		}
		else
		{
			if (setjmp(currentThread->currentenv)==0)
			{
				// printf("%d\n", currentThread->currentenv);
				currentThread->status=READY;
				ualarm(50000,50000);
				myThread_schedule();
			}
			else
			{
				// printf("147--Called Again\n");
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
void libInit()
{
	// printf("libInit Called\n"); 
	cirQueueInit(&queue);
	// sv=malloc(sizeof(scheduler_val));
	struct sigaction *s=malloc(sizeof(struct sigaction));
	s->sa_handler=&alarmHandlr;
	s->sa_flags=SA_NODEFER;
    sigaction(SIGALRM,s,NULL);
    // free(s);
	thread_lib_init=1;//initialization done
}
void f(void *a)
{
	// static int i=0;
	for (int i = 0; i < 1000; ++i)
	{
		sleep(0.9);
		printf("%d ", i);
		/* code */
	}
	printf("f finished.\n");
}
static int growsLower (void *fromaddr)
{
	int toaddr;
	return fromaddr > (void *) &toaddr;
}
int myThread_create(mythread_t *th,void *attr, void *start_routine, void *args)
{
	ualarm(0,0);
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
	t->stack_orig=malloc(STACK_SIZE*8);
	if (growsLower(&index))
		t->stack_ptr=t->stack_orig+t->stack_size;
	else
		t->stack_ptr=t->stack_orig;
	t->func=start_routine;
	t->arg=args;
	th->tb=t;

	cirEnqueue(&queue,t);
	// printf("201--%d\n", queue->head);
	// TCB_PTR a= (queue->q_orig)[queue->head];
	// printf("215--t %d\n", a);
	// printf("215--t status %d\n", a->status);
	// printf("214--Thread created\n");
	insideMTC=0;
    ualarm(50000,50000);
	return t->id;

}
void thread_exit(TCB_PTR t)
{
	t->status=EXITED;
	// printf("exited\n");
	// printf("145--%u\n",t->status);
    ualarm(50000,50000);
	// longjmp(t->SVenv,1);
	myThread_schedule();
}
void thread_start_wrapper(TCB_PTR t)
{
	// insideSchduler=0;//assumption only called at the end of my thread scheduler
	register void *top = t->stack_ptr;
    asm volatile(
        "mov %[rs], %%rsp \n"
        : [ rs ] "+r" (top) ::
    );
    printf("wrapper called\n");
	ualarm(50000,50000);
    wrapper();
   //  if(setjmp(t->currentenv) == 0)
   //      {
   //          t->currentenv[0].__jmpbuf[JB_SP] = manglex64((unsigned long)(t->stack_orig + (STACK_SIZE-8) / 8 - 2));
   //          t->currentenv[0].__jmpbuf[JB_PC] = manglex64((unsigned long) wrapper);
   //          // wrapper();
   //          printf("wrapper called\n");
			// ualarm(50000,50000);
   //          longjmp(t->currentenv,1);
   //      }
    // printf("236--Inside thread start wrapper. t id %d\n", t->id);
  //   if (t->status==CREATED)
  //   {
  //   	t->status=RUNNING;

  //   	if (setjmp(t->startenv)==0)
		// {
		// 	insideSchduler=0;
		// 	t->status=RUNNING;
		// 	ualarm(50000,50000);
		// 	(t->func)(t->arg);
		// 	longjmp(t->startenv,1);
		// }
  //   	/* code */
  //   }
  //   else	
  //   {
  //   	t->status=RUNNING;
		// ualarm(50000,50000);

  //   	longjmp(t->currentenv,1);
  //   }
	
  //   // }
	
	// return;
		// ualarm(50000,50000);

	thread_exit(t);
	// printf("190--%d\n",t->status );
}
void myThread_schedule()
{
	// insideSchduler=1;
	ualarm(0,0);
	if (queue->queueLen==0)
	//if there is not other thread just return to the existing thread only or anywhere where it was called from 
	{
		// free(queue);
		// free(sv);
		// printf("Nothing wating in the ready queue. returning\n");
		if (currentThread!=NULL)
		{
			currentThread->status=RUNNING;
			/* code */
		}
		insideSchduler=0;
		ualarm(50000,50000);
		return;
	}
	else
	{
		if (currentThread==NULL)
		{
			/* code */
			// printf("274--queue head %d\n", queue->head);
			TCB_PTR t=queue->q_orig[queue->head];
			currentThread=t;
			cirDequeue(&queue);
			if (t->status==CREATED)
			{
				// t->status==RUNNING;
				if (setjmp(t->SVenv)==0)
				{
					thread_start_wrapper(t);
				}
				else
				{
					// printf("285--%d\n",t->status);
				}
				/* code */
			}
			else
			{
				printf("Some error. No current thread and the queue.Not thought of. Exiting\n");
				printf("Thread status %d\n",t->status );
				exit(-1);
			}
		}
		else
		{
			TCB_PTR t=queue->q_orig[queue->head];
			// TCB_PTR t=queue[0];
			if (currentThread->status!=EXITED)
			{
				cirEnqueue(&queue,currentThread);
				/* code */
			}
			currentThread=t;
			cirDequeue(&queue);
			// thread_start_wrapper(t);
			if (t->status==READY)
			{
				// t->status==RUNNING;
				if (setjmp(t->SVenv)==0)
				{
					thread_start_wrapper(t);
				}
				/* code */
			}
			else if (t->status==CREATED)
			{
				if (setjmp(t->SVenv)==0)
				{
					thread_start_wrapper(t);
				}
			}
			else
			{
				printf("status of thread not at queeue is %d, which is not RUNNING or CREATED. asseryion error. exiting\n", t->status );
				exit(-1);
			}
			// if (t->status==CREATED)
			// {
			// 	if (setjmp(t->SVenv)==0)
			// 	{
			// 		thread_start_wrapper(t);
			// 	}
			// }
			// else
			// {
			// 	insideSchduler=0;
			// 	longjmp(t->currentenv,1);
			// }
			
		}
		
	}
}
void myThread_join(mythread_t mtt)
{
	while(((mtt.tb))->status!=EXITED)
	{
	}
	free(((mtt.tb))->stack_orig);
	free((mtt.tb));
}
int main(int argc, char const *argv[])
{
	void *a;
	int count=atoi(argv[1]);
	mythread_t t[count];
	// mythread_t t2;
	for (int i = 0; i < count; ++i)
	{
		myThread_create(&(t[i]),NULL,(&f),a);
	}
	// i=myThread_create(&t2,NULL,(&f),a);
	// printf("196--%u\n", ((t.tb))->status);
	// myThread_join(t);
	for (int i = 0; i < count; ++i)
	{
		myThread_join(t[i]);
		/* code */
	}
	return 0;
}
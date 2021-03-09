#include <stdio.h>
#include <stdlib.h>
// #include <stdlib.h>
#include <ucontext.h>
#include <setjmp.h>

typedef struct tcb
{
	enum {
        CREATED,
        RUNNING,
        WAITING,
        EXITED,
    } status;
	int read;//exist status read or not 
	int id;
	jmp_buf env;
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
TCB_PTR *queue;
int queueSize=5;
int queueLen=0;
int thread_lib_init=0;
#define STACK_SIZE 8192
scheduler_val *sv;
void libInit()
{
	// queueSize=5;
	queue=malloc(sizeof(TCB_PTR)*queueSize);
	sv=malloc(sizeof(scheduler_val));
	thread_lib_init=1;//initialization done
}
int getQueueLoc()
{
	if (queueLen<queueSize)
		return queueLen++;
	else
	{
		return 0;//to be handled later
	}
}
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
	static int id=1;
	if (thread_lib_init==0)
	{
		libInit();
	}
	int index=getQueueLoc();

	tcb *t=malloc(sizeof(tcb));
	t->read=0;
	t->id=id++;
	t->status=CREATED;
	printf("%u\n",t->status );
	t->stack_size=STACK_SIZE;
	t->stack_orig=malloc(STACK_SIZE);
	if (growsLower(&index))
		t->stack_ptr=t->stack_orig+t->stack_size;
	else
		t->stack_ptr=t->stack_orig;

	t->func=start_routine;
	t->arg=args;
	// th=malloc(sizeof(mythread_t));
	// th->tb=malloc(sizeof(tcb*));
	th->tb=t;
	printf("%u\n", ((th->tb))->status);
	queue[index]=t;

	return t->id;
}
void thread_exit(TCB_PTR t)
{
	t->status=EXITED;
	printf("exited\n");
	printf("%u\n",t->status);
	longjmp(sv->env,1);
}
void thread_start_wrapper(TCB_PTR t)
{
	register void *top = t->stack_ptr;
    asm volatile(
        "mov %[rs], %%rsp \n"
        : [ rs ] "+r" (top) ::
    );
	if (setjmp(t->env)==0)
	{
		(t->func)(t->arg);
		longjmp(t->env,1);
	}
	// return;
	thread_exit(t);
}
void myThread_schedule()
{
	if (queueLen==0)
	{
		free(queue);
		free(sv);
		return;
	}
	else
	{
		TCB_PTR t=queue[0];
		queueLen--;
		if (setjmp(sv->env)==0)
			thread_start_wrapper(t);
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
	/* code */
	void *a;
	mythread_t t;
	int i=myThread_create(&t,NULL,(&f),a);
	// printf("%u\n", ((t.tb))->id);
	printf("%u\n", ((t.tb))->status);
	myThread_schedule();
	myThread_schedule();
	myThread_join(t);
	return 0;
}
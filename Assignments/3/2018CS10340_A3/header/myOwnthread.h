#include <setjmp.h>
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
void myThread_yield(void);
void mythread_mutex_lock(mythread_mutex_t *m);
void mythread_mutex_unlock(mythread_mutex_t *m);
int myThread_create(mythread_t *th,void *attr, void *start_routine, void *args);
void myThread_join(mythread_t mtt);
int mythread_mutex_init(mythread_mutex_t* m, void* a);
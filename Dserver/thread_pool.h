#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include "http.h"

#include "debug.h"


#define DEFAULE_NUM 4

struct task
{
	HttpRequest*f;
	void* arg;
	struct task*next;
};

void* ThreadWorker(void*arg);       //work function

typedef enum {
    immediately_shutdown = 1,
    grace_shutdown = 2
}shutdown_type;

class Pool
{
public:
	friend void* ThreadWorker(void*arg); 
	
	Pool(int num = DEFAULE_NUM);
	int ThreadPoolAdd(void*t,void*arg);      //add task
	~Pool();

	struct task* GetTask();

private:
	struct task*task_list;    //task list
	pthread_mutex_t lock;     //mutex lock
	pthread_cond_t  cond;      //
	pthread_t * threads_array; //thread array
	int thread_count;
	int task_list_size;
	int shutdown;
	int started;
	int grace;
	int thread_num;
};




#endif
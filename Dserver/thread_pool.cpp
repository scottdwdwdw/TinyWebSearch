#include "thread_pool.h"


//init method
Pool::Pool(int num)
{
	task_list = new task;
	if(task_list==NULL)
	{
		debug("can not create task_list");
		exit(-1);
	}
	task_list->f = NULL;
	task_list->arg = NULL;
	task_list->next = NULL;

	thread_count = 0;
	shutdown = 0;
	started = 0;
	task_list_size = 0;
	grace = 1;        //grace shutdown 
	thread_num = num;

	if(pthread_mutex_init(&lock,NULL)!=0)
	{
		debug("lock init fail.");
		exit(-1);
	}

	if(pthread_cond_init(&cond,NULL)!=0)
	{
		debug("cond init fail.");
		exit(-1);
	}

	threads_array = new pthread_t[num];
	if(threads_array==NULL)
	{
		debug("malloc space of threads_array fail.");
		exit(-1);
	}

	
	for(int i=0;i<num;++i)
	{
		if(pthread_create(&threads_array[i],NULL,ThreadWorker,this)!=0)
		{
			debug("create thread fail");
			continue;
		}

		++thread_count;
		++started;
	}
}

int Pool::ThreadPoolAdd(void*f,void*arg)
{
	if(f==NULL)
	{
		debug("invalid parameter.");
		return -1;
	}

	if(pthread_mutex_lock(&lock)!=0)
	{
		debug("mutex lock error.");
		return -1;
	}
	if(shutdown!=0)
	{
		debug("pool alread shutdown.");
		pthread_mutex_unlock(&lock);
		return -1;
	}

	struct task*t = new task;
	if(t==NULL)
	{
		debug("malloc space fail for new task");
		pthread_mutex_unlock(&lock);
		return -1;
	}
    //add task.

	t->f = (HttpRequest*)f;
	t->arg = arg;
	t->next = task_list->next;
	task_list->next = t;
	++task_list_size; 

	int rrr = -1;
	rrr=pthread_cond_signal(&cond);
	//debug("the rrrrrrr is : %d",rrr);
	if(rrr!=0)    //send message
	{
		debug("pthread_cond_signal fail.,%d",rrr);
		perror("pthread_cond_signal error");
		pthread_mutex_unlock(&lock);
		return -1;
	}
    if(pthread_mutex_unlock(&lock)!=0)
	{
		debug("pthread_mutex_unlock fail.");
		return -1;
	}
	

	return 0;
}


Pool::~Pool()
{
	int res;
	int flag=0;
	if(!shutdown)
	{
		do
		{
			res = pthread_mutex_lock(&lock);
		    if(res!=0)
		    {
		    	debug("pthread_mutex_lock error.");
		    	flag = -1;
		    	break;
		    }

		    shutdown = grace?grace_shutdown:immediately_shutdown;

		    res = pthread_cond_broadcast(&cond);
		    if(res!=0)
		    {
			   debug("broadcast fail.");
			   flag = -1;
			   break;
		    }
		    res = pthread_mutex_unlock(&lock);
		    if(res!=0)
		    {
		    	debug("pthread_mutex_unlock fail.");
		    	flag=-1;
		    	break;
		    }

		    for(int i=0;i<thread_count;++i)
		    {
		    	res = pthread_join(threads_array[i],NULL);
		    	if(res!=0)
		    	{
		    		debug("pthread_join fail.");
		    		flag = -1;
		    	}
		    }

		}while(0);
		if(!flag)
		{
			pthread_mutex_destroy(&lock);
			pthread_cond_destroy(&cond);
			//destory resource
			struct task*current;
			while(task_list!=NULL)
			{
				current = task_list;
				task_list = task_list->next;
			    delete current;
			}

			delete [] threads_array;
		}

		
	}
	else
	{
		debug("alread shutdown.");
	}
}


//customer
void* ThreadWorker(void*arg)
{

	Pool*p = (Pool*)arg;
	while(1)
	{
		//debug("thread  %lu run...",pthread_self());
		if(pthread_mutex_lock(&(p->lock))!=0)
		{
			debug("pthread_mutex_lock fail.");
			break;
		}
     
		while((p->task_list_size==0)&&(p->shutdown==0))
		{
			pthread_cond_wait(&(p->cond),&(p->lock));     //wait
			//debug("wake up.");
		}

		if(p->shutdown==immediately_shutdown)
		{
			debug("immediately_shutdown shutdown.");
			break;
		}
		else if(p->shutdown == grace_shutdown&&p->task_list_size==0)
		{
			break;
		}

		//get a task;
		struct task*t = p->task_list->next;
		if(t==NULL)
		{
			pthread_mutex_unlock(&(p->lock));
			continue;
		}
		p->task_list->next = t->next;    //move
		--(p->task_list_size);
		pthread_mutex_unlock(&(p->lock));
		//debug("thread %lu get a task.",pthread_self());
		(t->f->RequestParse)(t->arg);                          //process

		delete t;
	}

	--p->started;
	pthread_mutex_unlock(&(p->lock));
	pthread_exit(NULL);   //thread exit

	return NULL;
}



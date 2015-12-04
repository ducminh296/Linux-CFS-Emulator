//*****************************************************************************
// CFS-emulator.c - Implement the Complete fair scheduler emulator
//
// AUTHOR: Minh Mai
//
// The emulator includes:
// - A producer thread generates 20 processes initially
// - 4 consumer threads (4 CPUs) execute processes
// - A balancer thread balances the process queues among 4 consumers
//
//*****************************************************************************

#include "util.h"
#include <time.h>
#define PROCESS_FILE "processes.txt"
#define NUM_CPU 4 //number of CPUs or consumers
#define NUM_QUEUE 3 //number of RQs

// ****************************************************************************
// Create a global table of run queues where:
// Row number is CPU number
// Column number is RQ numer
//
// Tables:
// CPU 0: RQ0 RQ1 RQ2
// CPU 1: RQ0 RQ1 RQ2
// CPU 2: RQ0 RQ1 RQ2
// CPU 3: RQ0 RQ1 RQ2
//
// For Examples: RQ[0,1] -> RQ1 of CPU0
// ****************************************************************************
struct queue_t RQ[NUM_CPU][NUM_QUEUE];
pthread_mutex_t QM[NUM_CPU][NUM_QUEUE];//mutex for each queue

//****************** GLOBAL VARIABLES *****************************************
int p_count[4];//stores number of processes in each CPU
int n_process; //stores total number of processes still in the ready queue
int b_value; //the number of processes each CPU should have in their ready queue
int running = 1; //indicate that the emulator is still running

/** producer thread **/
void *producer(void *arg);
struct process_t ltop(char *line);
void process_status(void);
/** consumer thread **/
void *consumer(void *arg);
/** balancer thread **/
void *balancer(void *arg);

int main(int argc, char const *argv[])
{
	srand(time(NULL));//seed the random generator
	producer(NULL);
	return 0;
}

//*****************************************************************************  
// Producer implementation
//
// Implement a producer thread
//
// The thread generates processes distributed into the queue of each comsumers 
//
// Initially, read processes infos from a file name defined in PROCESS_FILE
// 
// Generates 20 processes: 12 SCHED_NORMAL, 4 SCHED_FIFO, 4 SCHED_RR
// Each run queue will has 5 process intitally
//
//*****************************************************************************
void *producer(void *arg)
{
	FILE *file; //file descriptor
	char line[100]; //store a line of the text file
	struct process_t process; //store
	int CPU,res;
	int count = 1;
	file = fopen(PROCESS_FILE, "r");
	if (file == NULL) 
	{
		fprintf(stderr,"Producer: ERROR opening file.\n");
		exit(EXIT_FAILURE);
	}
	//Read the number of processes in the file
	fgets(line,100,file);
	n_process = atoi(line);
	if (n_process % NUM_CPU != 0)
	{
		fprintf(stderr,"Producer: Initial number of processes is invalid.\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		//b_value is the number of processes each CPU should has so that
		//all CPUs has the same number of processes
		b_value = n_process/NUM_CPU;
	}
	//Initially, each CPU has 0 process in the queue
	for (int i = 0; i < NUM_CPU; i++)
	{
		p_count[i]= 0;
	}
	//Initialize the queue for each CPU
	for (int i = 0; i< NUM_CPU; i++)
		for (int j=0; j<NUM_QUEUE; j++)
		{
			init_queue(&RQ[i][j]);//init queue
			//init queue mutex
			res = pthread_mutex_init(&QM[i][j], NULL);
   			if (res != 0) {
        		perror("Mutex initialization failed");
        		exit(EXIT_FAILURE);
    		}
		}
	//Read and distribute proccess into ready queue for each consumers
	while(1)
	{
		if (fgets(line,100,file) == NULL) break; //if reach EOF
		process = ltop(line);// convert string into process
		process.pid = count++;// assign pid to process
		process.prio = process.static_prio;
		process.time_slice = 0;
		process.accu_time_slice = 0;
		//find an available CPU to assign the process into
		do
		{
			CPU = (rand() % 4); // generate a random CPU number 0 - 3
		} while (p_count[CPU]>=b_value);
		process.last_cpu = CPU;
		// assign the process into the CPU queue
		// process is SCHED_NORMAL then store in RQ1
		if (strcmp(process.sched,"SCHED_NORMAL") == 0)
		{
			pthread_mutex_lock(&QM[CPU][1]);
			append(&process, &RQ[CPU][1]);
			pthread_mutex_unlock(&QM[CPU][1]);
		}
		else //else store process in RQ0
		{
			pthread_mutex_lock(&QM[CPU][0]);
			append(&process,&RQ[CPU][0]);
			pthread_mutex_unlock(&QM[CPU][0]);
		}
		p_count[CPU]++; //increase the number of processes for that CPU
	}
	process_status();
	pthread_exit(NULL);
}
//*****************************************************************************
//
//! \Internal
//! The function transfer process information string into process struct
//!
//! \param string containing the process info
//!
//! \return process_t containing process info extracted from the string
//
//*****************************************************************************
struct process_t ltop(char *line)
{
	struct process_t process;
	char *token;
	char delimiter[] = " ,";
	token = strtok(line, delimiter);//extract the SCHED info
	strcpy(process.sched, token);//store SCHED info into sched
	token = strtok(NULL,delimiter);//extract the static_prio
	process.static_prio = atoi(token);//store the info
	token = strtok(NULL,delimiter);//extract the execution time
	process.exec_time = atoi(token);//store the info
	return process;
}
//*****************************************************************************
//
//! \Internal
//! The function print out the processes status table
//!
//! \param None
//!
//! print out to the console the table containing all processes 
//! 
//! \return NONE
//
//*****************************************************************************
void process_status(void)
{
	struct process_t process;
	struct queue_t queue;
	printf("%-3s | %-12s | %-3s | %-8s | %-11s | %-4s | %-9s | %-10s | %-14s\n",\
		   "PID", "SCHED_SCHEME", "CPU", "last_CPU", "static_prio", "prio",		\
		   "exec_time", "time_slice", "accu_time_slice");
	for (int i = 0; i<NUM_CPU; i++)
		for (int j = 0; j<NUM_QUEUE; j++)
		{
			queue = RQ[i][j];
			for (int k = queue.out; k<queue.in; k++)	
			{
				process = queue.pool[k];
				printf("%-3d | %-12s | %-3d | %-8d | %-11d | %-4d | %-9d | %-10d | %-14d\n",\
						process.pid, process.sched, i, process.last_cpu, 		  \
						process.static_prio, process.prio, process.exec_time,	  \
						process.time_slice, process.accu_time_slice);
			}
		}
}
//*****************************************************************************  
// Consumer implementation
//
// Implement a consumer thread
//
// When created, the *arg tell the thread its CPU number (From 0 to 3)
//
// The thread execute the processes in its own ready queue
//
// The processes has 3 queue: RQ0, RQ1, RQ3
//
//*****************************************************************************
void *consumer(void *arg)
{
	int CPU = *(int*)arg;//CPU number assigned from for this consumer
	pthread_exit(NULL);
}
//*****************************************************************************
// Balancer implementation
//
// Implement a balancer thread
//
// The thread balance the number of queues distributed for each consumers
//
//*****************************************************************************
void *balancer(void *arg)
{
	struct process_t *process;
	int new_cpu,i,j;
	int possible = 1;
	while (running)
	{
		/* update the b_value */
		/* b_value is the number of processes each CPU should have so that  
		** processes are distributed equally to all CPUs */
		b_value = n_process/NUM_CPU;
		for (i = 0; i < NUM_CPU; i++)
		{
			possible = 1;
			while (p_count[i] > b_value && possible) //if CPU has more than b_value processes
			{
				//take out one process from the CPU's queue
				process = NULL;
				for (j=NUM_QUEUE-1; j>=0; j--)
				{
					pthread_mutex_lock(&QM[i][j]);
					process = take(&RQ[i][j]);
					pthread_mutex_unlock(&QM[i][j]);
					if (process != NULL) break;
				}
				// if cannot take out any process then break
				if (process == NULL) {
					possible = 0;
					break;
				}
				//find a CPU to move the process over
				for (j=0; j< NUM_CPU; j++)
				{
					if (j != i && p_count[j]< b_value) break;
				}
				// if cannot find any available cpu, then break
				if (j == NUM_CPU && p_count[j]>=b_value) {
					possible = 0;
					break;
				}
				//found a new cpu for process
				new_cpu = j; 
				// update process info
				process->last_cpu = i;
				// add process into new cpu's queue
				// process is SCHED_NORMAL then store in RQ1
				if (strcmp(process->sched,"SCHED_NORMAL") == 0)
				{
					pthread_mutex_lock(&QM[new_cpu][1]);
					append(process, &RQ[new_cpu][1]);
					pthread_mutex_unlock(&QM[new_cpu][1]);
				}
				else //else store process in RQ0
				{
					pthread_mutex_lock(&QM[new_cpu][0]);
					append(process,&RQ[new_cpu][0]);
					pthread_mutex_unlock(&QM[new_cpu][0]);
				}
				//update p_count
				p_count[i]--;
				p_count[new_cpu]++;
			}
		}
		sleep(1);
	}
	pthread_exit(NULL);
}
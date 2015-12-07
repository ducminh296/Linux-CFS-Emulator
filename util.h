//*****************************************************************************
// util.h - Define utilites for use in programs
//
// AUTHOR: Minh Mai
//
// Utility includes:
// + PROCESS QUEUE FACILITY
//
//
//
//*****************************************************************************

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "sched.h"
#include <sys/sem.h>

#define QUEUE_SIZE 100

//*****************************************************************************
//
// PROCESS QUEUE Facility
//
//*****************************************************************************
/* Define a queue can hold up to 100 processes */
struct queue_t{
	int in;
	int out;
	int k;
	struct process_t pool[QUEUE_SIZE];
};
//This method initilize the queue before using 
void init_queue(struct queue_t *queue);
//This method add a new process into the queue
void append(struct process_t *process, struct queue_t *queue);
//This method take the highest priority process out of the queue
struct process_t *take();
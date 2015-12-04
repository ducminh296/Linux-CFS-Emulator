//*****************************************************************************
// sched.h - Define the task_struct 
//
// AUTHOR: Minh Mai
//
//
//
//
//
//*****************************************************************************
struct process_t {
  int pid; // an integer PID created by the producer
  char sched[15]; //SCHED_FIFO, SCHED_RR, SCHED_NORMAL
  int static_prio; // Static priority, real-time: 0-99, normal: 100-139
  int prio; //dynamic priority
  int exec_time; //expected execution time
  int time_slice; //time slice for round robin
  int accu_time_slice; //accumulated time slice
  int last_cpu; //the CPU (thread) that the process last ran
};

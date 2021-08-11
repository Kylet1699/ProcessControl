#ifndef ProcessControl_h
#define ProcessControl_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//max size of processes
#define MAX_SIZE 100

//process state
enum process_type{
    process_type_running = 1000,
    process_type_block = -1000,
    process_type_ready = 500,
    process_type_init = -500,
};

enum process_msg{
    process_msg_send = 1,
    process_msg_receive = 2,
    process_msg_reply = 3,
};

//Sem struct
typedef struct SEM {
    int value;
    int id;
    struct SEM *next;
}sem_t;

//PCB struct
typedef struct PCB_Type{
    //process id
    int pid;
    //msg pid
    int msg_pid;
    //process priority
    int priority;
    //burst time
    int size;
    //content (receive/send/reply)
    char content[40];
    //process state
    int state;
    int msg_type;
    //next in line
    struct PCB_Type *next;
}PCB;

//init proces
void init(PCB*running_list, int *size);

//create new process
void create(PCB *running_list,PCB *block_list,int *size);
//display all process
void show_running(PCB *running_list, int prio);
//send
void send(PCB *running_list, PCB *block_list, int *size);
//receive
void receive(PCB *running_list, PCB *block_list, int *size);
//reply
void reply(PCB *running_list, PCB *block_list, int *size);
//Move a process to block queue
void block(PCB *running_list,PCB *block_list, int id, int *size);
//kill a process by id
void killed(PCB *running_list, PCB *block_list, int *size);
//exit the current process using CPU
void ex(PCB *running_list, int *size);
//move from block to ready
void wake_up(PCB *running_list,PCB *block_list, int pid, int *size);
//fork
void fo(PCB *running_list, int *size);
//Quamtum of runnign process expires
void quantum(PCB *running_list, int*size);
//new sem
void new_sem(sem_t *sem_list, int *size);
void sem_info(sem_t *sem_list);
//p_sem
void p_sem(PCB *running_list, PCB *block_list, sem_t *sem_list, int *size);
//v_sem
void v_sem(PCB *running_list, PCB *block_list, sem_t *sem_list, int *size);
//Process info
void procinfo(PCB *running_list, PCB *block_list);

//total info
void totalinfo(PCB *running_list, PCB *block_list, int *size);

//check if process exists in ready queue
int exist_in_running(PCB *running_list,int pid);
//check if process exists in block queue
int exist_in_block(PCB *block_list,int pid);
//find a process in queue
PCB *find(PCB *list,int pid);

#endif
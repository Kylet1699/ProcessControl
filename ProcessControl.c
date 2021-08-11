
#include "ProcessControl.h"
#include <string.h>


//init proces
void init(PCB *running_list, int *size) {

    //malloc space
    PCB *p = (PCB *)malloc(sizeof(PCB));
    assert(p != NULL);

    p -> next = NULL;

    //add init to queue
    PCB *s = running_list;
    s -> next = p;

    //add info to member
    p -> pid = 0;
    p -> priority = 3;
    p -> size = 0;
    p -> state = process_type_init;
    char msg[] = "init";
    strcpy(p -> content, msg);

    *size = *size + 1;
    printf("init process has been created!\n"); 
    printf("Process ID:%d\n",p->pid);
    printf("Process priority:%d\n",p->priority);
    printf("Burst time:%d\n",p->size);
    printf("Currently have %d process in queue\n", *size);
    printf("___________\n");
}

//create new process
void create(PCB *running_list,PCB *block_list,int *size){
    if (*size >= MAX_SIZE) {
        //not enough memory
        printf("Creation failed, already reached maximum number of processes\n");
        return;
    }
    //malloc space
    PCB *p = (PCB *)malloc(sizeof(PCB));
    assert(p != NULL);
    
    //set pid for this process
    int pid;
    printf("Please enter process ID: ");
    scanf("%d",&pid);
    
    //check if pid is duplicated
    if (exist_in_running(running_list, pid)) {
        printf("Entered process ID already exists in ready queue\n");
        return;
    }
    if (exist_in_block(block_list, pid)) {
        printf("Entered process ID already exists in block queue\n");
        return;
    }
    //no dupe, save the pid
    p->pid = pid;
    
    //set other info for this process
    printf("Please enter priority, 0(high), 1(norm), 2(low): ");
    scanf("%d",&p->priority);
    while (p -> priority != 0 && p -> priority != 1 && p -> priority != 2) {
        printf("Invalid priority, please enter again, 0(high), 1(norm), 2(low): ");
        scanf("%d",&p -> priority);
    }
    printf("Please enter CPU burst time for this process: ");
    scanf("%d",&p->size);
    memset(p -> content, '\0', 40);
    // printf("Plesae enter the content of message:\n");
    // scanf("%s",p->content);
    p->state = process_type_running;
    p->next = NULL;
    
    //place in ready queue
    PCB *s = running_list;
    while (s->next != NULL) {
        s = s->next;
    }
    s->next = p;
    
    //number of process + 1
    *size = *size + 1;
    printf("Number of processes in queue %d\n", *size);
}

//display all processes
//used to display both ready and block queue
void show_running(PCB *running_list, int prio){
    int count = 0;
    PCB *s = running_list->next;
    int p_prio = prio;
    
    while (s != NULL) {
        if (s -> priority == p_prio) {
            printf("Process ID: %d\n",s->pid);
            printf("Process priority: %d\n",s->priority);
            printf("Burst time: %d\n",s->size);
            printf("Content: %s\n",s->content);
            printf("___________\n");  
            count++;          
        }
        s = s->next;
    }
    if (count == 0) {
        printf("No process in queue\n");
        printf("_____________________\n");
    }
}

//send
void send(PCB *running_list, PCB *block_list, int *size) {
    if (*size == 1) {
        printf("No available process to send the message\n");
        return;
    }

    int pid;
    char msg[40];
    int priority = 4;
    PCB *s = running_list -> next;
    //find the first process with lowest priority
    while (s != NULL) { 
        if (s -> priority < priority) {
            priority = s -> priority;
            pid = s -> pid;
            if (priority == 0){
                break;
            }
        }
        s = s -> next;
    }
    s = find(running_list, pid);
    s = s -> next;
    s -> msg_type = process_msg_send;
    //gets the id to send the message to
    printf("Pleae enter the proces ID you would like to send the message to: ");
    scanf("%d", &s -> msg_pid);
    if (s -> msg_pid == pid) {
        printf("Cannot send the message to self\n");
        return;
    }
    if (s -> msg_pid == 0) {
        printf("Cannot send mesage to init proces");
        return;
    }
    //save the mesage and block the sender until reply 
    printf("Plese enter the message you would like to send (max 40 characters): ");
    scanf("%s", s -> content);
    block(running_list, block_list, pid, size); 
    printf("Message sent! Waiting to receive reply from process %d\n", s -> msg_pid);
    s = block_list;
    //if there's a receive process in block queue, wake it up
    while (s -> next != NULL) {
        if (s -> next -> msg_type == process_msg_receive) {
            pid = s -> next -> pid;
            s -> next -> state = process_type_ready;
            s -> next -> msg_type = 0;
            wake_up(running_list, block_list, pid, size);
            return;
        }
        s = s -> next;
    }
}

//receive
void receive(PCB *running_list, PCB *block_list, int *size) {
    int pid;
    int priority = 3;
    PCB *s = block_list -> next;
    while (s != NULL) { 
        if (s -> priority < priority && s -> msg_type == process_msg_send) {
            priority = s -> priority;
            pid = s -> pid;
            if (priority == 0){
                break;
            }
        }
        s = s -> next;
    }
    //check if any process has sent a message, if not create a new process in block queue to reeive message
    if (priority == 3) {
        printf("No messages to receive, process is now blocked until one arrives\n");
        s = running_list -> next;
        while (s != NULL) { 
        if (s -> priority < priority) {
            priority = s -> priority;
            pid = s -> pid;
            if (priority == 0){
                break;
            }
        }
        s = s -> next;
        }
        block(running_list, block_list, pid, size);
        s = block_list -> next;
        while (s -> next != NULL) { 
            s = s -> next;
        }
        s -> msg_type = process_msg_receive;
        s -> state = process_type_block;
        return;
    }
    
    if (s = find(block_list, pid)) {
        s = s -> next;
        printf("Message received from process %d: %s",s -> pid, s -> content);
    }
}

//reply
void reply(PCB *running_list, PCB *block_list, int *size) {
    //sender's pid
    int pid;
    printf("Please enter the process ID to reply to: ");
    scanf("%d", &pid);
    if (exist_in_block(block_list, pid)) {
        PCB *s = find(block_list, pid);
        //check if the pid has sent a message or not
        if (s -> next -> msg_type != process_msg_send) {
            printf("The process ID entered did not send a message\n");
            return;
        }
        printf("Please enter the reply message (max 40 characters): ");
        scanf("%s", s -> next -> content); 
        //set msg state of the sender
        s -> next -> msg_type = process_msg_reply;
        //wake up the sender 
        wake_up(running_list, block_list, pid, size);
        printf("Successfully sent reply, process ID %d is now in ready queue", pid);
        return;
    }
    else {
        printf("The process ID entered did not send a message\n");
        return;
    }
}

//Move a process to blocked queue
void block(PCB *running_list,PCB *block_list,int id, int *size){
    if (size == 0) {
        printf("Process you have chosen is the init process\n");
        return;
    }
    int pid = id;
    //check if process is in the ready queue
    if (exist_in_running(running_list, pid)) {
        //exists, find the previous node to the target node
        PCB *s = find(running_list, pid);
        //change state of process
        s->next->state = process_type_block;
        
        //move the process to block queue 
        PCB *p = block_list;
        while (p->next != NULL) {
            p = p->next;
        }

        p->next = s->next;
        
        //remove process from ready queue
        s->next = s->next->next;
        
        p->next->next = NULL;
        
        *size = *size - 1;
        printf("Successfully moved process %d to block queue\n", pid);
    }else{
        printf("Invalid process ID, please try again!\n");
    }
}

//kill process by ID
void killed(PCB *running_list, PCB *block_list, int *size){
    if (*size == 1) {
        printf("Kill failed. Only init process in queue\n");
        return;
    }
    
    int pid;
    printf("Enter process ID to kill: ");
    scanf("%d",&pid);
    //check if ID exists in queue
    if (exist_in_running(running_list, pid)) {
        //Exists, returns target process's previous process
        PCB *s = find(running_list, pid);
        if (s -> next -> state = process_type_init) {
            printf("Init process cannot be killed\n");
            return;
        }
        //Keep target process addres
        PCB *thisThread = s->next;
        //remove process from queue
        s->next = s->next->next;
        *size = *size - 1;
        
        //Free process
        free(thisThread);
        printf("Kill complete, process ID %d has been removed from the ready queue\n", pid);
        return;
    }
    else if (exist_in_block(block_list, pid)) { 
        //Exists, returns target process's previous process
        PCB *s = find(block_list, pid);
        //Keep target process addres
        PCB *thisThread = s->next;
        //remove process from queue
        s->next = s->next->next;
        *size = *size - 1;
        
        //Free process
        free(thisThread);
        printf("Kill complete, process ID %d has been removed from the block queue\n", pid);
        return;
    }
    else{
        printf("Invalid process ID in both ready & block queue\n");
        return;
    }
}

void ex(PCB *running_list, int *size) {
    if (*size == 1) {
        printf("Exit failed. Only init process in queue\n");
        return;
    }
    
    int pid;
    int priority = 4;
    PCB *s = running_list -> next;
    //find the first process with lowest priority
    while (s != NULL) { 
        if (s -> priority < priority) {
            priority = s -> priority;
            pid = s -> pid;
            if (priority == 0){
                break;
            }
        }
        s = s -> next;
    }
    s = find(running_list, pid);
    if (s -> next -> state == process_type_init) {
        printf("Init process cannot be killed");
        return;
    }
    PCB *thisThread = s -> next;
    s -> next = s -> next -> next;
    *size = *size - 1;
    free(thisThread);

    //check the next process in line to get the CPU

    priority = 4;
    pid = -1;
    s = running_list -> next;
    while (s != NULL) {
        if (s -> priority < priority) { 
            pid = s -> pid;
            priority = s -> priority;
            if (priority == 0) {
                break;
            }
        }
        s = s -> next;
    }
    
    printf("Exit complete, process with ID %d now gets control of the CPU", pid);
    return;
}

//move from block to ready
void wake_up(PCB *running_list,PCB *block_list,int pid, int *size){
    PCB *s = block_list;
    if (s->next == NULL) {
        printf("No process to wake up\n");
        return;
    }
    
    int id = pid;
    //check if process exists
    if (exist_in_block(block_list, id)) {
        //exists
        s = find(block_list, id);
        s->next->state = process_type_running;
        PCB *p = running_list;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = s->next;
        s->next = s->next->next;
        p->next->next = NULL;

        *size = *size + 1;
        printf("Success\n");
    }else{
        printf("Failed\n");
    }
}

void quantum(PCB *running_list, int *size) {
    if (*size == 1) {
        printf("No process in queue, init process is executed");
        return;
    }
    printf("Time quantum is 4\n");
    PCB *s = running_list -> next;
    int pid;
    int priority = 4;
    //find the first process with lowest priority
    while (s != NULL) { 
        if (s -> priority < priority) {
            priority = s -> priority;
            pid = s -> pid;
            if (priority == 0){
                break;
            }
        }
        s = s -> next;
    }
    s = find(running_list, pid);
    int time = s -> next -> size;
    if (s -> next -> size <= 4) {
        PCB *thisThread = s->next;
        //remove process from queue
        s->next = s->next->next;
        *size = *size - 1;
        //Free process
        free(thisThread);
        printf("Process ID %d with burst time of %d has been completed and removed from the queue\n", pid, time);
        return;
    }
    else {
        s -> next -> size = time - 4;
        time = s -> next -> size;
        PCB *p = running_list;
        while (p -> next != NULL) {
            p = p -> next; 
        }
        p -> next = s -> next;
        s -> next = s -> next -> next;
        p -> next -> next = NULL;
        printf("Process ID %d now has burst time of %d and moved to the end of queue\n", pid, time);
        return; 
    }
}

//fork
void fo(PCB *running_list, int *size) {
    if (*size == 1) {
        printf("Attempted to fork init process and failed\n");
        return;
    }

    //malloc space
    PCB *p = (PCB *)malloc(sizeof(PCB));
    assert(p != NULL);

    PCB *s = running_list -> next;
    int pid;
    int priority = 4;
    //find the first process with lowest priority
    while (s != NULL) { 
        if (s -> priority < priority) {
            priority = s -> priority;
            pid = s -> pid;
            if (priority == 0){
                break;
            }
        }
        s = s -> next;
    }
    s = find(running_list, pid);
    s = s -> next;
    //set info
    p -> msg_pid = s -> msg_pid;
    p -> priority = s -> priority;
    p -> size = s -> size;
    strcpy(p -> content, s -> content);
    p -> state = s -> state;
    p -> msg_type = s -> msg_type;

    //find a process id for the child
    int count = 1;
    s = running_list; 
    while (s -> next != NULL) {
        s = s -> next;
        if (s -> pid == count) {
            count++;
            s = running_list;
        }
    }
    p -> pid = count;
    
    //putting the new node to the end of the list
    
    s = running_list;
    while (s -> next != NULL) {
        s = s -> next;
    }
    s -> next = p; 
    p -> next = NULL;
    *size = *size + 1;
    printf("Process ID %d has been forked with new ID %d and put to the end of the queue\n", pid, p -> pid);
    return;
}

//check process infor mation by ID
void procinfo(PCB *running_list, PCB *block_list) {
    int pid = -1;
    printf("Please enter process ID: ");
    scanf("%d",&pid);
    if (exist_in_running(running_list, pid)) {
        PCB *p = find(running_list, pid);
        p = p -> next;
        printf("___________\n");
        printf("Process is in ready queue\n");
        printf("Process ID:%d\n",p->pid);
        printf("Process priority:%d\n",p->priority);
        printf("Burst time:%d\n",p->size);
        printf("Content:%s\n",p->content);
        printf("___________\n");
    }
    if (exist_in_block(block_list, pid)) {
        PCB *p = find(block_list, pid);
        p = p -> next;
        printf("___________\n");
        printf("Process is in block queue\n");
        printf("Process ID:%d\n",p->pid);
        printf("Process priority:%d\n",p->priority);
        printf("Burst time:%d\n",p->size);
        printf("Content:%s\n",p->content);
        printf("___________\n");
    }
}

void totalinfo(PCB *running_list, PCB *block_list, int *size) {
    //prints every node in every type of queue
    printf("Total number of process in ready queue right now is %d\n\n", *size);
    printf("INIT PROCESS\n\n");
    show_running(running_list, 3);
    printf("Priority 0 of ready queue\n\n");
    show_running(running_list, 0);
    printf("Priority 1 of ready queue\n\n");
    show_running(running_list, 1);
    printf("Priority 2 of ready queue\n\n");
    show_running(running_list, 2);
    printf("Priority 0 of block queue\n\n");
    show_running(block_list, 0);
    printf("Priority 1 of block queue\n\n");
    show_running(block_list, 1);
    printf("Priority 2 of block queue\n\n");
    show_running(block_list, 2);
}

void sem_info(sem_t *sem_list) {
    //shows sem list info
    sem_t *s = sem_list;
    while (s -> next != NULL) {
        printf("sem value is ");
        s = s -> next;
        printf("%d", s -> value);
    }
}

void new_sem(sem_t *sem_list, int *size) {
    //malloc space
    sem_t *s = (sem_t *)malloc(sizeof(sem_t));
    assert(s != NULL);

    //user inputs the initial value for the sem
    int value = 0;
    printf("Semaphore initialized, please enter initial value: ");
    scanf("%d", &value);
    if (value < 0) {
        printf("Error! Initial value must be 0 or higher\n");
        return;
    }
    int sid;
    //gets sem ID from user input
    printf("Please enter sem ID between 0 to 4: ");
    scanf("%d", &sid);
    if (sid < 0 || sid > 4){
        printf("Error! Please enter a value between 0 and 4\n");
        return;
    }

    s -> next = NULL;

    //check if sem ID has been initialized already
    sem_t *p = sem_list;
    while (p -> next != NULL) {
        if (p -> next -> id == sid) {
            printf("Error! ID already exist\n");
            return;
        }
        p = p -> next;
    }
    s -> id = sid;
    p -> next = s;
    s -> value = value;
    *size = *size + 1;
    printf("Successfully initialized semaphore ID %d\n", sid);
}

void p_sem(PCB *running_list, PCB *block_list, sem_t *sem_list, int *size){
    if (*size == 1) {
        printf("No process in the queue to execute semaphore P\n");
        return;
    }

    //get the sem id
    int sid;
    printf("Please enter semaphore ID: ");
    scanf("%d", &sid);

    //get the current process
    int pid;
    int priority = 4;
    PCB *p = running_list -> next;
    //find the first process with lowest priority, p is one before the target
    while (p != NULL) { 
        if (p -> priority < priority) {
            priority = p -> priority;
            pid = p -> pid;
            if (priority == 0){
                break;
            }
        }
        p = p -> next;
    }

    //get the semaphore from the list, s is one before the target
    int flag = 0;
    sem_t *s = sem_list;
    while (s -> next != NULL) {
        if (s -> next -> id == sid){
            flag = 1;
            break;
        }
        s = s -> next;
    }

    //check if semaphore id is initialized
    if (flag == 0) {
        printf("Semaphore %d was not initialized\n", sid);
        return;
    }

    //if sem value is <= 0, then block the current process
    if (s -> next -> value <= 0) { 
        block(running_list, block_list, pid, size);
        printf("Process ID %d is blocked\n", pid);
    }
    //else, sem value goes down by 1 and return
    else {
        int sem_value = s -> next -> value;
        s -> next -> value = sem_value - 1;
        printf("Process ID %d is not blocked\n", pid);
        printf("Semaphore ID %d now has value of %d\n", s -> next -> id, s -> next -> value);     
    }
}

void v_sem(PCB *running_list, PCB *block_list, sem_t *sem_list, int *size){

    PCB *b = block_list;
    if (b -> next == NULL) {
        printf("There is no process in blocked queue to ready\n");
        return;
    }
    int sid;
    printf("Please enter semaphore ID: ");
    scanf("%d", &sid);

    //get the semaphore from the list, s is one before the target
    int flag = 0;
    sem_t *s = sem_list;
    while (s -> next != NULL) {
        if (s -> next -> id == sid){
            flag = 1;
            break;
        }
        s = s -> next;
    }

    //check if semaphore id was initialized
    if (flag == 0) {
        printf("Semaphore %d was not initialized\n", sid);
        return;
    }
    int sem_value = s -> next -> value;
    s -> next -> value = sem_value + 1;
    printf("Semaphore ID %d now has a value of %d\n", sid, s -> next -> value);

    //if sem is higher than 0, wake up a process from block queue
    if (s -> next -> value > 0) {
        //get the current process
        int pid;
        int priority = 4;
        PCB *p = block_list -> next;
        //find the first process with lowest priority, p is one before the target
        while (p != NULL) { 
            if (p -> priority < priority) {
                priority = p -> priority;
                pid = p -> pid;
                if (priority == 0){
                    break;
                }
            }
            p = p -> next;
        }
        wake_up(running_list, block_list, pid, size);
        printf("Process ID %d has been placed in the ready queue", pid);
        return;
    }
    //if not then printf msg and return
    else {
        printf("Semaphore %d still has a value less or equal to 0, no process was readied\n", sid);
        return;
    }
}


//check if pid exists in running, 0 = no, 1 = yes
int exist_in_running(PCB *running_list,int pid){
    int result = 0;
    
    PCB *s = running_list->next;
    //iterates through the list
    while (s != NULL) {
        if (s->pid == pid) {
            //if exists, return right away
            result = 1;
            break;
        }
        s = s->next;
    }
    return result;
}
//check if pid exisst in block, 0 = no, 1 = yes
int exist_in_block(PCB *block_list,int pid){
    int result = 0;
    
    //iterates through the list
    PCB *s = block_list->next;
    while (s != NULL) {
        if (s->pid == pid) {
            //exists, return right away
            result = 1;
            break;
        }
        s = s->next;
    }
    
    return result;
}

//return the node before the target with pid
PCB *find(PCB *list,int pid){
    PCB *s = list;
    
    while (s->next != NULL) {
        if (s->next->pid == pid) {
            return s;
        }
        s = s->next;
    }
    
    return NULL;
}
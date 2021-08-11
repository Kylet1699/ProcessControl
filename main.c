

#include "ProcessControl.h"

static int storage_number = 0;
static int sem_num = 0;

void showLine(){
    printf("********************************\n");
}

int main(int argc, const char * argv[]) {
    
    //initialize ready queue
    PCB *running_list = (PCB *)malloc(sizeof(PCB));
    running_list->next = NULL;
    
    //initialize block queue
    PCB *block_list = (PCB *)malloc(sizeof(PCB));
    block_list->next = NULL;

    sem_t *sem_list = (sem_t *)malloc(sizeof(sem_t));

    char choose = 'A';
    //very important cannot delete
    int initialize = 1;

    showLine();
    printf("*     Process Scheduling Simulation      *\n");
    showLine();

    init(running_list, &storage_number);

    while (choose) {
        //show menu
        
        printf("\n");
        showLine();
        printf("'C' Create  'F' Fork     'K' Kill   'E' Exit           'Q' Quantum\n");
        printf("'S' Send    'R' Receive  'Y' Reply  'N' New Semaphore  'P' Semaphore P\n");
        printf("'V' Semaphore V          'I' Procinfo                  'T' Totalinfo\n");
        // printf("'Z' Display ready queue  'X' Display block queue\n");
        showLine();
            
        printf("Pick a command: ");
        scanf("%s",&choose); 
        printf("_________________\n");

        switch (choose) {
            case 'C':
                //Create
                create(running_list, block_list, &storage_number);
                break;
            // case 'Z':
            //     //Display ready queue 
            //     show_running(running_list, 1);
            //     break;
            // case 'X':
            //     //Display block queue 
            //     show_running(block_list, 1);
            //     break;
            case 'K':
                //Kill process by id
                killed(running_list, block_list, &storage_number);
                break;
            case 'E':
                //terminate the process in control of CPU
                ex(running_list, &storage_number);
                break;
            case 'S':
                send(running_list, block_list, &storage_number);
                break;
            case 'R':
                receive(running_list, block_list, &storage_number);
                break;
            case 'Y':
                reply(running_list, block_list, &storage_number);
                break;
            case 'I':
                procinfo(running_list, block_list);
                break;
            case 'T':
                totalinfo(running_list, block_list, &storage_number);
                break;
            case 'Q':
                quantum(running_list, &storage_number);
                break;
            case 'F':
                fo(running_list, &storage_number);
                break;
            case 'N':
                new_sem(sem_list, &sem_num);
                break;
            // case 'U':
            //     sem_info(sem_list);
            //     break;
            case 'P':
                p_sem(running_list, block_list, sem_list, &storage_number);
                break;
            case 'V':
                v_sem(running_list, block_list, sem_list, &storage_number);
                break;
            default:
                printf("Invalid input command\n");
                break;
        }
         
    }
      
    return 0;
}
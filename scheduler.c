#include "headers.h"
//#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
//#define PROJECT_ID 'M'

void RunAlgo(int);
void RunPHPF();
void RunSRTN();
void RunSJF();

void pause_process(struct PCB);
void resume_process(struct PCB);

void MakeOutputFile();

int main(int argc, char * argv[])
{
    initClk();
    printf("from scheduler \n");
    //TODO implement the scheduler :)
    //upon termination release the clock resources


    //setting up stuff here
    //setting up connection 
    key_t process_gen_key , scheduler_process_key;
    int p_gen_qid , sched_qid;
    //process gen scheduer ipc
    if ((process_gen_key = ftok("firstFTOK", 65)) == -1) {
        perror ("ftok");
        exit (1);
    }

    printf("the ftok at scheduler %d \n" , process_gen_key);

    if ((p_gen_qid = msgget (process_gen_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }
    printf("the p_gen_qid at the scheduler is %d \n" , p_gen_qid);

    // //process scheduler ipc
    // if ((scheduler_process_key = ftok (SERVER_KEY_PATHNAME, 'Q')) == -1) {
    //     perror ("ftok");
    //     exit (1);
    // }

    // if ((sched_qid = msgget (scheduler_process_key, 0)) == -1) {
    //     perror ("msgget: server_qid");
    //     exit (1);
    // }
    

    int currentTime = getClk();

    heap_t *PQ_PCBs = (heap_t *)calloc(1, sizeof (heap_t));
    int currentalgo = *argv[1];
    printf("here is the algorithm numeber you %d \n" , currentalgo);

    struct PCB temp_pcb;
    struct message_to_sched incoming_msg;
    while(true){
        printf("i am executing at the loop in sched \n");
        sleep(1);
        if (msgrcv (p_gen_qid, &incoming_msg, sizeof (struct PCB), 0, 0) == -1) {
            perror ("msgrcv");
            //exit (1);
        }else{
            //decide here the decison based on the algortihm choose
            //int x = *argv[0];
            printf("i haev reache here are you happy now \n");
            printf("%d",incoming_msg.proceess_info.runningtime);
            push(PQ_PCBs , 1 , incoming_msg.proceess_info);
            temp_pcb = pop(PQ_PCBs);
            printf("after pushing and poping %d \n" , temp_pcb.runningtime);
        }

    }
    //for shortest job first and other stuff also
    //we weill use a sigchild signal to know that a process terminated then 
    //depending on the beahvour of the selected algortihm we will decide what to do next
    //which is in every case also the same just handle sigchild to get the next process 
    //from the queue

    //also we shuold make a list to keep all process 
    //finally we should make one ipc commiunication like the one that is already made 
    //to recive stuff only for each process 

    //in the middle also we should make a loop depending on the next incoming 

    destroyClk(true);
}

// void RunAlgo(){

// }

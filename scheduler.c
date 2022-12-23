#include "headers.h"
#include <signal.h>
//#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
//#define PROJECT_ID 'M'

void RunAlgo(int);
void RunPHPF();
void RunSRTN();
void RunSJF();

void pause_process(struct PCB);
void run_process(struct PCB);
void handler_sigchild(int);

void MakeOutputFile();
void fetchToPQ();
void recieveProcess();

heap_t *PQ_PCBs; 
int currentalgo;
int p_gen_qid;
int something_running = 0;

//variables needed for output file
int totalTA = 0;
int totalRunningtime = 0;
int num_process = 0;


int main(int argc, char * argv[])
{
    initClk();
    printf("from scheduler \n");
    signal(SIGCHLD  , handler_sigchild);
    PQ_PCBs = (heap_t *)calloc(1, sizeof (heap_t));
    //TODO implement the scheduler :)
    //upon termination release the clock resources


    //setting up stuff here
    //setting up connection 
    key_t process_gen_key , scheduler_process_key;
    // , sched_qid;
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

    
    currentalgo = *argv[1];
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
            //push(PQ_PCBs , 1 , incoming_msg.proceess_info);
            //temp_pcb = pop(PQ_PCBs);
            //printf("after pushing and poping %d \n" , temp_pcb.runningtime);
            fetchToPQ(incoming_msg.proceess_info);
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

void RunAlgo(int num_of_algo){
    if(currentalgo == 1){
        RunPHPF();
    }else if(currentalgo == 2){
        RunSRTN();
    }else{
        RunSJF();
    }
}

void recieveProcess(){
    struct message_to_sched incoming_msg;
    if (msgrcv (p_gen_qid, &incoming_msg, sizeof (struct PCB), 0, 0) == -1) {
            perror ("msgrcv");
            //exit (1);
        }else{
            printf("i haev reache here are you happy now \n");
            printf("%d",incoming_msg.proceess_info.runningtime);
            //printf("after pushing and poping %d \n" , temp_pcb.runningtime);
            fetchToPQ(incoming_msg.proceess_info);
        }
}

void fetchToPQ(struct PCB p){
    if(currentalgo == 1){
        //fetch according to highest prioirty
        push(PQ_PCBs , p.priority , p);
    }else if(currentalgo == 2){
        //fetch according to shortest remainng time
        push(PQ_PCBs , p.remainingtime , p);
    }else{
        //fetch according to shortest running time 
        push(PQ_PCBs , p.runningtime , p);
    }
    
}

//here are the 3 algorithms implementation ------------------
void RunSRTN(){
    struct PCB currentPCB;
    while(true){
        sleep(1);
        recieveProcess();
        if(something_running == 0){
            //here you should add that something happened in the output file
            currentPCB = pop(PQ_PCBs);
            if(currentPCB.specialid == 0){
                printf("there is no process at the current time ");
            }else{
                run_process(currentPCB);
                something_running = 1;
            }
        }
    }
}

//write here the 2 remaining algorithms 
void RunPHPF(){

}

void RunSJF(){
    
}

//-----------------------

//this is an attemt at runprocess if you have something better please insert it sorry for any incoveinvce
//funcitons of stopping , pausing and running a process ---------------
void run_process(struct PCB currentprocess){
    if(currentprocess.FirstTime == true){
        int pid_process;
        char rem_time_char = (char) currentprocess.remainingtime;
        char* p_rem_time_char = &rem_time_char;
        pid_process = fork();
        if(pid_process == -1){
            perror("error in fork");
            exit(1);
        }else if(pid_process == 0){
            char* filepath = "./process.out";
            //printf("i hace reached this part");
            if(execlp(filepath , filepath, p_rem_time_char, NULL) == -1){
                perror("error in exec");
                exit(1);
            }
            currentprocess.FirstRunTime = getClk();
            currentprocess.pid = pid_process;
            //wrtie here that we started a process
        }
    }else{
        if(kill(currentprocess.pid , SIGCONT) == -1){
            perror("error in sending the signal of resuming");
        }
        //write here that we resumed a process
    }
}

void pause_process(struct PCB currentprocess){
    if(kill(currentprocess.pid , 20) == -1){
        perror("error in sending the signal sigstp ");
    }
    //write here that we paused a process
}

void handler_sigchild(int signum){
    something_running = 0;
    //write here in the perf that we finished a process
}
//------------------------------------------------
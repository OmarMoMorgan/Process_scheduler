#include "headers.h"
#include <signal.h>
#include <stdio.h>
//#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
//#define PROJECT_ID 'M'

void RunAlgo(int);
void RunPHPF();
void RunSRTN();
void RunSJF();

void pause_process(struct PCB*);
void run_process(struct PCB*);
void handler_sigchild(int);

void MakeOutputFile();
void fetchToPQ(struct PCB*);
int recieveProcess();

heap_t *PQ_PCBs; 
int currentalgo;
int p_gen_qid;
int something_running = 0;

//variables needed for output file
//int totalTA = 0;
//int totalRunningtime = 0;
int num_process = 0;
FILE *fileptr_log;

struct PCB *current_pro_ptr;

void handler_end(int);

int main(int argc, char * argv[])
{
    initClk();
    printf("from scheduler \n");
    signal(SIGCHLD  , handler_sigchild);
    signal(SIGINT , handler_end);
    PQ_PCBs = (heap_t *)calloc(1, sizeof (heap_t));
    //TODO implement the scheduler :)
    //upon termination release the clock resources

    fileptr_log = fopen("scheduler.log" , "w");
    fileptr_stats = fopen("scheduler.perf" , "w");
    fprintf(fileptr_log , "#At \t time \t x \t process \t y \t state \t arr \t w \t total \t z \t remain \t y \t wait \t k \n");
    //setting up stuff here
    //setting up connection 
    key_t process_gen_key , scheduler_process_key;
    // , sched_qid;
    //process gen scheduer ipc
    if ((process_gen_key = ftok("firstFTOK", 65)) == -1) {
        perror ("ftok");
        exit (1);
    }

    //printf("the ftok at scheduler %d \n" , process_gen_key);

    if ((p_gen_qid = msgget (process_gen_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }
    //printf("the p_gen_qid at the scheduler is %d \n" , p_gen_qid);

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


//this part was only made fro debuging it should be delted later ---------------
    // struct PCB temp_pcb;
    // struct message_to_sched incoming_msg;
    // while(true){
    //     printf("i am executing at the loop in sched \n");
    //     sleep(1);
    //     if (msgrcv (p_gen_qid, &incoming_msg, sizeof (struct PCB), 0, 0) == -1) {
    //         perror ("msgrcv");
    //         //exit (1);
    //     }else{
    //         //decide here the decison based on the algortihm choose
    //         //int x = *argv[0];
    //         printf("i haev reache here are you happy now \n");
    //         printf("%d",incoming_msg.proceess_info.runningtime);
    //         //push(PQ_PCBs , 1 , incoming_msg.proceess_info);
    //         //temp_pcb = pop(PQ_PCBs);
    //         //printf("after pushing and poping %d \n" , temp_pcb.runningtime);
    //         fetchToPQ(incoming_msg.proceess_info);
    //     }
    //
    //}
    //for shortest job first and other stuff also
    //we weill use a sigchild signal to know that a process terminated then 
    //depending on the beahvour of the selected algortihm we will decide what to do next
    //which is in every case also the same just handle sigchild to get the next process 
    //from the queue

    //also we shuold make a list to keep all process 
    //finally we should make one ipc commiunication like the one that is already made 
    //to recive stuff only for each process 

    //in the middle also we should make a loop depending on the next incoming 
    //-----------------------------------------------

    RunAlgo(currentalgo);

    //destroyClk(true);
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

int recieveProcess(){
    //0 for reciveing nothing 1 for reciveing something
    struct message_to_sched incoming_msg;
    //printf("recieve function is called \n");
    if (msgrcv (p_gen_qid, &incoming_msg, sizeof (struct PCB), 0, IPC_NOWAIT) == -1) {
            //perror ("msgrcv");
            //exit (1);
            //printf("filaure here at the recieve process");
            if(errno != ENOMSG){
                perror("reciving message failure");
                exit(1);
            }
            return 0;
        }else{
            printf("i haev reache here are you happy now \n");
            printf("%d",incoming_msg.proceess_info.runningtime);
            //printf("after pushing and poping %d \n" , temp_pcb.runningtime);
            fetchToPQ(&incoming_msg.proceess_info);
            return 1;
        }
}

void fetchToPQ(struct PCB *p){
    if(currentalgo == 1){
        //fetch according to highest prioirty
        push(PQ_PCBs , p->priority , *p);
        printf("pushing to the oq done sucesffuly \n");
    }else if(currentalgo == 2){
        //fetch according to shortest remainng time
        push(PQ_PCBs , p->remainingtime , *p);
    }else{
        //fetch according to shortest running time 
        push(PQ_PCBs , p->runningtime , *p);
    }
    
}

//here are the 3 algorithms implementation ------------------
void RunSRTN(){
    struct PCB currentPCB;
    struct PCB comparingPCB;
    current_pro_ptr = &currentPCB;
    while(true){
        int recieved = recieveProcess();
        if(something_running == 0)
        {
            currentPCB = pop(PQ_PCBs);
            if(currentPCB.specialid == 0){
                printf("there is no process at the current time second algo\n");
            }
            else
            {
                run_process(&currentPCB);
                something_running = 1;
            }
        }
        if(recieved == 1 && something_running == 1)
        {
            comparingPCB = pop(PQ_PCBs);
            if(comparingPCB.specialid != 0){
                if(comparingPCB.remainingtime < currentPCB.remainingtime)
                {
                    pause_process(&currentPCB);
                    fetchToPQ(&currentPCB);
                    currentPCB = comparingPCB;
                    run_process(&currentPCB);
                    something_running = 1;
                }else{
                    fetchToPQ(&comparingPCB);
                }
            }
        }
        sleep(1);
    }
}


//write here the 2 remaining algorithms 
void RunPHPF(){
    struct PCB currentPCB;
    struct PCB comparingPCB;
    current_pro_ptr = &currentPCB;
    while(true){
        //printf("start of the while loop \n");
        int recieved = recieveProcess();
        if(something_running == 0)
        {
            currentPCB = pop(PQ_PCBs);
            if(currentPCB.specialid == 0){
                printf("there is no process at the current time \n");
            }
            else
            {
                run_process(&currentPCB);
                printf("at this part after run process \n");
                something_running = 1;
            }
        }
        if(recieved == 1 && something_running == 1)
        {
            //printf("after the recieved condition \n");
            comparingPCB= pop(PQ_PCBs);
            printf("comparing PCB adata %d\n" , comparingPCB.specialid);
            if(comparingPCB.specialid != 0){
                if(comparingPCB.priority < currentPCB.priority)
                {
                    printf("before the pause \n");
                    pause_process(&currentPCB);
                    printf("test after pause \n");
                    //if())
                    printf("the process about to be pusehd %d" , currentPCB.specialid);
                    fetchToPQ(&currentPCB);
                    currentPCB = comparingPCB;
                    run_process(&currentPCB);
                    something_running = 1;
                }
                else{
                    fetchToPQ(&comparingPCB);
                }
            }
        }
        //printf("last part in the if condition \n");
        sleep(1);
    }
}


void RunSJF(){
    struct PCB currentPCB;
    current_pro_ptr = &currentPCB;
    while(true){
        
        int recivedsometing = recieveProcess();
        if(something_running == 0){
            //here you should add that something happened in the output file
            currentPCB = pop(PQ_PCBs);
            if(currentPCB.specialid == 0){
                printf("no process at the current time \n");
            }else{
                run_process(&currentPCB);
                something_running = 1;
            }
        }
        sleep(1);
    }
}

//-----------------------

//this is an attemt at runprocess if you have something better please insert it sorry for any incoveinvce
//funcitons of stopping , pausing and running a process ---------------
void run_process(struct PCB *currentprocess){
    if(currentprocess->FirstTime == true){
        int pid_process;
        char rem_time_char = (char) currentprocess->remainingtime;
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
            
            
            //wrtie here that we started a process LOG
        }else{
            currentprocess->pid = pid_process;
            printf("the process id is %d \n" , pid_process);
            currentprocess->FirstRunTime = getClk();
            
            currentprocess->FirstTime = false;
            currentprocess->runningstatus = 2;
            printf("the process id is this time from currentprocess%d \n" , currentprocess->pid);
            fprintf(fileptr_log , "At \t time \t %d \t process %d \t started \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , 0);
            printf("At \t time \t %d \t process %d \t started \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , 0);

            //printf("after running thisi process for the first time it is time to celeberate \n");
        }
        
    }else{
        if(kill(currentprocess->pid , SIGCONT) == -1){
            perror("error in sending the signal of resuming");
        }
        currentprocess->runningstatus = 2;
        //currentprocess->lastStartTime = getClk();
        //write here that we resumed a process LOG
        fprintf(fileptr_log , "At \t time \t %d \t process %d \t resumed \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , getClk()-currentprocess->lastStartTime);
        printf("At \t time \t %d \t process %d \t resumed \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , getClk()-currentprocess->lastStartTime);

    }
    currentprocess->lastStartTime = getClk();
    printf("running a rocess was done succ \n");
}

void pause_process(struct PCB *currentprocess){
    //struct currentprocess = *
    //printf("the process about to  be stopped %d \n"  , currentprocess->pid);
    printf("the process about to  be stopped special is %d \n"  , currentprocess->specialid);
    printf("I am here at the puase start of function \n");
    printf("the process about to  be stopped %d \n" , currentprocess->pid);
    if(kill(currentprocess->pid , SIGSTOP) == -1){
        perror("error in sending the signal sigstp ");
    }
    currentprocess->remainingtime = currentprocess->remainingtime - (getClk() - currentprocess->lastStartTime);
    //write here that we paused a process
    fprintf(fileptr_log , "At \t time \t %d \t process %d \t paused \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , 0);
    printf("At \t time \t %d \t process %d \t paused \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \n" , getClk() , currentprocess->specialid , currentprocess->arrivaltime , currentprocess->runningtime , currentprocess->remainingtime , 0);
    printf("I am here at the puase \n");
}

void handler_sigchild(int signum){
    
    printf("sigchild is running now will it fail \n");
    //write here in the perf that we finished a process
    struct PCB currentprocess = *current_pro_ptr;
    //printf("line 2 \n");
    int TA_for_current = getClk() - currentprocess.arrivaltime;
    int WTA_for_current = TA_for_current/currentprocess.runningtime;
    //printf("line 3 \n");
    current_pro_ptr->remainingtime = current_pro_ptr->remainingtime - (getClk() - current_pro_ptr->lastStartTime);
    printf("the remain time is %d\n" , current_pro_ptr->remainingtime);
    if(current_pro_ptr->remainingtime <= 0){
    fprintf(fileptr_log , "At \t time \t %d \t process %d \t finished \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \t TA \t %d \t WTA \t %d \n" , getClk() , currentprocess.specialid , currentprocess.arrivaltime , currentprocess.runningtime , 0 , 0 , TA_for_current , WTA_for_current);
    printf("At \t time \t %d \t process %d \t finished \t arr %d \t total \t %d \t remain \t %d \t wait \t %d \t TA \t %d \t WTA \t %d \n" , getClk() , currentprocess.specialid , currentprocess.arrivaltime , currentprocess.runningtime , currentprocess.remainingtime , 0 , TA_for_current , WTA_for_current);

    printf("it has finished running byee i was %d\n" , currentprocess.pid);
    something_running = 0;
    Total_WTA +=  WTA_for_current;
    totalTA += (currentprocess.FirstRunTime - currentprocess.arrivaltime );
    totalRunningtime += currentprocess.runningtime;
    each_WA_time[out_c] = WTA_for_current;
    out_c += 1 ;
    }else{
        //fetchToPQ(current_pro_ptr);
    }
}

void handler_end(int signum1){
    fclose(fileptr_log);
    fclose(fileptr_stats);
    //destroyClk(true);
    //printf("the p_gen_qid at the msgctl is %d \n" , p_gen_qid);
    // if (msgctl (p_gen_qid, IPC_RMID, NULL) == -1) {
    //         perror ("from the scheduler: msgctl");
    //         exit (1);
    // }
    measure_state(fileptr_stats);
    printf("Feeling that the end of our time is near \n");
    exit(1);
}
//------------------------------------------------
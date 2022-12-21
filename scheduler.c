#include "headers.h"
//#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
//#define PROJECT_ID 'M'

int main(int argc, char * argv[])
{
    initClk();
    
    //TODO implement the scheduler :)
    //upon termination release the clock resources


    //setting up stuff here
    //setting up connection 
    key_t process_gen_key , scheduler_process_key;
    int p_gen_qid , sched_qid;
    //process gen scheduer ipc
    if ((process_gen_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((p_gen_qid = msgget (process_gen_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }

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

    heap_t Shortesjobfirst;

    struct message_to_sched incoming_msg;
    while(true){
        if (msgrcv (p_gen_qid, &incoming_msg, sizeof (struct PCB), 0, 0) == -1) {
            perror ("msgrcv");
            exit (1);
        }else{
            //decide here the decison based on the algortihm choose
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

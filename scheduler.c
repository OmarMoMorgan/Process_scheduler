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

    //process scheduler ipc
    if ((scheduler_process_key = ftok (SERVER_KEY_PATHNAME, 'Q')) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((sched_qid = msgget (scheduler_process_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }
    
    destroyClk(true);
}

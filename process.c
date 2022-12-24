#include "headers.h"
//#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
//#define PROJECT_ID 'M'

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    
    // key_t scheduler_process_key;
    // int myqid ,sched_qid;

    // //process scheduler ipc
    // if ((scheduler_process_key = ftok (SERVER_KEY_PATHNAME, 'Q')) == -1) {
    //     perror ("ftok");
    //     exit (1);
    // }

    // if ((sched_qid = msgget (scheduler_process_key, 0)) == -1) {
    //     perror ("msgget: server_qid");
    //     exit (1);
    // }

    // if ((myqid = msgget (IPC_PRIVATE, 0666)) == -1) {
    //     perror ("msgget: myqid");
    //     exit (1);
    // }


    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    // while (remainingtime > 0)
    // {
    //     // remainingtime = ??;
    // }
    int start = getClk();
    printf("From the process File %d at %d\n" , getpid() , getClk());
    char c = *argv[1];
    int m = (int)c;
    int now = getClk();
    //remainingtime = m - (now - start);
    remainingtime = m;
    while(remainingtime>0)
    {
        
        //now = getClk();
        int othervar = getClk();
        if(othervar > now){
            now = now +1;
            remainingtime = remainingtime -1;
        }
        //remainingtime = m - (now - start);
        //printf("the remaining time for this process %d \n" ,remainingtime);
    }
    exit(2);
    
    destroyClk(false);
    
    return 0;
}

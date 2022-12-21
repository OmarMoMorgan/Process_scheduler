#include "headers.h"
#include <stddef.h>

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'

void clearResources(int);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    // 4. Use this function after creating the clock process to initialize clock
    

    //reading input files:
    struct PCB currentprocess;
    char temp;
    //scanf("%s", &temp);

    //sitting up ipc communications between scheduler and process generator
    
    key_t process_gen_key;
    int p_gen_qid;
    if ((process_gen_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }
    //printf("\n %d" , process_gen_key);

    if ((p_gen_qid = msgget (process_gen_key, 0)) == -1) {
        perror ("msgget: server_qid frmo the process generator ");
        exit (1);
    }

    FILE *in_file  = fopen("processes.txt", "r");
    char line[100]; 
          

    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources


    struct message_to_sched Currentmsg;
    while ( fgets( line, 100, in_file ) != NULL ) 
            { 
              //printf("inside the lopo \n");
              //printf("The line is: %s\n", line); 
              if(line[0] != '#'){
                //int x = (int) line[2];
                int c ,y,z,q;
                sscanf(line,"%d  %d  %d  %d" , &c ,&y ,&z ,&q);
                
                while(y > x){
                    sleep(y-x);
                }
                currentprocess.arrivaltime = y;
                currentprocess.runningtime = z;
                currentprocess.priority = q;
                currentprocess.specialid = c;
                
                Currentmsg.proceess_info = currentprocess;
                Currentmsg.message_type = 1;

                if (msgsnd (p_gen_qid, &Currentmsg, sizeof (struct PCB), 0) == -1) {
                    perror ("client: msgsnd");
                    exit (1);
                }       
                

                //printf("%d \t %d \t %d \t %d \n" , x ,y ,z ,q);
                //printf(" end is here \n");
              }
            } 



    destroyClk(true);
    // for(int i =0 ; i < 5;i++){
    //     printf("%d\n" ,x);
    // }
    if (msgctl (p_gen_qid, IPC_RMID, NULL) == -1) {
            perror ("client: msgctl");
            exit (1);
    }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}

#include "headers.h"
#include <stddef.h>

#define SERVER_KEY_PATHNAME "/tmp/mqueue_server_key"
#define PROJECT_ID 'M'

void clearResources(int);

void startScheduler(int);

void startClock();

void SendToSched();
int p_gen_qid;

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

    

    //-----sitting up ipc communications between scheduler and process generator----
    
    key_t process_gen_key;
    
    if ((process_gen_key = ftok("firstFTOK", 65)) == -1) {
        perror ("ftok");
        exit (1);
    }
    

    printf("\n %d \n" , process_gen_key);

    if ((p_gen_qid = msgget(process_gen_key,  IPC_CREAT | 0666)) == -1) {
        perror ("msgget: server_qid frmo the process generator ");
        exit (1);
    }
    //---------------------------------

    //------------------this part should be deleted and commented in case something goes wrong
    // if (msgctl (p_gen_qid, IPC_RMID, NULL) == -1) {
    //         perror ("client: msgctl");
    //         exit (1);
    // }
    // exit(1);
    //--------------------------
    
    //-------------------getting the type of algorithm needed and starting the scheduler
    int algo;
    //rename them as text file 1 for , 2 for , 3 for 
    printf("enter the algorithim number that you want");
    scanf("%d",&algo);
    
    

    //starrting the clock process ------------
    startClock();
    startScheduler(algo);
    //-------------
    
    //       destroyClk(true);
    // // for(int i =0 ; i < 5;i++){
    // //     printf("%d\n" ,x);
    // // }
    // if (msgctl (p_gen_qid, IPC_RMID, NULL) == -1) {
    //         perror ("client: msgctl");
    //         exit (1);
    // }

    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    //printf("the p_gen_qid at the pgen is %d \n" , p_gen_qid);


    struct message_to_sched Currentmsg;
    //----------Reading the text File-------------
    FILE *in_file  = fopen("processes.txt", "r");
    char line[100]; 
    while ( fgets( line, 100, in_file ) != NULL ) 
            { 
              //printf("inside the lopo \n");
              //printf("The line is: %s\n", line); 
              if(line[0] != '#'){
                //int x = (int) line[2];
                int c ,y,z,q;
                sscanf(line,"%d  %d  %d  %d" , &c ,&y ,&z ,&q);
                
                while(y > x){
                    //printf("I am  stuck here at y > x \n");
                    sleep(y-x);
                    x = getClk();
                }
                currentprocess.arrivaltime = y;
                currentprocess.runningtime = z;
                currentprocess.remainingtime = z;
                currentprocess.priority = q;
                currentprocess.specialid = c;
                currentprocess.FirstTime = true;
                
                Currentmsg.proceess_info = currentprocess;
                Currentmsg.message_type = 1;

                if (msgsnd (p_gen_qid, &Currentmsg, sizeof (struct PCB), 0) == -1) {
                    perror ("client: msgsnd error is from this line 102");
                    exit (1);
                }    
                //printf("i am executnig actullay here at the process generator");
                

                //printf("%d \t %d \t %d \t %d \n" , x ,y ,z ,q);
                //printf(" end is here \n");
              }
            } 
    //---------------------------------------

    //sleep(5);
    wait(NULL);
    destroyClk(true);
    // for(int i =0 ; i < 5;i++){
    //     printf("%d\n" ,x);
    // }
    // if (msgctl (p_gen_qid, IPC_RMID, NULL) == -1) {
    //         perror ("error in destruction");
    //         exit (1);
    // }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    destroyClk(true);
    printf("the p_gen_qid at the msgctl is %d \n" , p_gen_qid);
    if (msgctl (p_gen_qid, IPC_RMID, NULL) == -1) {
            perror ("from the pgen: msgctl");
            exit (1);
    }
    exit(1);
}

void startScheduler(int algonum){
    int pid_scheduler;
    pid_scheduler = fork();
    if(pid_scheduler == -1){
        perror("error in fork");
        exit(1);
    }else if(pid_scheduler == 0){
        //startScheduler(algo);
        //printf(":ihave reached this part in the scheduler \n");
        char* filepath = "./scheduler.out";
        char algochar = (char) algonum;
        char* p_algo_char = &algochar;
        if(execlp(filepath , filepath,p_algo_char , NULL) == -1){
            perror("error in exec of sched");
            exit(1);
        }
    }
    
}

void startClock(){
    int pid_clock;
    pid_clock = fork();
    if(pid_clock == -1){
        perror("error in fork");
        exit(1);
    }else if(pid_clock == 0){
        char* filepath = "./clk.out";
        //printf("i hace reached this part");
        if(execlp(filepath , filepath,NULL) == -1){
            perror("error in exec");
            exit(1);
        }
    }
}

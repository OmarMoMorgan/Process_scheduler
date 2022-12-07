#include "headers.h"
#include <stddef.h>

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
    FILE *in_file  = fopen("processes.txt", "r");
    char line[100]; 
          while ( fgets( line, 100, in_file ) != NULL ) 
            { 
              //printf("inside the lopo \n");
              //printf("The line is: %s\n", line); 
              if(line[0] != '#'){
                //int x = (int) line[2];
                int x ,y,z,q;
                sscanf(line,"%d  %d  %d  %d" , &x ,&y ,&z ,&q);
                
                //printf("%d \t %d \t %d \t %d \n" , x ,y ,z ,q);
                //printf(" end is here \n");
              }
            } 

    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
    // for(int i =0 ; i < 5;i++){
    //     printf("%d\n" ,x);
    // }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}

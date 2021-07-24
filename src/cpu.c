#include<string.h>
#include"ram.h"
#include"interpreter.h"
#include"shell.h"

#define DEFAULT_QUANTA 2

struct CPU { 
    int IP; 
    int offset;
    char IR[1000]; 
    int quanta; 
} CPU;

/*
This method passes a quanta.
It will then executes quanta number of lines in RAM as from the address
pointed by the CPU IP.
Returns an errorCode.
*/
int run(int quanta){
    //printf("in cpu\n");
    //printf("in cpu run\n");
    // If a quanta greater than the default quanta of 2 is passed,
    // run will instead execute only default quanta of lines.
    quanta=1;
    strcpy(CPU.IR,ram[CPU.IP*4+CPU.offset]);
    //printf("in cpu instruct is %s",CPU.IR);
        int errorCode = 0;
        if((strcmp(CPU.IR,"padding\n")!=0)){
            //printf("in cpu if statement\n");
            errorCode=parse(CPU.IR);
            }
        
        
        // Do error checking and if error, return error
        if (errorCode != 0){
            // Display error message if fatal
            if (errorCode < 0){
                ram[CPU.IP][strlen(ram[CPU.IP])-2]='\0';
                displayCode(errorCode,ram[CPU.IP]);
            }
            return errorCode;
        }
        
    
    return 0;
}

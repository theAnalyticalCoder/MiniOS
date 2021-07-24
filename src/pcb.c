#include <stdio.h>
#include <stdlib.h>
/*
PCB has 3 fields
PC : Points to the the current line reached for that program
start: Points to first line of that program
end: Points to last line of that program
*/
typedef struct PCB
{
    int PC;
    int PC_page;
    int PC_offset, pages_max;
    int pageTable[10];
}PCB;


/*
Passes 2 parameters (start , end)
This method creates a PCB with fields set as this:
PC = start
start = start
end = end
*/
PCB* makePCB(int PC, int pages_max, int pageTable[]){
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    pcb->PC=PC;
    pcb->PC_page = 0;
    pcb->PC_offset = 0;
    pcb->pages_max = pages_max;
    for (int i=0;i<10;i++){
        if (i<pages_max){
           pcb->pageTable[i]= pageTable[i];
        }
        else{
            pcb->pageTable[i]= -1;
        }
    }
    return pcb;
}



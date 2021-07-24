#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include"shell.h"
#include"pcb.h"
#include"ram.h"
#include"cpu.h"
#include"interpreter.h"
#include "memorymanager.h"

/*
This is a node in the Ready Queue implemented as 
a linked list.
A node holds a PCB pointer and a pointer to the next node.
PCB: PCB
next: next node
*/
void boot();
int kernel();
int size();
typedef struct ReadyQueueNode {
    PCB*  PCB;
    struct ReadyQueueNode* next;
} ReadyQueueNode;

ReadyQueueNode* head = NULL;
ReadyQueueNode* tail = NULL;
int sizeOfQueue = 0;

int main(int argc, char const *argv[])
{
    int error=0;
    boot(); // First : actions performed by boot 
    error = kernel(); // Second: actions performed by kernel 
    remove("BackingStore\\storage.txt");
    return error;
    
}
void boot(){
    
    //first sets ram to Null
    removeFromRam(0,39);
    remove("BackingStore\\storage.txt");
    //rmdir("BackingStore");
    
    mkdir("BackingStore");
}
int kernel(){
    storage[0]=0;
    return shellUI();
}
/*
Adds a pcb to the tail of the linked list
*/
void addToReady(struct PCB* pcb) {
    ReadyQueueNode* newNode = (ReadyQueueNode *)malloc(sizeof(ReadyQueueNode));
    newNode->PCB = pcb;
    newNode->next = NULL;
    if (head == NULL){
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    sizeOfQueue++;
}
void updateVictim(int frame){
    int sizeOfReady=size();
    ReadyQueueNode * temp = head;
    int found=0;
    for(int i=0;i<sizeOfReady;i++){
        
        for(int j=0; j<10; j++){
            if(temp->PCB->pageTable[j]==frame){
                temp->PCB->pageTable[j]=-1;
                found=1;
                //printf("found\n");
                break;
        }
        }
        if(found){break;}
        temp=temp->next;
    }
    
    
    

}

/*
Returns the size of the queue
*/
int size(){
    return sizeOfQueue;
}

/*
Pops the pcb at the head of the linked list.
pop will cause an error if linkedlist is empty.
Always check size of queue using size()
*/
struct PCB* pop(){
    PCB* topNode = head->PCB;
    ReadyQueueNode * temp = head;
    if (head == tail){
        head = NULL;
        tail = NULL;
    } else {
        head = head->next;
    }
    free(temp);
    sizeOfQueue--;
    return topNode;
}

/*
Passes a filename
Opens the file, copies the content in the RAM.
Creates a PCB for that program.
Adds the PCB on the ready queue.
Return an errorCode:
ERRORCODE 0 : NO ERROR
ERRORCODE -3 : SCRIPT NOT FOUND
ERRORCODE -5 : NOT ENOUGH RAM (EXEC)
*/
int myinit(char* filename){
    // Open the filename to get FILE *
    // call addToRam on that File *
    // If error (check via start/end variable), return that error
    // Else create pcb using MakePCB
    // Then add it to the ReadyQueue
    FILE * fp = fopen(filename,"r");
    if (fp == NULL) return -3;
    int start;
    int end;
    addToRAM(fp,&start,&end);
    fclose(fp);
    if (start == -1) return -5;
    //PCB* pcb = makePCB(start,end);
    //addToReady(pcb);
    return 0;
}

int scheduler(){
    // set CPU quanta to default, IP to -1, IR = NULL
    //printf("in scheduler\n");
    
    CPU.quanta = DEFAULT_QUANTA;
    CPU.IP = -1;
    
    CPU.offset=0;
    while (size() != 0){
        //pop head of queue
        PCB* pcb = pop();
        int storagePageNum=0;
        int frame=0;
        //copy PC of PCB to IP of CPU
        //first check if page is loaded
        if (pcb->pageTable[pcb->PC_page]==-1){

            storagePageNum=storage[pcb->PC]+pcb->PC_page;
            frame=FindFrame();
            FILE *fp2 = fopen ("BackingStore\\storage.txt","r");
            if(frame>-1){
                loadPage(storagePageNum,fp2,frame);
                fclose(fp2);
                updatePageTable(pcb, pcb->PC_page, frame, -1);
                } 
                            //shouldnt happen
            else{
                            //first find actual pagenumber
                frame=findVictim(pcb);
                updatePageTable(pcb, pcb->PC_page, frame, frame);
                loadPage(storagePageNum,fp2,frame);
                fclose(fp2);
                        }
                 
         }
        else{
        CPU.IP = pcb->pageTable[pcb->PC_page];
        CPU.offset=pcb->PC_offset;

        int isOver = FALSE;
        for (int i=0;i<CPU.quanta;i++){
            //printf("about to run\n");
            if(pcb->PC==2){
                int a =0;
            }
            int errorCode = run(1);
            CPU.offset++;
            pcb->PC_offset++;
            //page fault
            if (CPU.offset==4){
                CPU.offset=0;
                pcb->PC_offset=0;
                //removeFromRam(CPU.IP,CPU.IP+3);
                pcb->PC_page++;
                if (pcb->PC_page==pcb->pages_max)
                {   free(pcb);
                 isOver=TRUE;}
                else{
                    if (pcb->pageTable[pcb->PC_page]==-1){
                        //offset in file+
                         storagePageNum=storage[pcb->PC]+pcb->PC_page;
                         frame=FindFrame();
                        FILE *fp2 = fopen ("BackingStore\\storage.txt","r");
                        if(frame>-1){
                            loadPage(storagePageNum,fp2,frame);
                            fclose(fp2);
                            updatePageTable(pcb, pcb->PC_page, frame, -1);
                            } 
                            //shouldnt happen
                        else{
                            //first find actual pagenumber
                            frame=findVictim(pcb);
                            updatePageTable(pcb, pcb->PC_page, frame, frame);
                            loadPage(storagePageNum,fp2,frame);
                            fclose(fp2);
                            }
                    }
                    else{
                        CPU.IP=pcb->pageTable[pcb->PC_page];
                        
                    }
                    
                    break;
                }
                
            }
            
        }
        if(isOver==FALSE){ addToReady(pcb);}
       
        }
        
    }
    // reset RAM
    
    remove("BackingStore\\storage.txt");
    FILE *fp3 = fopen ("BackingStore\\storage.txt","w");
    fclose(fp3);
    resetStorage();
    removeFromRam(0,39);
    return 0;
}

/*
Flushes every pcb off the ready queue in the case of a load error
*/
void emptyReadyQueue(){
    while (head!=NULL){
        ReadyQueueNode * temp = head;
        head = head->next;
        free(temp->PCB);
        free(temp);
    }
    sizeOfQueue =0;
}


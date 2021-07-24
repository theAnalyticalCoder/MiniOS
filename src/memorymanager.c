#include<stdio.h>
#include<stdlib.h>
#include <unistd.h> 
#include <fcntl.h> 
#include<string.h>
#include "ram.h"
//#include "pcb.h"
#include "kernel.h"
int countTotalPages(FILE *p);
void loadPage(int pageNumber, FILE *f, int frameNumber);
int findVictim(PCB *p);
int FindFrame();
int isPresent(int length,int pageTable[], int frame);
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);
int storage[4];
int numPrograms=1;
int launcher(FILE *p){
    
    
    //printf("in launcher launching the %d prog starting at %d\n",numPrograms,storage[numPrograms-1] );
    int i=0;
    char buffer[1000];
    FILE * fp;
    
    int numPages=countTotalPages(p);
    //printf("num pages%d\n",numPages);\
    //gives the correct placement of each program in the storage
    storage[numPrograms]=numPages+storage[numPrograms-1];
    
    rewind(p);
   /* open the file for writing*/
    fp = fopen ("BackingStore\\storage.txt","a");
    int numberOflines=0;
    while (fgets(buffer, sizeof(buffer), p) != NULL ){
        //fgets(buffer,999,p);
        numberOflines++;
        //printf("sending %s",buffer);
        fprintf (fp, "%s",buffer);
    }
    int emptylinesToWrite=numPages*4-numberOflines;
    for(int j=0;j<emptylinesToWrite;j++){
        fprintf (fp, "padding\n");
    }
    //creating pcb and loading into Ram
    //calculating page number for program
    int frame=0;
    fclose(fp);
    FILE *fp2 = fopen ("BackingStore\\storage.txt","r");
    int pageTable[10];
    for(int i=0;i<10;i++){
        pageTable[i]=-1;
    }
    for(int i=0;i<2&&i<numPages;i++){
        
       frame=FindFrame();
       //printf("the frame is %d\n",frame);
       if(frame>-1){
           rewind(fp2);
           loadPage(storage[numPrograms-1]+i,fp2,frame);
           pageTable[i]=frame;
       } 
       //shouldnt happen
       else{
           int a=3;

       }
    }
    PCB* pcb =makePCB(numPrograms-1,numPages,pageTable);
    addToReady(pcb);
    numPrograms++;
    fclose(p);
    fclose(fp2);
    
}
int countTotalPages(FILE *p){
    char buffer[1000];
    int numLines=0;
    int numPages=0;
    while (fgets(buffer, sizeof(buffer), p) != NULL ){
        numLines++;
    }
    
    if(numLines%4==0){
        return numLines/4;
    }
    else{
        return numLines/4+1;
    }
    
}
void loadPage(int pageNumber, FILE *p, int frameNumber){
    int counter=0;
    char buffer[1000];
    int locationInRam=frameNumber*4;
    if(frameNumber>9||frameNumber<0){
        printf("error frame number is wrong");
    }
    //printf("load page page number %d frame number%d\n",pageNumber,frameNumber);
    while (fgets(buffer, sizeof(buffer), p) != NULL &&  counter<pageNumber*4+4){
        //printf("in while loop counter is %d\n",counter);
        //printf("load the buffer is %s",buffer);
        if(counter>=pageNumber*4){
            //printf("load putting into ram %s",buffer);
            ram[locationInRam]= strdup(buffer);
            locationInRam++;
        }
        counter++;
    }
    if(counter!=pageNumber*4+4){
        printf("error in getting pages load page counter is%d\n",counter);
    }
}
int FindFrame(){
    for(int i=0;i<40;i+=4){
        if(ram[i]==NULL){
            return i/4;
        }
    }
    return -1;
}
int findVictim(PCB *p){
    int victim=rand() % 10;
    int counter=0;
    while(counter<10){
        int answer=isPresent(10,p->pageTable,victim);
        if (answer>-1){
            return victim;
        }
        victim=(victim+1)%10;
        counter++;
    }
    printf("error all frames taken by a single program");
    return victim;

}
int isPresent(int length,int pageTable[], int frame){
    for(int i=0; i<length; i++){
        if(pageTable[i]==frame){
            return -1;
        }
    }
    return frame;
}
int updatePageTable(PCB *pcb, int pageNumber, int frameNumber, int victimFrame){
    pcb->pageTable[pageNumber]=frameNumber;
    //now need to search ready queue for corresponding victim frame
    if(victimFrame!=-1){
        updateVictim(victimFrame);
    }
    return 0;
}
void resetStorage(){//need to reset fileaswell

    for(int i=0;i<4;i++){
        storage[i]=0;
    }
    numPrograms=1;
}
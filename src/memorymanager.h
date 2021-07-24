int launcher(FILE *p);
int countTotalPages(FILE *p);
void loadPage(int pageNumber, FILE *f, int frameNumber);
int findVictim(PCB *p);
int FindFrame();
int isPresent(int length,int pageTable[], int frame);
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);
void resetStorage();
extern int storage[4];
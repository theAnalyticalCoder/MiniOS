# MiniOS
The Aim of this project is Model a Linux operating system,  
Shell.c unsuprisingly models the shell of an operating system. 
It can handle several commands such as saving and displaying variables and running scripts. It parses the command line arguments by using
interpreter.c and storing anything it needs to save into a cache, shellmemory.c.  
    
Furthermore this project simulates the linux runtime environment.
I.e many programs are too big to be stored in the ram let alone the cache of a computer. An operating system must make decisions on what to store in Ram.
It does this by implementing a ReadyQueue (FIFO) of Process Control Block (PCB.c).  
    
Each PCB points to the page, modelled as 4 lines of code,where a program starts in RAM, where it ends and where it is at currently.
The CPU (Cpu.c) will run for 2 quanta excuting the program pointed to by the PCB. It is very likely and infact often that a page fault will occur.  
  
The Page table only contains 10 pages i.e 40 lines of code. I.e if we run two programs that are 30 lines each we will only be able to store 20 lines each in RAM (or 10 PCB in the ready Queue).
Thats where memorymanager.c comes in.  
  
It implements task switching when the PCB runs out of code implement a text file where all the lines of code for each program exist (i.e the Hard Drive on a normal computer)
When the CPU page faults it queries the memorymanager to determine what PCB to get rid of (the victim) in Ram. The vicitm PCB is removed and new code is loaded (task switch) from the Backing Store and the
CPU continues until all its quantas are used 

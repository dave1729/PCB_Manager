#ifndef PCB_H
#define PCB_H

//Max PC must be at least 48
#define MAX_PC 100

//Max PC must be at least 48
#define MAX_TERM_COUNT 5
#include <time.h>

typedef struct cust_mutex* cust_mutex_p;
/*
 ===========================================================================
 Name        : 422_HW3.c
 Date        : 2/12/2016
 Authors     : Nok Him Nam, Jowy Tran, Mark Johnson, David Humphreys
 Description : Discontinuities with I/O service request traps and interrupts
 ===========================================================================
 */

enum state_type {
	created,
	ready,
	running,
	blocked,
	interrupted,
	terminated
};

typedef enum state_type State;

enum pcb_type {
	regular,
	idle,
	vanilla,
	producer,
	consumer,
        mutual_resource_A,
	mutual_resource_B
};

typedef enum pcb_type Type;

struct PCB {
	char name[20];
	int pid;
	int priority;
        int tempPriority;
	//int interrupt_simulator;
	unsigned int pc;
        int pcb_data[10];
	State currentState;
        Type type;
        int max_pc; //jowy added
	char* creation; //jowy renamed to match diagram
	char* termination; //jowy renamed to match diagram
	int terminate;
	int term_count; //jowy renamed to match diagram
        int runTimes;
	int IO_1_traps[4]; //jowy renamed to match diagram
	int IO_2_traps[4]; //jowy renamed to match diagram
        int arrayR1Lock[4];
        int arrayR1Unlock[4];
        int arrayR2Lock[4];
        int arrayR2Unlock[4];
        cust_mutex_p mutexR1;
        cust_mutex_p mutexR2;
};

typedef struct PCB* PCB_p;

PCB_p createMutexPCB();
PCB_p createPCB(char* name, int newPid);
PCB_p createOneRandomPCB(char* name, int thePid, Type theType);
PCB_p createProdAndConsPCB(char* name, int thePid, Type theType,
                           cust_mutex_p theMutexR1, cust_mutex_p theMutexR2);
PCB_p createIdlePCB();
char* getName(PCB_p pcb);
int getPid(PCB_p pcb);
int getPriority(PCB_p pcb);
int getPCValue(PCB_p pcb);
char* getState(PCB_p pcb);
char* getType(PCB_p pcb);
int getMax_PC(PCB_p pcb); //jowy added
int getNextData(PCB_p pcb);

void freePCB(PCB_p pcb);
void toString(PCB_p pcb);
void toStringFileVersion(PCB_p pcb, FILE* theOuputFile);

void setName(PCB_p pcb, char* newName);
void setPid(PCB_p pcb, int newPid);
void setPriority(PCB_p pcb, int newPriority);
void setType(PCB_p pcb, Type theType);
void setPCValue(PCB_p pcb, int thePCValue);
void setTheState(PCB_p pcb, State theState);
void setNextData(PCB_p pcb, int nextData);
// New Stuff
void generateIdleArrays(int arrayOne[4], int arrayTwo[4]);
void generateIOArrays(int arrayOne[4], int arrayTwo[4]);
void generateMutexArrays(int arrayR1Lock[4], int arrayR1Unlock[4],
                         int arrayR2Lock[4], int arrayR2Unlock[4],
                         int arrayIO1[4], int arrayIO2[4]);
void printArray(int theArray[4]);
char* getTheCurrentTime(void);





#endif

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "PCB.h"
#include "Mutex.h"

/*
 ===========================================================================
 Name        : 422_HW3.c
 Date        : 2/12/2016
 Authors     : Nok Him Nam, Jowy Tran, Mark Johnson, David Humphreys
 Description : Discontinuities with I/O service request traps and interrupts
 ===========================================================================
 */
// returns the pointer to the list; NULL if list not created
PCB_p createMutexPCB() {
    int i = 0;
    PCB_p pcb = malloc(sizeof(struct PCB));
    // if allocation was successful
    if (pcb != NULL) {

        strcpy(pcb->name, "Mutex");
        pcb->pid = 0;
        //-----------------------------
        pcb->priority = 0;
        pcb->tempPriority = 0;
        //-----------------------------
        pcb->pc = 0;
        pcb->currentState = ready;

        //New stuff
        pcb->creation = getTheCurrentTime();
        pcb->terminate = 0;
        pcb->term_count = 0;
        pcb->runTimes = 0;

        for(i = 0; i < 4; i++) {
            pcb->IO_1_traps[i] = -1;
            pcb->IO_2_traps[i] = -1;
        }
    }
    return pcb;
}

PCB_p createPCB(char* name, int newPid) {
    int priorityProbability = 0;
    int realPriority = 0;
    //struct control_block_type testing;
    PCB_p pcb = malloc(sizeof(struct PCB));
    // if allocation was successful
    if (pcb != NULL) {

        strcpy(pcb->name, "name");
        pcb->pid = newPid;
        srand(time(NULL));

        priorityProbability = rand() % 100;

        if(priorityProbability < 5) {
            realPriority = 0;
        } else if(priorityProbability < 85) {
            realPriority = 1;
        } else if (priorityProbability < 95) {
            realPriority = 2;
        } else {
            realPriority = 3;
        }

        pcb->priority = realPriority;
        pcb->tempPriority = realPriority;
        //-----------------------------
        pcb->pc = 0;
        pcb->currentState = ready;

        //New stuff
        pcb->creation = getTheCurrentTime();
        pcb->terminate = (rand() % MAX_TERM_COUNT) +1;
        pcb->term_count = 0;
        pcb->runTimes = 0;

        generateMutexArrays(pcb->arrayR1Lock, pcb->arrayR1Unlock,
                            pcb->arrayR2Lock, pcb->arrayR2Unlock,
                            pcb->IO_1_traps, pcb->IO_2_traps);

    }
    return pcb;
}

PCB_p createOneRandomPCB(char* name, int thePid, Type theType) {
   
    int tempPriority;
    int i = 0 ;
    //struct control_block_type testing;
    PCB_p pcb = malloc(sizeof(struct PCB));
    // if allocation was successful
    if (pcb != NULL) {

        strcpy(pcb->name, name);
        pcb->pid = thePid;
        srand(time(NULL));
        
        tempPriority = rand() % 100;
        
        if(tempPriority < 5) {
            pcb->priority = 0;
        } else if(tempPriority < 85) {
            pcb->priority = 1;
        } else if (tempPriority < 95) {
            pcb->priority = 2;
        } else {
            pcb->priority = 3;
        }
        
        pcb->type = theType;
        pcb->pc = 0;
        pcb->currentState = ready;

        //New stuff
        //pcb->max_pc = ?   jowy added  (do we need a new parameter?)
        pcb->creation = getTheCurrentTime();
        pcb->terminate = (rand() % MAX_TERM_COUNT) +1;
        pcb->term_count = 0;
        
        if( pcb->type == regular) {
            generateIOArrays(pcb->IO_1_traps, pcb->IO_2_traps);
        }
        else if (pcb->type == idle) {
            for(i = 0; i < 4; i++) {
                pcb->IO_1_traps[i] = -1;
                pcb->IO_2_traps[i] = -1;
            }
        }
    }
    return pcb;
}

PCB_p createProdAndConsPCB(char* name, int thePid, Type theType,
                           cust_mutex_p theMutexR1, cust_mutex_p theMutexR2) {
   
    int priorityProbability;
    int realPriority = 0;
    int i = 0 ;
    //struct control_block_type testing;
    PCB_p pcb = malloc(sizeof(struct PCB));
    // if allocation was successful
    if (pcb != NULL) {

        strcpy(pcb->name, name);
        pcb->pid = thePid;
        srand(time(NULL));
        
        priorityProbability = rand() % 100;
        
        if(priorityProbability < 5) {
            realPriority = 0;
        } else if(priorityProbability < 85) {
            realPriority = 1;
        } else if (priorityProbability < 95) {
            realPriority = 2;
        } else {
            realPriority = 3;
        }
        
        pcb->priority = realPriority;
        pcb->type = theType;
        pcb->pc = 0;
        pcb->currentState = ready;

        //New stuff
        //pcb->max_pc = ?   jowy added  (do we need a new parameter?)
        pcb->creation = getTheCurrentTime();
        pcb->terminate = (rand() % MAX_TERM_COUNT) +1;
        pcb->term_count = 0;
        pcb->mutexR1 = theMutexR1;
        pcb->mutexR2 = theMutexR2;
        
        Sleep(3);
        srand(time(NULL) * thePid * thePid);
        
        if(pcb->type == producer) {
            for(i = 0; i < 10; i++) {
                pcb->pcb_data[i] = (rand() % MAX_TERM_COUNT) +1 + i;
            }
        }
        if(pcb->type == consumer) {
            for(i = 0; i < 10; i++) {
                pcb->pcb_data[i] = 0;
            }
        }
	
        //if mutual resource A put locks in correct order
        if(pcb->type == mutual_resource_A) {
            generateMutexArrays(pcb->arrayR1Lock, pcb->arrayR1Unlock,
                                pcb->arrayR2Lock, pcb->arrayR2Unlock,
                                pcb->IO_1_traps, pcb->IO_2_traps);
            generateIdleArrays(pcb->IO_1_traps, pcb->IO_2_traps);
        }
        //if mutual resource B put locks in correct order
        else if (pcb->type == mutual_resource_B) {
            generateMutexArrays(pcb->arrayR2Lock, pcb->arrayR2Unlock,
                                pcb->arrayR1Lock, pcb->arrayR1Unlock,
                                pcb->IO_1_traps, pcb->IO_2_traps);
            generateIdleArrays(pcb->IO_1_traps, pcb->IO_2_traps);
        }
        else if(pcb->type == producer || pcb->type == consumer) {
        generateMutexArrays(pcb->arrayR1Lock, pcb->arrayR1Unlock,
                            pcb->arrayR2Lock, pcb->arrayR2Unlock,
                            pcb->IO_1_traps, pcb->IO_2_traps);
        }
        else if( pcb->type == regular) {
            generateIdleArrays(pcb->arrayR1Lock, pcb->arrayR1Unlock);
            generateIdleArrays(pcb->arrayR2Lock, pcb->arrayR2Unlock);
            generateIOArrays(pcb->IO_1_traps, pcb->IO_2_traps);
        }
        else if (pcb->type == idle) {
            generateIdleArrays(pcb->arrayR1Lock, pcb->arrayR1Unlock);
            generateIdleArrays(pcb->arrayR2Lock, pcb->arrayR2Unlock);
            generateIdleArrays(pcb->IO_1_traps, pcb->IO_2_traps);
        }
    }
    return pcb;
}

//Creates an IDLE PCB
// returns the pointer to the list; NULL if list not created
PCB_p createIdlePCB() {
        int i = 0;
	//struct control_block_type testing;
	PCB_p pcb = createPCB("IDLE_PCB", 0);
	// if allocation was successful

        strcpy(pcb->name, "IDLE_PCB");
        pcb->pid = -1;
        pcb->priority = 4;
        pcb->type = idle;
        pcb->pc = 0;
        pcb->currentState = running;
        pcb->type = idle;
            //New stuff
            //pcb->max_pc = ?   jowy added  (do we need a new parameter?)
        pcb->creation = getTheCurrentTime();
        pcb->terminate = INT_MAX;
        pcb->term_count = 0;
        for (; i < 4; i++) {
            pcb->IO_1_traps[i] = -1;
            pcb->IO_2_traps[i] = -1;
        }
	return pcb;
}

char* getName(PCB_p pcb) {
	return pcb->name;
}

int getPid(PCB_p pcb) {
	return pcb->pid;
}

int getPriority(PCB_p pcb) {
	return pcb->priority;
}

int getPCValue(PCB_p pcb) {
	return pcb->pc;
}

char* getState(PCB_p pcb) {

	switch (pcb->currentState) {
	case created: return "created"; break;
	case ready: return "ready"; break;
	case running: return "running"; break;
	case blocked: return "blocked"; break;
	case interrupted: return "interrupted"; break;
	case terminated: return "terminated"; break;
	default: return "ERROR";
	}
}	

char* getType(PCB_p pcb) {

	switch (pcb->type) {
	case regular: return "regular"; break;
	case idle: return "idle"; break;
	case vanilla: return "vanilla"; break;
	case producer: return "producer"; break;
	case consumer: return "consumer"; break;
	case mutual_resource_A: return "mutual_resource_A"; break;
	case mutual_resource_B: return "mutual_resource_B"; break;
	default: return "ERROR";
	}
}

int getMax_PC(PCB_p pcb) { //jowy added
    return pcb->max_pc;
}

int getNextData(PCB_p pcb) { //jowy added
    int nextData = 0, i = 0;
    for(i = 0; i < 10; i++) {
        if(pcb->pcb_data[i] != 0) {
            nextData = pcb->pcb_data[i];
            pcb->pcb_data[i] = 0;
            return nextData;
        }
    }
    return nextData;
}

void setNextData(PCB_p pcb, int nextData) { //jowy added
    int i = 0;
    for(i = 0; i < 10; i++) {
        if(pcb->pcb_data[i] == 0) {
            pcb->pcb_data[i] = nextData;
            return;
        }
    }
}

void setName(PCB_p pcb, char* newName) {
	strcpy(pcb->name, newName);
}

void setPid(PCB_p pcb, int newPid) {
	pcb->pid = newPid;
}

void setPriority(PCB_p pcb, int newPriority) {
	pcb->priority = newPriority;
}

void setInterruptSimulator(PCB_p pcb, Type theType) {
	pcb->type = theType;
}

void setPCValue(PCB_p pcb, int thePCValue) {
	pcb->pc = thePCValue;
}

void setTheState(PCB_p pcb, State theState) {
	pcb->currentState = theState;
}

void setMax_PC(PCB_p pcb, int theMax_PC) { //jowy added
    pcb->max_pc = theMax_PC;
}

void freePCB(PCB_p pcb) {
	free(pcb);
	pcb = NULL;
}

void toString(PCB_p pcb) {
        //note: %u is official print reference for unsigned integer.
	printf("Name: %s, terminate: %d, Priority: %d\nterm_count: %d, PC: %u, Type: %s, Mutex: %s\n\n",
		pcb->name, pcb->terminate, pcb->priority,
		pcb->term_count, pcb->pc, getType(pcb), pcb->mutexR1->name);
}

void toStringFileVersion(PCB_p pcb, FILE* theOuputFile) {
	fprintf(theOuputFile, "Name: %s, terminate: %d, Priority: %d\nterm_count : %d, PC : %u, Type : %s\n\n",
		pcb->name, pcb->terminate, pcb->priority,
		pcb->term_count, pcb->pc, getType(pcb));
}

void generateIOArrays(int arrayOne[4], int arrayTwo[4]) {

	int i = 0;
	int spaceNum = MAX_PC / 8;

	for (i = 0; i < 8; i++) {

		if (i % 2 == 0) {
			arrayOne[i/2] = i * spaceNum + rand() % spaceNum;
		}
		else {
			arrayTwo[i/2] = i * spaceNum + rand() % spaceNum;
		}
	}
}

void generateIdleArrays(int arrayOne[4], int arrayTwo[4]) {
    int i = 0;
    for (; i < 4; i++) {
        arrayOne[i] = -1;
        arrayTwo[i] = -1;
    }
}


void generateMutexArrays(int arrayR1Lock[4], int arrayR1Unlock[4],
                         int arrayR2Lock[4], int arrayR2Unlock[4],
                         int arrayIO1[4], int arrayIO2[4]) {
    if(MAX_PC < 48) {
        printf("ERROR: Max PC must be set to at least 48\n");
        return;
    }
    int i = 1;
    int spaceNum = MAX_PC / 24;

    for (i = 1; i <= 24; i++) {

            if (i % 6 == 1) {
                arrayR1Lock[(i-1)/6] = i * spaceNum + rand() % spaceNum;
            }
            else if (i % 6 == 2) {
                arrayR2Lock[(i-1)/6] = i * spaceNum + rand() % spaceNum;
            }
            else if (i % 6 == 3) {
                arrayR2Unlock[(i-1)/6] = i * spaceNum + rand() % spaceNum;
            }
            else if (i % 6 == 4) {
                arrayR1Unlock[(i-1)/6] = i * spaceNum + rand() % spaceNum;
            }
            else if (i % 6 == 5) {
                arrayIO1[(i-1)/6] = i * spaceNum + rand() % spaceNum;
            }
            else {
                arrayIO2[(i-1)/6] = i * spaceNum + rand() % spaceNum;
            }
    }
}

void printArray(int theArray[4]) {

	int i = 0;

	for (i = 0; i < 4; i++) {
		printf("%d ", theArray[i]);
	}

	printf("\n");
}

char* getTheCurrentTime(void) {

	time_t currentTimeAccesser;
	char* tempString;
	currentTimeAccesser = time(NULL);
	tempString = ctime(&currentTimeAccesser);
	return tempString;
}
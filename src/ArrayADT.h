/*
 ===========================================================================
 Name        : 422_HW3.c
 Date        : 2/12/2016
 Authors     : Nok Him Nam, Jowy Tran, Mark Johnson, David Humphreys
 Description : Discontinuities with I/O service request traps and interrupts
 ===========================================================================
 */
#ifndef ARRAYADT_H
#define ARRAYADT_H
#include "Mutex.h"

struct Priority_queue {
	FIFO_queue_p Queue_List[16];
	int topPriority;
};

typedef struct Priority_queue * Priority_queue_p;

void printPQ(Priority_queue_p pq);
Priority_queue_p createPQ();
void addPCB(Priority_queue_p pq, PCB_p pcb);
int peek(Priority_queue_p pq);
int checkTopPriority(Priority_queue_p pq);
PCB_p search(Priority_queue_p pq);
void freePriority_queue(Priority_queue_p pq);
int size_of(Priority_queue_p pq);

#endif



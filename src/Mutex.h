/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Mutex.h
 * Author: David
 *
 * Created on March 9, 2016, 1:40 PM
 */

#ifndef MUTEX_H
#define MUTEX_H
#include "LinkedListADT.h"

#define POSSIBLE_PCBS 50

#define DATA_SIZE 10
    
struct cust_mutex {
    char name[20];
    PCB_p owningPCB;
    FIFO_queue_p waitingPCBS;
    int sharedData;
};

typedef struct cust_mutex* cust_mutex_p;

cust_mutex_p createCustMutex();
int tryLockCustMutex(cust_mutex_p* mutex, PCB_p* pcb);
int unlockCustMutex(cust_mutex_p* mutex, PCB_p* pcb);
int getBlockingPCB(cust_mutex_p mutex);

#endif /* MUTEX_H */


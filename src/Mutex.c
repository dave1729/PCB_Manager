/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
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
cust_mutex_p createCustMutex() {
    int i = 0;
    cust_mutex_p myMutex = malloc(sizeof(struct cust_mutex));
    myMutex->sharedData = 0;
    myMutex->waitingPCBS = createLinkedList();
    strcpy(myMutex->name, "DefaultMutex");
    return (cust_mutex_p) myMutex;
}

int tryLockCustMutex(cust_mutex_p* mutex, PCB_p* pcb) {
    if(*pcb == (*mutex)->owningPCB) {
        //you already have the lock, do nothing
        return 0;
    }
    else if ((*mutex)->owningPCB == NULL) {
        //you want it and it is avaliable, take it!
        (*mutex)->owningPCB = *pcb;
        return 0;
    }
    else {
        //you don't have it and it's not avaliable
        //Error 666: It was owned by someone else.
        return 666;
    }
}
int unlockCustMutex(cust_mutex_p* mutex, PCB_p* pcb) {    
    if(*pcb == (*mutex)->owningPCB) {
        //you own it and we'll set it to null
        (*mutex)->owningPCB = NULL;
        return 0;
    }
    else {
        //you don't have access to unlock
        return 1;
    }
}
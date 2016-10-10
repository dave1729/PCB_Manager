
/*
 ===========================================================================
 Name        : 422_FINAL_Project.c
 Date        : 3/11/2016
 Authors     : Nok Him Nam, Jowy Tran, Mark Johnson, David Humphreys
 Description : Testing situations that cause Deadlock.
 ===========================================================================
 */

#include <pthread.h>//for multi-threading
#include <stdio.h>
#include <stdlib.h>
#include "ArrayADT.h"
#include <unistd.h> // for usleep
#include <signal.h>
#include <errno.h>

//Testing mode = 1, Regular Mode = 0;
int const TESTING_MODE = 1;

//Number of Mutual Resource PCB Pairs to create
int const TOTAL_PCB_COUNT = 12;

//Number of instructions to execute before program ends
int const INSTRUCTIONS_TO_EXE = 2000;

//Stops the program 4 times per quantum for this many milliseconds
int const PROGRAM_SPEED = 10;

//Timer wait time in milliseconds
int const TIMER_WAIT_TIME = 993;

//IO 1 wait time in milliseconds
int const IO1_WAIT_TIME = 2015;

//IO 2 wait time in milliseconds, IO 2 should be larger than IO 1
int const IO2_WAIT_TIME = 3159;

//THREAD MUTEXES
cust_mutex_p timer_mtx, io1_mtx, io2_mtx;
//THREAD PCBS (since PCB addresses are used to hold locks))
PCB_p mainPCB, timerPCB, io1PCB, io2PCB;

/* TIMER FUNCTION*/
void *timer_function(void *int_void_p) {
    int* programKill = (int*) int_void_p;

    //Timer Loop
    while(*programKill) {
        Sleep(TIMER_WAIT_TIME);
        tryLockCustMutex(&timer_mtx, &timerPCB);
    }
    printf("Timer Thread Ended. FINAL QUANTUM = %d\n", *programKill);
    /* the function must return something - NULL will do */
    return NULL;
}

void *io_1_function(void *int_void_p) {
    int* programKill = (int*) int_void_p;

    //IO1 Loop
    while(*programKill) {
        Sleep(IO1_WAIT_TIME);
        tryLockCustMutex(&io1_mtx, &io1PCB);
    }
    printf("IO1 Thread Ended. FINAL QUANTUM = %d\n", *programKill);
    /* the function must return something - NULL will do */
    return NULL;
}

void *io_2_function(void *int_void_p) {
    int* programKill = (int*) int_void_p;

    //IO2 Loop
    while(*programKill) {
        Sleep(IO2_WAIT_TIME);
        tryLockCustMutex(&io2_mtx, &io2PCB);
    }
    printf("IO2 Thread Ended. FINAL QUANTUM = %d\n", *programKill);
    /* the function must return something - NULL will do */
    return NULL;
}

void starvationChecking (Priority_queue_p thePriorityQ) {

    int i = 0;
    struct node * tempNode;
    int totalOfPCBs = size_of(thePriorityQ) - 1;
    FIFO_queue_p tempFifoQueue = createLinkedList();

    // Loop through the priority q and add all the PCBs into a fifo q
    for(i = 0; i < totalOfPCBs; i++ ) {
        enqueue(tempFifoQueue, search(thePriorityQ));
    }

    tempNode = tempFifoQueue->frontNode_ptr;

    while (!is_empty(tempFifoQueue)) {

        if(tempNode->data->runTimes == 0 && tempNode->data->tempPriority != 0) {
            tempNode->data->tempPriority--;
        } else {
            tempNode->data->tempPriority = tempNode->data->priority;
        }

        tempNode->data->runTimes = 0;

        addPCB(thePriorityQ, dequeue(tempFifoQueue)->data);
        tempNode = tempNode->next;
    }
    freeLinkedList(tempFifoQueue);
}

int main(int argc, char **argv) {
    timer_mtx = createCustMutex();
    io1_mtx = createCustMutex();
    io2_mtx = createCustMutex();
    mainPCB = createMutexPCB();
    timerPCB = createMutexPCB();
    io1PCB = createMutexPCB();
    io2PCB = createMutexPCB();
    FILE* output_ptr;
    //output_ptr = output_ptr = fopen("test.txt", "w+");
    if(argc == 2 && (int)( sizeof(argv) / sizeof(argv[0]) ) > 0) {
        output_ptr = fopen(argv[1], "w+");
    }else {
        output_ptr = stdout;
    }

    //holds initial PCBs
    FIFO_queue_p new_queue = createLinkedList();

    //Print generated PCBs
    fprintf(output_ptr, "--------LIST OF GENERATED PCBs--------\n\n");

    //populates new_queue with PCBs
    createRandomPCBs(new_queue, TOTAL_PCB_COUNT);

    if(1) {
        fprintf(output_ptr, "----------PCB CREATION TIMES----------\n\n");
    }

    //A Priority Queue to hold the organized PCBs in the ready state
    Priority_queue_p PQ_p = createPQ();

    //Queues that represent the two io waiting queues
    FIFO_queue_p io1_queue = createLinkedList();
    FIFO_queue_p io2_queue = createLinkedList();

    //runningPCB_p: the currently running pcb, temp: used for switching
    PCB_p runningPCB_p, temp;

    //A temp node that is used for returning from dequeued FIFO queues
    struct node* tempNode;

    //ends all running while loops in threads when set to 0
    int quantumNum = 1, i = 0, mutexCheck;

    if(1) {
        //All Mutual Resource PCBs go into ArrayADT and print their creation time
        while(size_is(new_queue) != 0) {
            tempNode = dequeue(new_queue);
            PCB_p tempPCB_p = tempNode->data;
            fprintf(output_ptr, "Process created: PID %d at %s\n", tempPCB_p->pid, tempPCB_p->creation);
            addPCB(PQ_p, tempPCB_p);
        }
    }

    // Start the Idle PCB and initialize runningPCB to idle
    runningPCB_p = createIdlePCB();
    addPCB(PQ_p, createIdlePCB());

    //Threads initialization
    pthread_t timer_thread, io_1_thread, io_2_thread;

    fprintf(output_ptr, "Timer Thread Starting...\n\n");
    /* create timer thread */
    if(pthread_create(&timer_thread, NULL, timer_function, &quantumNum)) {

        fprintf(output_ptr, "Error 326485: creating timer thread\n");
        return 3;
    }
    Sleep(TIMER_WAIT_TIME);
    fprintf(output_ptr, "Timer Started.\n\n");
    fprintf(output_ptr, "IO1 Thread Starting...\n\n");
    /* create io 1 thread */
    if(pthread_create(&io_1_thread, NULL, io_1_function, &quantumNum)) {

	fprintf(output_ptr, "Error 78787: creating io 1 thread\n");
	return 1;
    }
    Sleep(IO1_WAIT_TIME);
    fprintf(output_ptr, "IO1 Started.\n\n");
    fprintf(output_ptr, "IO2 Thread Starting...\n\n");
    /* create io 2 thread */
    if(pthread_create(&io_2_thread, NULL, io_2_function, &quantumNum)) {

	fprintf(output_ptr, "Error 666: creating io 2 thread\n");
	return 2;
    }
    Sleep(IO2_WAIT_TIME);
    fprintf(output_ptr, "IO2 Started.\n\n");
    fprintf(output_ptr, "------------------------------------\n\n");

    fprintf(output_ptr, "CPU STARTS RUNNING!\n");
    //CPU THREAD (loop) STARTS HERE! (all other threads are already running)
    /* CPU essentially runs until programKill is 0. */

    while(quantumNum != INSTRUCTIONS_TO_EXE) {

        if(TESTING_MODE) {
            fprintf(output_ptr, "\n------------QUANTUM #%d!------------\n", ++quantumNum);
        }
        //Program delay speed in milliseconds
        Sleep(PROGRAM_SPEED);

        //try mutex lock on timer
        mutexCheck = tryLockCustMutex(&timer_mtx, &mainPCB);
        //if timer mutex is already locked
        if(mutexCheck != 0) {

            //PSEUDO CALL TO TIMER_ISR
            runningPCB_p->currentState = ready;
            //PSEUDO CALL TO DISPATCHER
            temp = runningPCB_p;
            runningPCB_p = search(PQ_p);
            runningPCB_p->currentState = running;
            addPCB(PQ_p, temp);

            //print switching details
            fprintf(output_ptr, "\nTimer interrupt: PID %d was running, PID %d dispatched                    <- TIMER EVENT\n",
                    temp->pid, runningPCB_p->pid);
            int temp = -1;
            temp = unlockCustMutex(&timer_mtx, &timerPCB);
        }
        //if there was no lock already, unlock the lock we accomplished
        else {
            unlockCustMutex(&timer_mtx, &mainPCB);
        }

        Sleep(PROGRAM_SPEED);

        //try mutex lock on IO1
        mutexCheck = tryLockCustMutex(&io1_mtx, &mainPCB);

        //if IO1 mutex is already locked
        if(mutexCheck != 0) {

            //Dequeue from IO1 waiting queue if possible
            if(size_is(io1_queue) > 0) {
                tempNode = dequeue(io1_queue);
                tempNode->data->currentState = ready;
                toStringFileVersion(tempNode->data, output_ptr);
                addPCB(PQ_p, tempNode->data);
                fprintf(output_ptr, "\nI/O 1 completion interrupt: PID %d is running, PID %d put in ready queue          <- IO1 COMPLETION\n",
                        runningPCB_p->pid, tempNode->data->pid);
            }
            //if not
            else {
                fprintf(output_ptr, "\nI/O 1 completion interrupt: PID %d is running, No avaliable PCB to put in ready queue          <- IO1 COMPLETION\n",
                        runningPCB_p->pid);
            }
            unlockCustMutex(&io1_mtx, &io1PCB);
        }
        else {
            unlockCustMutex(&io1_mtx, &mainPCB);
        }

        Sleep(PROGRAM_SPEED);

        //try mutex lock on IO2
        mutexCheck = tryLockCustMutex(&io2_mtx, &mainPCB);

        //if IO2 mutex is already locked
        if(mutexCheck != 0) {

            //Dequeue from IO2 waiting queue if possible
            if(size_is(io2_queue) > 0) {
                tempNode = dequeue(io2_queue);
                tempNode->data->currentState = ready;
                toStringFileVersion(tempNode->data, output_ptr);
                addPCB(PQ_p, tempNode->data);
                fprintf(output_ptr, "\nI/O 2 completion interrupt: PID %d is running, PID %d put in ready queue          <- IO2 COMPLETION\n",
                        runningPCB_p->pid, tempNode->data->pid);
            }
            //if not
            else {
                fprintf(output_ptr, "\nI/O 2 completion interrupt: PID %d is running, No avaliable PCB to put in ready queue          <- IO2 COMPLETEION\n",
                        runningPCB_p->pid);
            }
            unlockCustMutex(&io2_mtx, &io2PCB);
        }
        else {
            unlockCustMutex(&io2_mtx, &mainPCB);
        }

        if(TESTING_MODE) {
            //print array and queue sizes for testing
            fprintf(output_ptr, "\nPrint Data Structure Sizes for Testing:\n");
            fprintf(output_ptr, "The size of the array ADT is : %d\n"
                    "The size of the I/O 1 queue : %d\n"
                    "The size of the I/O 2 queue : %d\n\n", size_of(PQ_p),
                    size_is(io1_queue), size_is(io2_queue));
        }

        Sleep(PROGRAM_SPEED);

        if(TESTING_MODE) {
            //Increment running PCB to simulate it running a single instruction
            fprintf(output_ptr, "\nCurrent PC: %d    IN   PCB Named: %s\n", runningPCB_p->pc++, runningPCB_p->name);
        }

        //rintf("runningPCB PC: %d      MAX_PC: %d      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", runningPCB_p->pc, MAX_PC);
        //if pc val gets too its max, increment term count
        if(runningPCB_p->pc >= MAX_PC) {
            runningPCB_p->pc = 0;
            runningPCB_p->term_count++;
        }
        //Else check for IO interrupts
        else {
            if(TESTING_MODE) {
                fprintf(output_ptr, "\nRunning PCB: %s\n", runningPCB_p->name);
                fprintf(output_ptr, "R1Lock:   %d %d %d %d\n", runningPCB_p->arrayR1Lock[0], runningPCB_p->arrayR1Lock[1], runningPCB_p->arrayR1Lock[2], runningPCB_p->arrayR1Lock[3]);
                fprintf(output_ptr, "R1Unlock: %d %d %d %d\n", runningPCB_p->arrayR1Unlock[0], runningPCB_p->arrayR1Unlock[1], runningPCB_p->arrayR1Unlock[2], runningPCB_p->arrayR1Unlock[3]);
                fprintf(output_ptr, "R2Lock:   %d %d %d %d\n", runningPCB_p->arrayR2Lock[0], runningPCB_p->arrayR2Lock[1], runningPCB_p->arrayR2Lock[2], runningPCB_p->arrayR2Lock[3]);
                fprintf(output_ptr, "R2Unlock: %d %d %d %d\n", runningPCB_p->arrayR2Unlock[0], runningPCB_p->arrayR2Unlock[1], runningPCB_p->arrayR2Unlock[2], runningPCB_p->arrayR2Unlock[3]);
                fprintf(output_ptr, "IO1:      %d %d %d %d\n", runningPCB_p->IO_1_traps[0], runningPCB_p->IO_1_traps[1], runningPCB_p->IO_1_traps[2], runningPCB_p->IO_1_traps[3]);
                fprintf(output_ptr, "IO2:      %d %d %d %d\n", runningPCB_p->IO_2_traps[0], runningPCB_p->IO_2_traps[1], runningPCB_p->IO_2_traps[2], runningPCB_p->IO_2_traps[3]);
            }

            if(TESTING_MODE) {
                if(runningPCB_p->type == producer) {
                    fprintf(output_ptr, "\n\nPCB: %s  PID: %d with Mutex: %s is a producer. Data is %d %d %d %d %d %d %d %d %d %d\n",
                           runningPCB_p->name, runningPCB_p->pid, runningPCB_p->mutexR1->name, runningPCB_p->pcb_data[0], runningPCB_p->pcb_data[1],
                           runningPCB_p->pcb_data[2], runningPCB_p->pcb_data[3], runningPCB_p->pcb_data[4], runningPCB_p->pcb_data[5],
                           runningPCB_p->pcb_data[6], runningPCB_p->pcb_data[7], runningPCB_p->pcb_data[8], runningPCB_p->pcb_data[9]);
                }

                if(runningPCB_p->type == consumer) {
                    fprintf(output_ptr, "\n\nPCB: %s  PID: %d with Mutex: %s is a consumer. Data is %d %d %d %d %d %d %d %d %d %d\n",
                           runningPCB_p->name, runningPCB_p->pid, runningPCB_p->mutexR1->name, runningPCB_p->pcb_data[0], runningPCB_p->pcb_data[1],
                           runningPCB_p->pcb_data[2], runningPCB_p->pcb_data[3], runningPCB_p->pcb_data[4], runningPCB_p->pcb_data[5],
                           runningPCB_p->pcb_data[6], runningPCB_p->pcb_data[7], runningPCB_p->pcb_data[8], runningPCB_p->pcb_data[9]);
                }
            }

            int tempInt = 0, j = 0;
            // Check for I/O interrupts and Mutex Actions
            for(i = 0; i < 4; i++) {
                if(runningPCB_p->pc == runningPCB_p->arrayR1Lock[i]) {
                    mutexCheck = tryLockCustMutex(&runningPCB_p->mutexR1, &runningPCB_p);
                    if (mutexCheck == 0) {
                        fprintf(output_ptr, "\nPID %d: requested lock on %s - succeeded                                       <- Mutex Lock Success!\n",
                            runningPCB_p->pid, runningPCB_p->mutexR1->name);
                        //PRODUCER PRINTOUT
                        if(runningPCB_p->type == producer && runningPCB_p->mutexR1->sharedData == 0) {
                            tempInt = getNextData(runningPCB_p);
                            fprintf(output_ptr, "PRODUCER... SHARED DATA: %d   PASSED DATA: %d\n", runningPCB_p->mutexR1->sharedData, tempInt);
                            runningPCB_p->mutexR1->sharedData = tempInt;
                            fprintf(output_ptr, "PCB: %s  PID: %d with Mutex: %s is a producer. Data is %d %d %d %d %d %d %d %d %d %d\n",
                                runningPCB_p->name, runningPCB_p->pid, runningPCB_p->mutexR1->name, runningPCB_p->pcb_data[0], runningPCB_p->pcb_data[1],
                                runningPCB_p->pcb_data[2], runningPCB_p->pcb_data[3], runningPCB_p->pcb_data[4], runningPCB_p->pcb_data[5],
                                runningPCB_p->pcb_data[6], runningPCB_p->pcb_data[7], runningPCB_p->pcb_data[8], runningPCB_p->pcb_data[9]);
                            fprintf(output_ptr, "\nNotice the above Producer PCB has written to Shared Data.\n");
                            sleep(7);
                        }
                        //CONSUMER PRINTOUT
                        if(runningPCB_p->type == consumer && runningPCB_p->mutexR1->sharedData != 0) {
                            fprintf(output_ptr, "Running is CONSUMER AND GOT LOCK 1: %d 2: %d Data: %d", runningPCB_p->pcb_data[0], runningPCB_p->pcb_data[1], runningPCB_p->mutexR1->sharedData);
                            fprintf(output_ptr, "CONSUMER... SHARED DATA: %d   Reset Shared to: %d\n", runningPCB_p->mutexR1->sharedData, 0);
                            setNextData(runningPCB_p, runningPCB_p->mutexR1->sharedData);
                            runningPCB_p->mutexR1->sharedData = 0;
                            fprintf(output_ptr, "PCB: %s  PID: %d with Mutex: %s is a consumer. Data is %d %d %d %d %d %d %d %d %d %d\n",
                                runningPCB_p->name, runningPCB_p->pid, runningPCB_p->mutexR1->name, runningPCB_p->pcb_data[0], runningPCB_p->pcb_data[1],
                                runningPCB_p->pcb_data[2], runningPCB_p->pcb_data[3], runningPCB_p->pcb_data[4], runningPCB_p->pcb_data[5],
                                runningPCB_p->pcb_data[6], runningPCB_p->pcb_data[7], runningPCB_p->pcb_data[8], runningPCB_p->pcb_data[9]);
                            fprintf(output_ptr, "\nNotice the above consumer PCB has read from Shared Data.\n");
                            sleep(7);
                        }
                    }
                    else {
                        fprintf(output_ptr, "\nPID %d: requested lock on %s - blocked by PID %d                                       <- Mutex Lock Blocked!\n",
                            runningPCB_p->pid, runningPCB_p->mutexR1->name, runningPCB_p->mutexR1->owningPCB->pid);
                    }
                }
                else if(runningPCB_p->pc == runningPCB_p->arrayR1Unlock[i]) {
                    mutexCheck = unlockCustMutex(&runningPCB_p->mutexR1, &runningPCB_p);
                    if (mutexCheck == 0) {
                        fprintf(output_ptr, "\nPID %d: relinquished lock on %s - succeeded                                       <- Mutex UNlock Success!\n",
                            runningPCB_p->pid, runningPCB_p->mutexR1->name);
                    }
                }
                else if(runningPCB_p->pc == runningPCB_p->arrayR2Lock[i] && (runningPCB_p->type == mutual_resource_B || runningPCB_p->pid == runningPCB_p->mutexR1->owningPCB->pid) ) {
                    mutexCheck = tryLockCustMutex(&runningPCB_p->mutexR2, &runningPCB_p);
                    if (mutexCheck == 0) {
                        fprintf(output_ptr, "\nPID %d: requested lock on %s - succeeded                                       <- Mutex Lock Success!\n",
                            runningPCB_p->pid, runningPCB_p->mutexR2->name);
                        //POSSIBLE DEADLOCK PRINTOUT
                        if(runningPCB_p->mutexR1->owningPCB->name != NULL && runningPCB_p->mutexR2->owningPCB->name != NULL && runningPCB_p->mutexR1->owningPCB != runningPCB_p->mutexR2->owningPCB) {
                            fprintf(output_ptr, "\n\nDEADLOCK\n");
                            fprintf(output_ptr, "\nBetween: %s and %s.\n", runningPCB_p->mutexR1->owningPCB->name,  runningPCB_p->mutexR2->owningPCB->name);
                            sleep(7);
                            //runningPCB_p->mutexR1->owningPCB = NULL;
                            //runningPCB_p->mutexR2->owningPCB = NULL;
                        }
                    }
                    else {
                        fprintf(output_ptr, "\nPID %d: requested lock on %s - blocked by PID %d                                       <- Mutex Lock Blocked!\n",
                            runningPCB_p->pid, runningPCB_p->mutexR2->name, runningPCB_p->mutexR2->owningPCB->pid);
                    }
                }
                else if(runningPCB_p->pc == runningPCB_p->arrayR2Unlock[i]) {
                    mutexCheck = unlockCustMutex(&runningPCB_p->mutexR2, &runningPCB_p);
                    if (mutexCheck == 0) {
                        fprintf(output_ptr, "\nPID %d: relinquished lock on %s - succeeded                                       <- Mutex UNlock Success!\n",
                            runningPCB_p->pid, runningPCB_p->mutexR2->name);
                    }
                }
                else if(runningPCB_p->pc == runningPCB_p->IO_1_traps[i]) {
                //if(runningPCB_p->pc == runningPCB_p->IO_1_traps[i]) {
                    temp = runningPCB_p;
                    temp->currentState = interrupted;
                    enqueue(io1_queue, temp);
                    runningPCB_p = search(PQ_p);
                    runningPCB_p->currentState = running;
                    fprintf(output_ptr, "\nI/O trap request: I/O device 1, PID %d put into waiting queue, PID %d dispatched          <- IO1 TRAP REQ.\n",
                            temp->pid, runningPCB_p->pid);
                }
                else if(runningPCB_p->pc == runningPCB_p->IO_2_traps[i]) {
                    temp = runningPCB_p;
                    temp->currentState = interrupted;
                    enqueue(io2_queue, temp);
                    runningPCB_p = search(PQ_p);
                    runningPCB_p->currentState = running;
                    fprintf(output_ptr, "\nI/O trap request: I/O device 2, PID %d put into waiting queue, PID %d dispatched          <- IO2 TRAP REQ.\n",
                            temp->pid, runningPCB_p->pid);
                }
                break;
            }
        }

        //Delete PCBs if they are done.
        if (runningPCB_p->term_count == runningPCB_p->terminate) {
            fprintf(output_ptr, "PCB: %s   Terminated.  Term Count: %d  Terminate @: %d\n", runningPCB_p->name, runningPCB_p->term_count, runningPCB_p->terminate);
            temp = runningPCB_p;
            runningPCB_p = search(PQ_p);
            runningPCB_p->currentState = running;
            free(temp);
            temp = NULL;
            sleep(7);
        }

        if(quantumNum % 200 == 199) {
            starvationChecking(PQ_p);
        }

        //quantumNum++;
    }

    fprintf(output_ptr, "\nMAXIMUM NUMBER OF INSTRUCTIONS TO EXECUTE REACHED!\n");
    fprintf(output_ptr, "\nMAIN THREAD ENDING OTHER THREADS\n");
    fprintf(output_ptr, "--------------------------------\n\n");
    unlockCustMutex(&timer_mtx, &timerPCB);
    unlockCustMutex(&io1_mtx, &io1PCB);
    unlockCustMutex(&io2_mtx, &io2PCB);
    quantumNum = 0;
    fprintf(output_ptr, "Wait for all 3 threads to end...\n");
    //IO2 has the longest wait time
    Sleep(IO2_WAIT_TIME);

    /* wait for the second thread to finish */

    if(pthread_join(timer_thread, NULL)) {

	fprintf(output_ptr, "Error 3131: joining timer thread\n");
	return 6;
    }
    if(pthread_join(io_1_thread, NULL)) {

	fprintf(output_ptr, "Error 6964747: joining io 1 thread\n");
	return 4;
    }

    if(pthread_join(io_2_thread, NULL)) {

	fprintf(output_ptr, "Error 11111: joining io 2 thread\n");
	return 5;
    }

    //Free Dynamically Allocated Data
    freePCB(runningPCB_p);
    freePCB(temp);
    freeLinkedList(new_queue);
    freeLinkedList(io1_queue);
    freeLinkedList(io2_queue);
    //freePriority_queue(PQ_p);

    fprintf(output_ptr, "\nMAIN(CPU) Thread Ended. FINAL QUANTUM = %d\n", quantumNum);
    fprintf(output_ptr, "\n!!! SCROLL BACK TO SEE SIMULATION PROGRESSION !!!\n");
    return 0;
}

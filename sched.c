#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BNode
{
 int burst;
 struct BNode *next;
} BNode;

typedef struct BurstQ
{
 int size;
 struct BNode *head;
 struct BNode *tail;
} BurstQ;

typedef struct Process
{
 int ArrivalTime;
 int numOfBursts;
 BurstQ *burstQ;

 int TurnAroundTime;
 int WaitTime;
 int sleepTime;
 int responseTime;
} Process;

typedef struct QNode
{
 Process *process;
 struct QNode *next;
} Qnode;

typedef struct Q
{
 int size;
 struct QNode *head;
 struct QNode *tail;
} Q;

int BQpush(int newInt,BurstQ *toQ)
{
 struct BNode *newNode = malloc(sizeof(struct BNode));
 newNode->burst = newInt;
 if(toQ->size == 0)
  {
   toQ->head = newNode;
   toQ->tail = newNode;
  }
 else
 {
  toQ->tail->next = newNode;
  toQ->tail = newNode;
 }
  toQ->size++;
  return toQ->size;
}

int BQpop(BurstQ *popQ)
{
 if(popQ->size == 0)
  {
   return NULL;
  }
  else if(popQ->size == 1)
  {
   int toPop = popQ->head->burst;
   popQ->head = NULL;
   popQ->tail = NULL;
   popQ->size--;
   return toPop;
  }
  else
   {
    int toPop = popQ->head->burst;
    popQ->head = popQ->head->next;
    popQ->size--;
    return toPop;
   }
}

int Qpush(Process *newProcess,Q *toQ)
{
 struct QNode *newNode = malloc(sizeof(struct QNode));
 newNode->process = newProcess;
 if(toQ->size == 0)
  {
   toQ->head = newNode;
   toQ->tail = newNode;
  }
 else
 {
  toQ->tail->next = newNode;
  toQ->tail = newNode;
 }
  toQ->size++;
  return toQ->size;
}

Process *Qpop(Q *popQ)
{
 if(popQ->size == 0)
  {
   return NULL;
  }
  else if(popQ->size == 1)
  {
   Process *toPop = popQ->head->process;
   popQ->head = NULL;
   popQ->tail = NULL;
   popQ->size--;
   return toPop;
  }
  else
   {
    Process *toPop = popQ->head->process;
    popQ->head = popQ->head->next;
    popQ->size--;
    return toPop;
   }
}

void printProcessStatus(Process *process)

{
 printf("[ Process: -Arrival Time: %d -NumOfBursts: %d -BurstQ: ",process->ArrivalTime,process->numOfBursts);
 BNode *cur = process->burstQ->head;
 for(int i = 0;i<process->burstQ->size;i++)
  {
   printf("%d ",(cur->burst));
   cur = cur->next;
  }
 printf("]\n");
}

int printQ(Q *q)
{
 struct QNode *cur = q->head;
 int i = 0;
 if(q->size == 0)
 {
  printf("empty\n");
  return 0;
 }
 else
  {
   for(int i = 0; i<q->size;i++)
    {
     printProcessStatus(cur->process);
     cur = cur->next;
    }
    printf("\n");
    fflush(stdout);
    return q->size;
  }

}

void parseInput(char filename[],Q *arrivalQ)
{
 char *token;
 FILE *file = fopen (filename,"r");
 if (file != NULL)
  {
   char line[128];

   while(fgets(line,sizeof(line),file) != NULL)
    {
     Process *process = malloc(sizeof(struct Process));
     BurstQ *burstQ = malloc(sizeof(struct BurstQ));
     process->burstQ = burstQ;
     token = strtok(line," ");
     process->ArrivalTime = atoi(token);
     token = strtok(NULL," ");
     process->numOfBursts = atoi(token);
     token = strtok(NULL," ");
     while(token != NULL)
      {
       BQpush(atoi(token),process->burstQ);
       token = strtok(NULL," ");
      }
     Qpush(process,arrivalQ);
    }
    fclose(file);
  }
}

int main( int argc,char *argv[])
{
 if(argc != 3)
  {
   printf("usage:\n");
  }
 int quantum = atoi(argv[1]);
 Q *arrivalQ = malloc(sizeof(struct Q));
 parseInput(argv[2],arrivalQ);
 printf("Arrival Queue: \n");
 printQ(arrivalQ);
 printf("\n");
 int clockTick = 0;
 Process *CPU = NULL;
 int totalJobs = arrivalQ->size;
 int currentQuantum = quantum;
 while(doneQ->size != totalJobs)
  {
   clockTick++;
   arrive(clockTick,arrivalQ,readyQ);
   if(CPU == NULL)
    {
     schedule(readyQ,CPU);
    }
   bool preempted = checkForPreempt(CPU,currentQuantum,readyQ);
   bool CPUstatus = CPUtick(CPU,preempted);
   transferCPU(CPU,CPUstatus,waitQ);
   wake(clockTick,waitQ);
  }
  printf("Scheduler Status: \n");
  printQfinal(doneQ);
  printf("\n");
}
// int main()
// {
//     parseFile();
//     int clockTick = 0;
//     int quantum = 0;
//     while(ProcessInQueue)
//      {
//       if(quantumIsUp)
//        {
//         premptRunningProcess();
//         quantum = 0;
//        }
//       clockTick++;
//       quantum++;
//      }
// }

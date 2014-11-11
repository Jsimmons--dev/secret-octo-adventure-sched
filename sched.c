#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int preempt = 0;

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
   return -1;
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

Process *Qremove(int i,Q *q)
{
 if(q->size == 0)
  {
   return NULL;
  }
  else if(q->size == 1)
  {
   return Qpop(q);
  }
  else
   {
  if(i == 0)
  {
   return Qpop(q);
  }
  if(i < q->size-1)
   {
    struct QNode *cur = q->head;
    for(int j = 0; j<i-2;j++)
    {
     cur = cur->next;
    }
    Process *toRemove = cur->next->process;
    if(i == q->size-1)
     {
      cur->next = NULL;
      return toRemove;
     }
    cur->next = cur->next->next;
    return toRemove;
   }
   return NULL;
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



void printProcessStatus(Process *process)

{
 if(process != NULL)
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
else
 {
  printf(" *NULL*\n");
 }
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

int arrive(int clockTick,Q *arrivalQ,Q *readyQ)
{
 struct QNode *curNode = arrivalQ->head;
 if(arrivalQ->size != 0)
  {
 int stillReady = 1;
 for(int i = 0;i<arrivalQ->size && stillReady == 1;i++)
  {
   if(curNode->process->ArrivalTime == clockTick)
   {
    Process *pop = Qpop(arrivalQ);
    printf("-Process ");
    printProcessStatus(pop);
    printf(" -has arrived\n\n");
    Qpush(pop,readyQ);
    curNode = arrivalQ->head;
   }
   else
    {
     stillReady = 0;
    }
  }
  return arrivalQ->size;
 }
 return -1;
}


Process *schedule(Q *readyQ)
{
 if(readyQ->size != 0)
  {
   return Qpop(readyQ);
  }
  return NULL;
}

int CPUtick(Process *CPU,int currentQuantum,int quantumMax)
{
 //if nothing is on there dont do anything
 if(CPU == NULL)
  {
   return 0;
  }

  //if the scheduler is preemptive
 if(preempt == 1)
  {
   //if the quantum has hit 0
  if(currentQuantum-- == 0)
   {
    //reset quantum and return that the process should be preempted
    currentQuantum = quantumMax;
    return 2;
   }
  }

  //else if you dont preempt
  //if the burst hits 0
 if(CPU->burstQ->head->burst-- == 0)
  {
   //pop the burst node
   BQpop(CPU->burstQ);
   //if there is no bursts left
   if(CPU->burstQ->size == 0)
    {
     //you're done and you need to go to done queue
     return 1;
    }
   else
    {
     //else you need to go to the wait queue
     return 3;
    }
  }
  printf("CPU Burst left: %d ",CPU->burstQ->head->burst);
  //and keep going
  return 0;
}

int transferCPU(Process *CPU,int CPUstatus,Q *doneQ,Q *readyQ,Q *waitQ)
{
 switch(CPUstatus)
 {
  case 0:
  return 0;
  break;

  case 1:
  printf("Process ");
  printProcessStatus(CPU);
  printf(" -has finished\n");
  Qpush(CPU,doneQ);
  CPU = NULL;
  return 1;
  break;

  case 2:
  printf("Process ");
  printProcessStatus(CPU);
  printf(" -has been returned to readyQ\n");
  Qpush(CPU,readyQ);
  CPU = NULL;
  return 2;
  break;

  case 3:
  printf("Process ");
  printProcessStatus(CPU);
  printf(" -has been put to sleep\n");
  Qpush(CPU,waitQ);
  CPU = NULL;
  return 3;
  break;

  default:
  return -1;
  break;
 }
}

Q *waitQtick(Q *waitQ)
{
 Q *returnQ = malloc(sizeof(struct Q));
 struct QNode *cur = waitQ->head;
 for(int i = 0;i<waitQ->size;i++)
  {
   if(cur->process->burstQ->head->burst-- == 0)
    {
     BQpop(cur->process->burstQ);
     Qpush(Qremove(i,waitQ),returnQ);
     i--;
     continue;
    }
    cur = cur->next;
  }
 return returnQ;
}

void wake(Q *toReadyQ,Q *waitQ)
{
 for(int i = 0;i<toReadyQ->size;i++)
  {
   Process *wake = Qpop(toReadyQ);
   Qpush(wake,waitQ);
   printf("-Process ");
   printProcessStatus(wake);
   printf(" has been awoken\n\n");
   fflush(stdout);
  }
}

void printQfinal(Q *doneQ)
{
 printf("done");
}

int main( int argc,char *argv[])
{
 if(argc != 2 && argc != 3)
  {
   printf("usage:\n");
   return 0;
  }
  int quantum;
  Q *arrivalQ = malloc(sizeof(struct Q));
  Q *readyQ = malloc(sizeof(struct Q));
  Q *doneQ = malloc(sizeof(struct Q));
  Q *waitQ = malloc(sizeof(struct Q));
  Process *CPU = NULL;

  if(argc == 2)
   {
    preempt = 0;
    quantum = 0;
    parseInput(argv[1],arrivalQ);
   }
   else
   {
    preempt = 1;
    int quantum = atoi(argv[1]);
    parseInput(argv[2],arrivalQ);
   }

 printf("Arrival Queue: \n");
 printQ(arrivalQ);
 printf("\n----------------\n");

 int clockTick = 0;
 int totalJobs = arrivalQ->size;
 int currentQuantum = quantum;


 while(doneQ->size != totalJobs)
  {
   clockTick++;
   printf("\n-------------------------------------\n");
   printf("[---- Clock Cycle: %d ----]\n\n",clockTick);
   arrive(clockTick,arrivalQ,readyQ);
   int CPUstatus;
   if(CPU != NULL)
    {
     //cycle the cpu and return the CPU status
     printf("\nQUANTUM ELAPSED: %d\n",currentQuantum);
     CPUstatus = CPUtick(CPU,currentQuantum,quantum);
     transferCPU(CPU,CPUstatus,doneQ,readyQ,waitQ);
     if(CPUstatus != 0)
      {
       CPU = NULL;
      }
    }
   else
    {
     if(readyQ->size != 0)
     {
     CPU  = schedule(readyQ);
     printf("-Process ");
     printProcessStatus(CPU);
     printf(" -has entered CPU\n\n");
     fflush(stdout);
    }
    }
    printf("\n");
    Q *toReadyQ = waitQtick(waitQ);
    wake(toReadyQ,readyQ);

    printf("\n\n---CPU contents");
    printProcessStatus(CPU);
    printf("---WaitQ contents\n");
    printQ(waitQ);
    printf("---ReadyQ contents\n");
    printQ(readyQ);
    printf("---ArrivalQ contents\n");
    printQ(arrivalQ);
    printf("--DoneQ contents\n");
    printQ(doneQ);
  }
  printf("\n-----------------\nScheduler Status: \n");
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

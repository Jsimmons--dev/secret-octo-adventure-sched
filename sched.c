#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int preempt = 0;
static int debug = 0;
int CPUutil = 0;
typedef struct BNode
{
 int burst;
 struct BNode *next;
} BNode;

BNode *newBNode()
{
 BNode *newBNode = malloc(sizeof(BNode));
 if(newBNode == NULL)
  {
   printf("no memory allocated for new Burst Node\n");
   exit(-2);
  }
  newBNode->burst = 0;
  newBNode->next = NULL;
  return newBNode;
}

typedef struct BurstQ
{
 int size;
 struct BNode *head;
 struct BNode *tail;
} BurstQ;

BurstQ *newBurstQ()
{
 BurstQ *newBurstQ = malloc(sizeof(BurstQ));
 if(newBurstQ == NULL)
  {
   printf("no memory allocated for new burst queue\n");
   exit(-2);
  }
  newBurstQ->size = 0;
 return newBurstQ;
}

typedef struct Process
{
 int pid;
 int ArrivalTime;
 int numOfBursts;
 BurstQ *burstQ;

 int whenToWake;
 int timeOnCPU;
 int FirstOn;

 int TurnAroundTime;
 int WaitTime;
 int sleepTime;
 int responseTime;
} Process;

Process *newProcess()
{
 Process *newProcess = malloc(sizeof(Process));
 if(newProcess == NULL)
  {
   printf("no new memory allocated for new process\n");
   exit(-2);
  }
 newProcess->pid = 0;
 newProcess->ArrivalTime = 0;
 newProcess->numOfBursts = 0;
 newProcess->burstQ = newBurstQ();
 newProcess->timeOnCPU = 0;
 newProcess->FirstOn = -1;
 newProcess->whenToWake = 0;
 newProcess->TurnAroundTime = 0;
 newProcess->WaitTime = 0;
 newProcess->sleepTime = 0;
 newProcess->responseTime = 0;
 return newProcess;
}

typedef struct QNode
{
 Process *process;
 struct QNode *next;
} QNode;

QNode *newQNode()
{
 QNode *newQNode = malloc(sizeof(QNode));
 if(newQNode == NULL)
  {
   printf("no memory allocated for new queue node\n");
   exit(-2);
  }
  newQNode->process = newProcess();
  newQNode->next = NULL;
  return newQNode;
}

typedef struct Q
{
 int size;
 struct QNode *head;
 struct QNode *tail;
} Q;

Q *newQ()
{
 Q *newQ = malloc(sizeof(Q));
 if(newQ == NULL)
  {
   printf("no memory allocated for new queue");
   exit(-2);
  }
  newQ->size = 0;
  newQ->head = NULL;
  newQ->tail = NULL;
  return newQ;
}

int BQpush(int newInt,BurstQ *toQ)
{

 struct BNode *newNode = newBNode();
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
 else
   {
  if(i == 0)
  {
   return Qpop(q);
  }
  if(i < q->size)
   {
    struct QNode *cur = q->head;
    for(int j = 0; j<i-1;j++)
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
 QNode *newNode = newQNode();
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


void Qinsert(Process *CPU,Q *q)
{
 QNode *newNode,*left,*right;
 newNode = newQNode();
 newNode->process = CPU;

 left = right = q->head;

if(left == NULL)
 {
  Qpush(CPU,q);
  return;
 }
 right = left->next;
 if(right == NULL)
  {
   if(left->process->burstQ->head->burst <= CPU->burstQ->head->burst)
    {
     newNode->next = left;
     q->head = newNode;
     return;
    }
Qpush(CPU,q);
return;
  }
  while(right->process->burstQ->head->burst <= CPU->burstQ->head->burst)
   {
    left = right;
    right = right->next;
    if(right == NULL)
     {
      Qpush(CPU,q);
      return;
     }
     if(right->process->burstQ->head->burst > CPU->burstQ->head->burst)
      {
       left->next = newNode;
       newNode->next = right;
       return;
      }
   }
}


void printProcessStatus(Process *process)

{
 if(process != NULL)
  {
   if(debug == 1)
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
   printf("P: %d ",process->pid);
  }
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

int parseInput(char filename[],Q *arrivalQ)
{
 char *token;
 FILE *file = fopen (filename,"r");
 if (file != NULL)
  {
   char line[128];
   int pid = 0;
   while(fgets(line,sizeof(line),file) != NULL)
    {
     Process *process = newProcess();
     process->pid = pid;
     pid++;
     BurstQ *burstQ = newBurstQ();
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
    return 0;
  }
  return -1;
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
    printf(" -has arrived @time:%d\n",clockTick);
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
   return Qpop(readyQ);
}

int CPUtick(Process *CPU,int currentQuantum)
{
 //if nothing is on there dont do anything
 if(CPU == NULL)
  {
   return -1;
  }
 if(currentQuantum == 0 && preempt == 1) return 2;
 CPU->timeOnCPU++;
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
  if(debug == 1)
   {
  printf("CPU Burst left: %d ",CPU->burstQ->head->burst);
  }
  //and keep going
  return 0;
}

int transferCPU(Process *CPU,int CPUstatus,Q *doneQ,Q *readyQ,Q *waitQ,int clockTick)
{
 switch(CPUstatus)
 {

  case 0:
  return 0;
  break;

  case 1:
  printf("-Process ");
  printProcessStatus(CPU);
  printf(" -has finished @time: ");
  printf("%d\n",clockTick);
  CPU->TurnAroundTime = clockTick - CPU->ArrivalTime;
  CPU->WaitTime = CPU->TurnAroundTime - CPU->timeOnCPU - CPU->sleepTime;
  CPU->responseTime = CPU->FirstOn - CPU->ArrivalTime;
  printf("with TaT:%d WaitTime:%d I/O Wait:%d ResponseTime:%d\n\n",CPU->TurnAroundTime,CPU->WaitTime,CPU->sleepTime,CPU->responseTime);
  Qpush(CPU,doneQ);
  return 1;
  break;

  case 2:
  printf("-Process ");
  printProcessStatus(CPU);
printf(" -has returned to the ready queue @time: ");
printf("%d\n\n",clockTick);  Qpush(CPU,readyQ);
  return 2;
  break;

  case 3:
  printf("-Process ");
  printProcessStatus(CPU);
  CPU->whenToWake = clockTick + CPU->burstQ->head->burst;
  printf(" -has been put to sleep @time: ");
  printf("%d\n\n",clockTick);
  Qinsert(CPU,waitQ);
  return 3;
  break;

  default:
  return -1;
  break;
 }
}

Q *waitQtick(Q *waitQ)
{
 Q *returnQ = newQ();
 struct QNode *cur = waitQ->head;

 for(int i = 0;i<waitQ->size;i++)
  {
   cur->process->sleepTime++;
   if(cur->process->burstQ->head->burst-- == 0)
    {
     BQpop(cur->process->burstQ);
     Process *removed = Qremove(i,waitQ);
     Qpush(removed,returnQ);
     i--;
     continue;
    }
    cur = cur->next;
  }
 return returnQ;
}

void wake(Q *toReadyQ,Q *readyQ,int clockTick)
{
 for(int i = 0;i<toReadyQ->size;i++)
  {
   Process *wake = Qpop(toReadyQ);
   Qpush(wake,readyQ);
   printf("-Process ");
   printProcessStatus(wake);
   printf(" has been awoken @time: ");
   printf("%d\n\n",clockTick);
   fflush(stdout);
  }
}

void wakeUp(Q *waitQ,Q *readyQ,int clockTick)
{
 for(int i = 0;i<waitQ->size;i++)
  {
   Qpush(Qpop(waitQ),readyQ);
  }
 // if(waitQ->size > 0)
 //  {
 //   QNode *cur = waitQ->head;
 //   while(waitQ->head->process->whenToWake <= clockTick)
 //   {
 //    BQpop(waitQ->head->process->burstQ);
 //    Qpush(Qpop(waitQ),readyQ);
 //    if(waitQ->size == 0)
 //     {
 //      return;
 //     }
 //   }
 // }
}

void printQfinal(Q *doneQ,int clockTick)
{

 int avgTaT = 0;
 int maxTaT = 0;
 int avgWait = 0;
 int maxWait = 0;
 int avgRes = 0;
 int maxRes = 0;

 QNode *curNode = doneQ->head;
 for(int i = 0;i<doneQ->size;i++)
  {
   Process *curP = curNode->process;
   int curTaT = curP->TurnAroundTime;
   int curWait = curP->WaitTime;
   int curRes = curP->responseTime;
   avgTaT += curTaT;
   avgWait += curWait;
   avgRes += curRes;
   if(curTaT > maxTaT)
    maxTaT = curTaT;
   if(curWait > maxWait)
    maxWait = curWait;
   if(curRes > maxRes)
    maxRes = curRes;
   curNode = curNode->next;
  }
  avgTaT /= doneQ->size;
  avgWait /= doneQ->size;
  avgRes /= doneQ->size;

 printf("\n[ CPU Utilization: %.2f%% ]\n",((float)CPUutil/(float)clockTick)*100);
 printf("[ Average Turn Around Time: %d ]\n",avgTaT);
 printf("[ Maximum Turn Around Time: %d ]\n",maxTaT);
 printf("[ Average Wait Time: %d ]\n",avgWait);
 printf("[ Maximum Wait Time: %d ]\n",maxWait);
 printf("[ Average Response Time: %d ]\n",avgRes);
 printf("[ Maximum Response Time: %d ]\n",maxRes);
 printf("done!");
}

void printAll(Process *CPU,Q *waitQ,Q*readyQ,Q *arrivalQ,Q *doneQ)
{
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

int main( int argc,char *argv[])
{

 int quantum = 0;
 int contextSwitchHalf = 0;
 Q *arrivalQ = newQ();
 Q *readyQ = newQ();
 Q *doneQ = newQ();
 Q *waitQ = newQ();

 Process *CPU = NULL;


 if(argc != 4 && argc != 5)
  {
   printf("usage: sched [filename] [scheduler] [quantum] [context switch cost / 2]\n");
   exit(0);
  }
  if(strcmp(argv[2],"FCFS") != 0)
   {
    printf("Scheduler was not one of -FCFS\n");
    exit(0);
   }


  if(argc == 4)
   {
    preempt = 0;
    quantum = 0;
    contextSwitchHalf = atoi(argv[3]);
   }
   else
   {
    preempt = 1;
    quantum = atoi(argv[3]);
    contextSwitchHalf = atoi(argv[4]);
   }
   int parseStatus = parseInput(argv[1],arrivalQ);
     if(parseStatus == -1)
      {
       printf("file not found. exiting...\n");
       exit(-1);
      }

{ //debug print
 if(debug == 1)
 {
 printf("Arrival Queue: \n");
 printQ(arrivalQ);
 printf("\n----------------\n");
 }
}

 int totalJobs = arrivalQ->size;
 int currentQuantum = quantum;

 int clockTick = 0;
 int contextCost = 0;
 int CPUstatus = -1;
 while(doneQ->size < totalJobs-1)
  {
   clockTick++;

   if(debug == 1)
   {
    printf("\n-------------------------------------\n");
    printf("[---- Clock Cycle: %d ----]\n",clockTick);
    printf("\nQUANTUM ELAPSED: %d\n",currentQuantum);
    printf("\nCONTEXT ELAPSED: %d\n",contextCost);
   }

   //get new arrivals
  arrive(clockTick,arrivalQ,readyQ);

  //wake up any ready processes
  //Q *toReadyQ = waitQtick(waitQ);
  //wake(toReadyQ,readyQ,clockTick);
  wakeUp(waitQ,readyQ,clockTick);

   //if we're switching just loop
   if(contextCost != 0)
    {
     printf("switching...\n\n");
     contextCost--;
     continue;
    }


     //there isnt anything on the CPU
     if(CPU == NULL)
      {

      if(readyQ->size != 0)
       {
      CPU  = schedule(readyQ);
      printf("-Process ");
      printProcessStatus(CPU);
      if(CPU->FirstOn == -1)
      CPU->FirstOn = clockTick;
      printf(" -has entered CPU @time: ");
      printf("%d\n\n",clockTick);
      fflush(stdout);
      contextCost += contextSwitchHalf;
     }
    }
     else
      {
      //cycle the cpu and return the CPU status
      CPUstatus = CPUtick(CPU,currentQuantum);
      CPUutil++;
      transferCPU(CPU,CPUstatus,doneQ,readyQ,waitQ,clockTick);
    //if the current process is not staying on the CPU
    if(CPUstatus != 0)
     {
      //empty the CPU
      CPU = NULL;
      currentQuantum = quantum;
      //if the move incurs a context switch cost
      if(CPUstatus == 2 || CPUstatus == 3)
      contextCost += contextSwitchHalf;
     }
     else
      {
       currentQuantum--;
      }
     }




{ // debug print
    if(debug == 1)
    {
     printAll(CPU,waitQ,readyQ,arrivalQ,doneQ);
   }
  }
  }
  printf("\n-----------------\nScheduler Status: \n");
  printQfinal(doneQ,clockTick);
  printf("\n");
}

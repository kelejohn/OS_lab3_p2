#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#define NUM_LOOP 25 

int DepositMoney(int account){
    int balance = rand() % 101; //rand number from 0-100
    if (balance % 2 == 0){
      account += balance;
      printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
    }
    else{
      printf("Dear old Dad: Doesn't have any money to give\n");
    }
    return account;
}

int WithdrawMoney(int account, int amount){
  if (amount <= account){
    account -= amount;
    printf("Poor Student: Withdraws $%d / Balance = $%d\n", amount, account);
  }
  else{
    printf("Poor Student: Not Enough Cash ($%d)\n", account );
  }
  return account;
}


int main()
{
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int *BankAccount;
    int *Turn;

    ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (ShmID < 0) {
        perror("shmget");
        exit(1);
    }
    if (ShmPTR == (void*)-1) {
        perror("shmat error");
        exit(1);
    }

    BankAccount = &ShmPTR[0];
    Turn = &ShmPTR[1];

    *BankAccount = 0;
    *Turn = 0;

    pid = fork(); 
    
    if (pid < 0) 
    { 
      fprintf(stderr, "fork Failed" ); 
      return 1; 
    } 
    
    else if (pid > 0) 
    { 
      int i;
      srand(getpid());
      for (i = 0; i < NUM_LOOP; i++) {
        sleep(rand() % 6); //rand number from 0-5
        //parent
        //while not parent turn
        while (*Turn != 0){
          //busy
        }
        int account = *BankAccount;
        if (account <= 100){
          account = DepositMoney(account);
        }
        else{
          printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
        }
        *BankAccount = account;
        *Turn = 1; //child turn
      
      }

      wait(NULL); //wait for child
      shmdt((void *)ShmPTR);
      shmctl(ShmID, IPC_RMID, NULL); //removed shared memory
    }
    //child
    else
    {
      int i;
      srand(getpid());

      for (i = 0; i < NUM_LOOP; i++) {
        sleep(rand() % 6);
        //while not child turn
        while (*Turn != 1){
          //wait
        }

        int account = *BankAccount;
        int balance = rand() % 51; //rand number from 0-50
        printf("Poor Student needs $%d\n", balance);
        account = WithdrawMoney(account, balance);
        *BankAccount = account;
        *Turn = 0; //give turn back to parent
      }

    shmdt((void*) ShmPTR);
    }

    return 0;
}
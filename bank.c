#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

int DepositMoney(int account){
    int balance = rand() % 101;
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
    int    ShmID;
    int    *ShmPTR;
    pid_t  pid;
    int BankAccount = 0;
    int Turn = 0;

    ShmID = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    ShmPTR = (int *) shmat(ShmID, NULL, 0);
    if (*ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }

    ShmPTR[0] = BankAccount;
    ShmPTR[1] = Turn;

    pid = fork(); 
    
    if (pid < 0) 
    { 
      fprintf(stderr, "fork Failed" ); 
      return 1; 
    } 
    
    else if (pid > 0) 
    { 
      int i;
      for (i = 0; i < 25; i++) {
        sleep(rand() % 6);
        int account = ShmPTR[0];
        while (ShmPTR[1] != 1){
          if (account <= 100){
            account = DepositMoney(account);
          }
          else{
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
          }
          ShmPTR[0] = account;
          ShmPTR[1] = 1;
        }
      }
    }
    else
    {
      int i;
      for (i = 0; i < 25; i++) {
        sleep(rand() % 6);
        int account = ShmPTR[0];
        while (ShmPTR[1] != 0){
          int balance = rand() % 51;
          printf("Poor Student needs $%d\n", balance);
          account = WithdrawMoney(account, balance);
          ShmPTR[0] = account;
          ShmPTR[1] = 0;
        }
      }
    }
    shmdt((void *) ShmPTR);
    shmctl(ShmID, IPC_RMID, NULL);

    return 0;
}
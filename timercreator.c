#include <unistd.h> 
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>

#define W_LEN ( 1 << 10)

void SignHandler(int iSignNo);
void testTimerSign();
void printTime();

uint64_t rdtsc()
{
  uint64_t ret = 0;
  __asm__ __volatile__(
//    "mfence\n\t"
    "rdtscp"
    : "=a" (ret)
    : 
    : //"%rax"
  );
  
  return ret;
}

const int NUMBER = 1;
uint64_t signaltime[10]; 
uint64_t begintime[10];
uint64_t starttime;
int count = 0;
int fd;
char buf[W_LEN] = {0};

int main() {
    printf("%lu\n", rdtsc());
    fd = open("output", O_WRONLY  | O_CREAT);
    if (fd == -1){
        return EXIT_FAILURE;
    } 

    testTimerSign();
    while(count < NUMBER){
        starttime = rdtsc();
        begintime[count] = rdtsc();
         //int left = sleep(5);
        write(fd, buf, W_LEN);
         //printTime();
        signaltime[count++] = rdtsc();
    }
    
    return 0; 
}

void SignHandler(int iSignNo){
    //printTime();
    uint64_t signaltime = rdtsc();
    printf("TSC when entering sig handler: %lu \n", signaltime);
    printf("Delay %lu \n", signaltime-starttime);
    if(iSignNo == SIGUSR1){
        printf("Capture sign no : SIGUSR1\n"); 
    }else if(SIGALRM == iSignNo){
//        printf("Capture sign no : SIGALRM\n"); 
    }else{
        printf("Capture sign no:%d\n",iSignNo); 
    }
}

void testTimerSign(){
    struct sigevent evp;  
    struct itimerspec ts;  
    timer_t timer;  
    int ret;  
    evp.sigev_value.sival_ptr = &timer;  
    evp.sigev_notify = SIGEV_SIGNAL;  
    evp.sigev_signo = SIGALRM;
    signal(evp.sigev_signo, SignHandler); 
    ret = timer_create(CLOCK_REALTIME, &evp, &timer);  
    if(ret) {
        perror("timer_create");
    } 
    
    int begin, end;
    begin = rdtsc();
    
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 1000;  
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 1;
      
    //printTime();
    //printf("start\n");
    ret = timer_settime(timer, 0, &ts, NULL);  
    
    end = rdtsc();
    if(ret) {
        perror("timer_settime"); 
    } 
    //printf("settime delay: %d\n", end-begin);
}

void printTime(){
    struct tm *cursystem;
    time_t tm_t;
    time(&tm_t);
    cursystem = localtime(&tm_t);
    char tszInfo[2048] ;
    sprintf(tszInfo, "%02d:%02d:%02d", 
        cursystem->tm_hour, 
        cursystem->tm_min, 
        cursystem->tm_sec);
        printf("[%s]",tszInfo);
}
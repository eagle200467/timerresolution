//timercreate_demo.cpp
#include <unistd.h> 
#include <stdio.h>
#include <signal.h>
#include <time.h>

#define W_LEN ( 1 << 10)

void SignHandler(int iSignNo);
void testTimerSign();
void printTime();

int rdtsc()
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
int signaltime[10]; int begintime[10];
int count = 0;
int fd;
char buf[W_LEN] = {0};

int main() {
    printf("%d\n", rdtsc());
    fd = open("output", O_WRONLY);
    if (fd == -1){
        return EXIT_FAILURE;
    } 

    testTimerSign();
    while(count < NUMBER){
         begintime[count] = rdtsc();
         //int left = sleep(5);
         write(fd, buf, W_LEN);
         //printTime();
         signaltime[count++] = rdtsc();
 //        printf("======  in loop: %d\n", signaltime[count-1]);
//         printf("sleep(5)(left=%d)\n", left);
    }
    
    int i;
    for(i = 0; i < count; i++)
    {
      printf("timestamp %d: %lu -- %lu (delay: %lu)\n", i, begintime[i], signaltime[i], signaltime[i]-begintime[i]);
    }
    printf("%d\n", rdtsc());
    return 0; 
}

void SignHandler(int iSignNo){
    //printTime();
    printf("TSC when entering sig handler: %lu \n", rdtsc());
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
    ts.it_interval.tv_nsec = 100;  
    ts.it_value.tv_sec = 0;
    ts.it_value.tv_nsec = 10;  
    //printTime();
    //printf("start\n");
    ret = timer_settime(timer, 0, &ts, NULL);  
    
    end = rdtsc();
    if(ret) {
        perror("timer_settime"); 
    } 
    printf("settime delay: %d\n", end-begin);
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <pigpio.h>
#include <time.h>
#include <signal.h>

#define GPIO_IN 23
#define GPIO_OUT 24
#define BATCH_COUNT 100
#define PULSE_COUNT 10000
#define SLEEP_INTERVAL 2000

// volatile sig_atomic_t signal_received = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct timespec write_moment, handle_moment;
long int elapsed_times[PULSE_COUNT];
int count = 0;

void* blink_thread(void* arg);
void handle_input(int gpio, int level, uint32_t tick);
void test(int index);

int main() {
    if (gpioInitialise() == PI_INIT_FAILED) {
      printf("ERROR: Failed to initialize the GPIO interface.\n");
      return 1;
    }
    int err;

    printf("Set mode.\n");
    err = gpioSetMode(GPIO_OUT, PI_OUTPUT);
    if(err) {
        printf("1: %i", err);
        return err;
    }
    err = gpioSetMode(GPIO_IN, PI_INPUT);
    if(err) {
        printf("2: %i", err);
        return err;
    }
    err = gpioSetPullUpDown(GPIO_IN, PI_PUD_UP);
    if(err) {
        printf("3: %i", err);
        return err;
    }

    printf("Set interrupt.\n");
    err = gpioSetISRFunc(GPIO_IN, EITHER_EDGE, 0, handle_input);
    if(err) {
        printf("4: %i", err);
        return err;
    }

    // Wait for ISR to be handled. 
    // Sometimes it seems that the thread starts too soon for it to handle input.
    sleep(1); 
    for(int i = 0; i < BATCH_COUNT; ++i) {
        test(i);
    }
    gpioSetMode(GPIO_OUT, PI_INPUT);
    printf("Terminate.\n");
    gpioTerminate();
    return 0;
}

void test(int index) {
    count = 0;
    printf("Spawn thread.\n");
    pthread_t blink_tid;
    pthread_create(&blink_tid, NULL, blink_thread, NULL);
    pthread_join(blink_tid, NULL);
    printf("Joined thread. %i/%i tests tried\n", count, PULSE_COUNT);

    pthread_mutex_lock(&mutex); // `handle_input` might still be doing work
    char filename[50];
    sprintf(filename, "test_output/read_interval_%i.csv", index);
    FILE *f = fopen(filename, "w");
    fprintf(f, ",n_secs_delay,\n");
    for(int i=0; i<count; i++){
        printf("%i: %li\n", i, elapsed_times[i]);
        fprintf(f, "%i,%li,\n", i, elapsed_times[i]);
    }
    pthread_mutex_unlock(&mutex);
    fclose(f);
}

void* blink_thread(void* arg) {
    
    printf("Start blinking.\n");
    for(int i = 0; i < PULSE_COUNT; ++i) {
        pthread_mutex_lock(&mutex);
        if(i%2) {
            clock_gettime(CLOCK_MONOTONIC_RAW, &write_moment);
            gpioWrite(GPIO_OUT, PI_HIGH);
        } else {
            clock_gettime(CLOCK_MONOTONIC_RAW, &write_moment);
            gpioWrite(GPIO_OUT, PI_LOW);
        }
        pthread_mutex_unlock(&mutex);
        struct timespec interval = { 
            .tv_sec = 0,
            .tv_nsec = SLEEP_INTERVAL,
        };
        nanosleep(&interval, NULL);
    }
    printf("Stop blinking.\n");
    return NULL;
}

void handle_input(int gpio, int level, uint32_t tick) {
    clock_gettime(CLOCK_MONOTONIC_RAW, &handle_moment);

    pthread_mutex_lock(&mutex);
    long int elapsed_time = (handle_moment.tv_sec - write_moment.tv_sec) * 1000000;
    elapsed_time += (handle_moment.tv_nsec - write_moment.tv_nsec) ;
    if(count < PULSE_COUNT) {
        elapsed_times[count] = elapsed_time;
    }
    ++count;
    pthread_mutex_unlock(&mutex);
}
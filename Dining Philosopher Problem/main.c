//
//  main.c
//  Dining Philosopher Problem
//
//  Created by Jonathan Thomas on 4/23/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *philosopher(void *params);
void pickup_forks(int phil_num);
void putdown_forks(int phil_num);
void eating(int sleep_time);
void thinking(int sleep_time);
int left_neighbor(int phil_num);
int right_neighbor(int phil_num);
void test(int phil_num);

const int NUMBER_OF_PHILOSOPHERS = 5;
const int MAX_SLEEP_TIME = 5;

enum {THINKING, EATING, HUNGRY} state[NUMBER_OF_PHILOSOPHERS];

int forks[] = {1, 2, 3 ,4 ,5};
pthread_t philosophers[NUMBER_OF_PHILOSOPHERS];
pthread_cond_t cond_var[NUMBER_OF_PHILOSOPHERS];

pthread_mutex_t mutex_lock;

int main(int argc, const char * argv[]) {
    // insert code here...
    pthread_mutex_init(&mutex_lock, NULL);
    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        pthread_cond_init(&cond_var[i], NULL);
    }
    
    int phil_num[5];
    
    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        phil_num[i] = i;
        pthread_create(&philosophers[i], 0, *philosopher, (void *) &phil_num[i]);
    }
    
    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }
    return 0;
}

void *philosopher(void *params) {
    int phil_num = *(int *)params;
    int sleep_time;
    int times_through_loop = 0;
    
    srandom((unsigned)time(NULL));
    
    while (times_through_loop < 5) {
        sleep_time = (int) ((random() % MAX_SLEEP_TIME) + 1);
        
        printf("Philosopher %d is THINKING\n", phil_num);
        thinking(sleep_time);
        
        
        pickup_forks(phil_num);
        printf("Philosopher %d is EATING\n", phil_num);
        eating(sleep_time);
        
        printf("Left of %d is ", phil_num);
        if (state[left_neighbor(phil_num)] == THINKING) {
            printf("THINKING\n");
        }
        else if (state[left_neighbor(phil_num)] == HUNGRY) {
            printf("HUNGRY\n");
        }
        else {
            printf("EATNG\n");
        }
        printf("Right of %d is ", phil_num);
        if (state[right_neighbor(phil_num)] == THINKING) {
            printf("THINKING\n\n");
        }
        else if (state[right_neighbor(phil_num)] == HUNGRY) {
            printf("HUNGRY\n\n");
        }
        else {
            printf("EATNG\n\n");
        }
        
        putdown_forks(phil_num);
        
        times_through_loop++;
        
    }
    pthread_exit(0);
}

void pickup_forks(int phil_num) {
    pthread_mutex_lock(&mutex_lock);
    
    state[phil_num] = HUNGRY;
    test(phil_num);
    
    while (state[phil_num] != EATING) {
        pthread_cond_wait(&cond_var[phil_num], &mutex_lock);
    }
    
    pthread_mutex_unlock(&mutex_lock);
}

void putdown_forks(int phil_num) {
    pthread_mutex_lock(&mutex_lock);
    
    state[phil_num] = THINKING;
    test(left_neighbor(phil_num));
    test(right_neighbor(phil_num));
    
    pthread_mutex_unlock(&mutex_lock);
}

void eating(int sleep_time) {
    sleep(sleep_time);
}

void thinking(int sleep_time) {
    sleep(sleep_time);
}

int left_neighbor(int phil_num) {
    if (phil_num == 0) {
        return NUMBER_OF_PHILOSOPHERS - 1;
    }
    return phil_num - 1;
}

int right_neighbor(int phil_num) {
    if (phil_num == NUMBER_OF_PHILOSOPHERS - 1) {
        return 0;
    }
    return phil_num + 1;
}

void test(int phil_num) {
    if (state[left_neighbor(phil_num)] != EATING && state[phil_num] == HUNGRY && state[right_neighbor(phil_num)] != EATING) {
        state[phil_num] = EATING;
        pthread_cond_signal(&cond_var[phil_num]);
    }
}

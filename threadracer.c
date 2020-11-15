#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>

#define NVALS 8
#define NJOBS 10000000

pthread_mutex_t mjob;
unsigned remain_jobs = NJOBS;

int sum(int *buf, int size) {
    // sum all of the values in an int array
    int tot = 0;
    for (int i = 0; i < size; i ++) {
        tot += buf[i];
    }
    return tot;
}

void *jobfun(void *args) {
    int buf[NVALS];
    int jobs_complete = 0;
    int pos;

    while (remain_jobs > 0) {
        // grab a job
        pthread_mutex_lock(&mjob);
        remain_jobs --;
        pthread_mutex_unlock(&mjob);

        // reset position
        pos = 0;

        // reset buffer
        for (int i = 0; i < NVALS; i ++) { buf[i] = 1; }

        while (sum(buf, NVALS) > 0) {
            int v1 = buf[pos];
            int v2 = buf[pos + 1];
            buf[pos + 2]  += (v1 > 0 ? v1 : 0) + (v2 > 0 ? v2 : 0);
            pos = (pos + 1) % (NVALS - 2);
        }

        jobs_complete ++;
    }

    printf("jobs_complete %d\n", jobs_complete);
    return 0;
}

int main() {
    int n_cores = sysconf(_SC_NPROCESSORS_ONLN); // get number of system cores
    struct timeval start_time;
    struct timeval end_time;
    pthread_t threads[n_cores];

    pthread_mutex_init(&mjob, NULL);
    gettimeofday(&start_time, NULL);

    printf("racing on %d threads...\n\n", n_cores);

    // run all workers
    for (int i = 0; i < n_cores; i ++) { pthread_create(&threads[i], NULL, jobfun, NULL); }

    // wait for all workers
    for (int i = 0; i < n_cores; i ++) { pthread_join(threads[i], NULL); }

    gettimeofday(&end_time, NULL);

    printf("\ntotal time : %fs\n",
        ((double)(end_time.tv_usec - start_time.tv_usec)/1000000)
        + (double)(end_time.tv_sec - start_time.tv_sec));

    // clean up mutex
    pthread_mutex_destroy(&mjob);

    return 0;
}

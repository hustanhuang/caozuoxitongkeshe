#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


// data for each thread
typedef struct thread_data {
    int start;
    int end;
} thread_data;


// global sum with mutex lock
int sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


// thread function
void *thread_func(void *ptr) {
    // get thread data
    thread_data data = *(thread_data *) ptr;

    // calculate thread-local sum
    int local_sum = 0;
    for (int i = data.start; i < data.end; ++i)
        local_sum += i;

    // add local sum to global sum
    pthread_mutex_lock(&mutex);
    sum += local_sum;
    pthread_mutex_unlock(&mutex);
    return NULL;
}


int multisum(int n, int m) {
    if (n < 1) {
        fprintf(stderr, "need at least one thread to calculate\n");
        return 0;
    }

    // allocate threads and thread_data
    pthread_t *threads = malloc(sizeof(pthread_t) * n);
    thread_data *thread_data = malloc(sizeof(thread_data) * n);

    // schedule work for each thread
    int work_size = m / n;
    for (int i = 0; i < n - 1; i++) {
        thread_data[i].start = i * work_size + 1;
        thread_data[i].end = (i + 1) * work_size + 1;
    }
    thread_data[n - 1].start = (n - 1) * work_size + 1;
    thread_data[n - 1].end = m + 1;

    // create threads
    for (int i = 0; i < n; i++) {
        if (pthread_create(threads + i, NULL, thread_func, (void *) (thread_data + i)) != 0) {
            fprintf(stderr, "failed to create threads[%d]\n", i);
            goto FREE_THREADS;
        }
    }

    // join threads
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    // cleanup
    FREE_THREADS:
    free(thread_data);
    free(threads);

    // output
    return sum;
}

int main() {
    FILE *fin = fopen("input.txt", "r");
    int n, m;
    fscanf(fin, "N=%d\n", &n);
    fscanf(fin, "M=%d\n", &m);
    fclose(fin);

    printf("n = %d\n", n);
    printf("m = %d\n", m);
    int output = multisum(n, m);

    FILE *fout = fopen("output.txt", "w");
    fprintf(fout, "%d\n", output);
    printf("sum = %d\n", output);
    return 0;
}

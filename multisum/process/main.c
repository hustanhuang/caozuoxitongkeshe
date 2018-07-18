#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


// data for each process
typedef struct process_data {
    int start;
    int end;
} process_data;


void child_process_func(int out_fd, process_data data)
{
    // calculate child process sum
    int local_sum = 0;
    for (int i = data.start; i < data.end; ++i)
        local_sum += i;

    write(out_fd, &local_sum, sizeof(local_sum));
    close(out_fd);
}


int multisum(int n, int m) {
    if (n < 1) {
        fprintf(stderr, "need at least one process to calculate\n");
        return 0;
    }

    // allocate pid, pipes, and process_data
    int *fds = malloc(sizeof(int) * n * 2);
    for (int i = 0; i < n; ++i)
        pipe(fds + 2 * i);
    process_data *process_data = malloc(sizeof(process_data) * n);

    // schedule work for each process
    int work_size = m / n;
    for (int i = 0; i < n - 1; i++) {
        process_data[i].start = i * work_size + 1;
        process_data[i].end = (i + 1) * work_size + 1;
    }
    process_data[n - 1].start = (n - 1) * work_size + 1;
    process_data[n - 1].end = m + 1;

    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            child_process_func(fds[2 * i + 1], process_data[i]);
            goto CHILD;
        }
    }

    // main process read pipes and close pipes
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        int read_fd = fds[2 * i];
        int child_sum;
        read(read_fd, &child_sum, sizeof(child_sum));
        sum += child_sum;
        close(read_fd);
    }

    // main process cleanup
    free(fds);
    free(process_data);
    return sum;

    // child process cleanup
    CHILD:
    free(fds);
    free(process_data);
    exit(EXIT_SUCCESS);
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

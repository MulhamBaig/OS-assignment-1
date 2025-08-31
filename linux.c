#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

#define NUM_MSGS 1000000
int main()
{
    int p[2];
    if (pipe(p) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }

    if (pid == 0)
    {
        // ---------- CHILD (Reader) ----------
        close(p[1]); // close write end

        int msg;
        for (int i = 0; i < NUM_MSGS; i++)
        {
            if (read(p[0], &msg, sizeof(msg)) <= 0)
            {
                perror("read");
                exit(1);
            }
        }
        close(p[0]);
        exit(0);
    }
    else
    {
        // ---------- PARENT (Writer) ----------
        close(p[0]); // close read end

        struct timeval start, end;
        gettimeofday(&start, NULL);

        for (int i = 0; i < NUM_MSGS; i++)
        {
            if (write(p[1], &i, sizeof(i)) <= 0)
            {
                perror("write");
                exit(1);
            }
        }
        close(p[1]);

        wait(NULL); // wait for child to finish

        gettimeofday(&end, NULL);

        double time_taken = (end.tv_sec - start.tv_sec) +
                            (end.tv_usec - start.tv_usec) / 1000000.0;

        printf("Total time = %.6f seconds\n", time_taken);
        printf("Requests per second = %.2f\n", NUM_MSGS / time_taken);
        printf("Average time per message = %.6f microseconds\n",
               (time_taken * 1e6) / NUM_MSGS);
    }

    return 0;
}

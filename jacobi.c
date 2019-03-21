/**
 * File              : jacobi.c
 * Author            : recurze
 * Date              : 23:25 21.03.2019
 */

#include "types.h"
#include "user.h"

#define N 10
#define E 0.00001
#define T 100.0
#define P 2
#define L 20000

inline void amax(float *a, float b) { if (b > *a) *a = b; }
inline float abs(float a) { if (a > 0) return a; return -a; }

int parent_pid;

int convergence(float del, int pid) {
    // send del to parent for checking convergence condition
    struct Message msg = (struct Message){
        .data = del,
        .sender = pid
    };

    send(pid, parent_pid, &msg);
    recv(&msg);

    return msg.data;
}

void send_ghost_points(int pid, float u[][N], int rows) {
    if (pid != parent_pid + 1) {
        for (int i = 0; i < N; ++i) {
            struct Message msg = (struct Message){
                .data = u[1][i],
                .sender=pid
            };
            send(pid, pid - 1, &msg);
        }
    }

    if (pid != parent_pid + P - 1) {
        for (int i = 0; i < N; ++i) {
            struct Message msg = (struct Message){
                .data = u[rows][i],
                .sender=pid
            };
            send(pid, 1 + pid, &msg);
        }
    }
}

void recv_ghost_points(int pid, float u[][N], int rows) {
    int n = 0;
    if (pid != parent_pid + 1) n += N;
    if (pid != parent_pid + P - 1) n += N;

    struct Message *msg = (struct Message *) malloc(sizeof(struct Message));
    for (int i = 0, j = 0, k = 0; k < n; ++k) {
        recv(msg);
        if (msg->sender == pid + 1) {
            u[rows + 1][i++] = msg->data;
        } else {
            u[0][j++] = msg->data;
        }
    }
}

void work_child() {
    int pid = getpid();
    int rows = (N - 2)/(P - 1);
    if (pid == parent_pid + P) {
        rows += (N - 2)%(P - 1);
    }

    float u[rows + 2][N];
    for (int i = 1; i <= rows; ++i) {
        u[i][0] = u[i][N - 1] = T;
        for (int j = 1; j < N - 1; ++j) {
            u[i][j] = 3*T/4;
        }
    }

    float del = 0;
    float w[rows][N];
    do {
        send_ghost_points(pid, u, rows);
        recv_ghost_points(pid, u, rows);

        del = 0;
        for (int i = 1; i <= rows; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                w[i][j] = (u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1])/4;
                amax(&del, abs(w[i][j] - u[i][j]));
            }
        }

        for (int i = 1; i <= rows; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                u[i][j] = w[i][j];
            }
        }

    } while (!convergence(del, pid));
}

void work_parent() {
    int converged = 1;
    struct Message * msg = (struct Message*) malloc(sizeof(struct Message));
    int it = 0;
    do {
        for (int i = 0; i < P - 1; ++i) {
            recv(msg);
            printf(1, "Iteration: %d %d\n", it++, (int)msg->data);
            converged &= (msg->data < E);
        }
        msg->data = converged;
        msg->sender = parent_pid;
        for (int i = 1; i < P; ++i) {
            send(parent_pid, parent_pid + i, msg);
        }
    } while(!converged);
}

void read_parameters() {

}

int main(int argc, char **argv) {
    parent_pid = getpid();

    read_parameters();

    for (int child = 0; child < P - 1; ++child) {
        if (!fork()) {
            work_child();
        }
    }

    work_parent();
    exit();
}

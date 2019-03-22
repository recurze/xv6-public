/**
 * File              : jacobi.c
 * Author            : recurze
 * Date              : 23:25 21.03.2019
 */

#include "types.h"
#include "user.h"

#define N 5
#define E 0.0001
#define T 100.0
#define P 7
#define L 20000

inline void amax(float *a, float b) { if (b > *a) *a = b; }
inline float abs(float a) { if (a > 0) return a; return -a; }

int parent_pid;

int convergence(float del, int pid) {
    // send del to parent for checking convergence condition
    struct Message msg = (struct Message) {
        msg.data = del,
        msg.sender = pid
    };

    send(pid, parent_pid, &msg);
    recv(&msg, 1);
    if (msg.sender != parent_pid) {
        printf(1, "Something's wrong for %d\n", pid);
    }

    return msg.data;
}

void send_ghost_points(int pid, float u[][N], int rows) {
    if (pid != parent_pid + 1) {
        struct Message msg;
        for (int i = 0; i < N; ++i) {
            msg.data = u[1][i];
            msg.sender = pid;
            send(pid, pid - 1, &msg);
        }
    }

    if (pid != parent_pid + P - 1) {
        struct Message msg;
        for (int i = 0; i < N; ++i) {
            msg.data = u[rows][i];
            msg.sender = pid;
            send(pid, pid + 1, &msg);
        }
    }
}

void recv_ghost_points(int pid, float u[][N], int rows) {
    int n = 0;
    if (pid != parent_pid + 1) n += N;
    if (pid != parent_pid + P - 1) n += N;

    struct Message *msg = (struct Message *) malloc(sizeof(struct Message));
    for (int i = 0, j = 0, k = 0; k < n; ++k) {
        recv(msg, 0);
        if (msg->sender == pid + 1) {
            u[rows + 1][i++] = msg->data;
        } else {
            u[0][j++] = msg->data;
        }
    }
}

void print_grid(float u[][N], int rows) {
    for (int i = 1; i <= rows; ++i) {
        for (int j = 0; j < N; ++j) {
            printf(1, "%d ", (int) u[i][j]);
        }
        printf(1, "\n");
    }
}

void work_child() {
    int pid = getpid();
    int rows = (N - 2)/(P - 1);
    if (pid == parent_pid + P - 1) {
        rows += (N - 2)%(P - 1);
    }

    float u[rows + 2][N];
    if (pid == parent_pid + 1) {
        for (int i = 0; i < N; ++i) {
            u[0][i] = T;
        }
    }

    for (int i = 1; i <= rows; ++i) {
        u[i][0] = u[i][N - 1] = T;
        for (int j = 1; j < N - 1; ++j) {
            u[i][j] = 3*T/4;
        }
    }

    if (pid == parent_pid + P - 1) {
        for (int i = 0; i < N; ++i) {
            u[0][rows + 1] = 0;
        }
    }

    float del = 0;
    float w[rows + 2][N];
    struct Message *msg = (struct Message*) malloc(sizeof(struct Message));
    do {
        recv(msg, 1);
        send(pid, parent_pid, msg);

        send_ghost_points(pid, u, rows);
        recv_ghost_points(pid, u, rows);

        del = 0;
        for (int i = 1; i <= rows; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                w[i][j] = (u[i-1][j] + u[i+1][j]
                         + u[i][j-1] + u[i][j+1])/4;
                amax(&del, abs(w[i][j] - u[i][j]));
            }
        }

        for (int i = 1; i <= rows; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                u[i][j] = w[i][j];
            }
        }

    } while (!convergence(del, pid));

    recv(msg, 1);
    print_grid(u, rows);
    send(pid, parent_pid, msg);
}

void work_parent() {
    int converged = 1;
    struct Message * msg = (struct Message*) malloc(sizeof(struct Message));
    int it = 0;
    do {
        for (int i = P - 1; i; --i) {
            send(parent_pid, parent_pid + i, msg);
            recv(msg, 0);
        }
        converged = 1;
        for (int i = 0; i < P - 1; ++i) {
            recv(msg, 0);
            converged &= (msg->data < E);
        }

        msg->data = converged || (it > L);
        msg->sender = parent_pid;
        for (int i = P - 1; i; --i) {
            send(parent_pid, parent_pid + i, msg);
        }
    } while(!converged);

    for (int i = 0; i < N; ++i) {
        printf(1, "%d ", (int) T);
    }
    printf(1, "\n");

    for (int i = 1; i < P; ++i) {
        send(parent_pid, parent_pid + i, msg);
        recv(msg, 0);
    }

    printf(1, "%d ", (int) T);
    for (int i = 1; i < N; ++i) {
        printf(1, "%d ", 0);
    }
    printf(1, "\n");
}

void read_parameters() {

}

int main(int argc, char **argv) {
    parent_pid = getpid();

    read_parameters();

    for (int child = 0; child < P - 1; ++child) {
        if (!fork()) {
            work_child();
            exit();
        }
    }

    work_parent();
    exit();
}

#include "types.h"
#include "stat.h"
#include "user.h"

#define N_PROCESSES 8
int compute_sum(short *, int);

int main(int argc, char *argv[]) {
	if(argc< 2){
		printf(1,"Need type and input filename\n");
		exit();
	}
	char *filename;
	filename=argv[2];
	int type = atoi(argv[1]);
	printf(1,"Type is %d and filename is %s\n",type, filename);

	int tot_sum = 0;	
	float variance = 0.0;

	int size=1000;
	short arr[size];
	char c;
	int fd = open(filename, 0);
	for(int i=0; i<size; i++){
		read(fd, &c, 1);
		arr[i]=c-'0';
		read(fd, &c, 1);
	}	
  	close(fd);
  	// this is to supress warning
  	printf(1,"first elem %d\n", arr[0]);
  
  	//----FILL THE CODE HERE for unicast sum and multicast variance
  	//------------------

  	if(type==0){ //unicast sum
        tot_sum = compute_sum(arr, size);
		printf(1,"Sum of array for file %s is %d\n", filename,tot_sum);
	}
	else{ //mulicast variance
		printf(1,"Variance of array for file %s is %d\n", filename,(int)variance);
	}
	exit();
}

char * itoa(int);
int accumulate_range(short *, int, int);
int compute_sum(short * arr, int size) {
    int parent_pid = getpid();
    int b_sz = size/(N_PROCESSES - 1);
    for (int i=0, l=0, r=b_sz; i < N_PROCESSES-1; ++i, l+=b_sz, r+=b_sz) {
        if (!fork()) {
            if (i == N_PROCESSES - 2) r = size;
            int sum = accumulate_range(arr, l, r);
            printf(1, "%d %d %d\n", l, r, sum);
            send(getpid(), parent_pid, itoa(sum));
            exit();
        }
    }

    int total = 0;
    char * msg = (char *) malloc(MSGSIZE);
    for (int i = 0; i < N_PROCESSES - 1; ++i) {
        recv(msg);
        total += atoi(msg);
    }
    return total;
}

int accumulate_range(short * arr, int l, int r) {
    int sum = 0;
    for (int i = l; i < r; sum += arr[i++]);
    return sum;
}

char * itoa(int n) {
    char * s = (char *) malloc(MSGSIZE);

    int i = 0;
    while (n) {
        s[i++] = '0' + n%10;
        n /= 10;
    }

    strrev(s, i);
    return s;
}

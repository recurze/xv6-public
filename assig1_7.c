#include "types.h"
#include "stat.h"
#include "user.h"

int main(void)
{
	printf(1,"%s\n","IPC Test case");
	
	int cid = fork();
	if(cid==0){
		// This is child
		char *msg = (char *)malloc(MSGSIZE);
		recv(msg);
		printf(1,"2 CHILD: msg recv is: %s \n", msg );

		exit();
	}else{
		// This is parent
		char *msg_child = (char *)malloc(MSGSIZE);
		msg_child = "P";
        sleep(100);
		send(getpid(),cid,msg_child);	
		printf(1,"1 PARENT: msg sent is: %s \n", msg_child );
		
		free(msg_child);
        wait();
	}
	
	exit();
    return 0;
}
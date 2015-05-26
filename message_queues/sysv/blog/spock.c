#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/* 在kirk.c和spock.c中下面两个宏必须保持一致 */
#define IPC_KEY_FILE    "kirk.c"
#define IPC_PJID        (10)

#define MAXTEXT	(512)
struct msgbuf {
	long mtype;
	char mtext[MAXTEXT + 1];
};

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
	struct msgbuf buf;
    int msqid, msg_flags = 0;	
	key_t key;

	/* get key */
	if ((key = ftok(IPC_KEY_FILE, IPC_PJID)) == (key_t)-1) {
		err_sys("fotk error");	
	}
	/* get msqid */
	if ((msqid = msgget(key, msg_flags | 0444)) == -1) {	/* r--r--r-- */
		err_sys("msgget error");
	} 

	while (1) {
		if (msgrcv(msqid, &buf, sizeof(struct msgbuf), 0, 0) == -1) {
			err_sys("msgrcv error");
		}
		printf("[%s]\n", buf.mtext);
	}	

	exit(EXIT_SUCCESS);
}

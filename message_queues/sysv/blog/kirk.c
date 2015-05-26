#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

/* 在kirk.c和spock.c中下面两个宏必须保持一致 */
#define IPC_KEY_FILE	"kirk.c"
#define IPC_PJID		(10)

#define MAXTEXT	(512)
struct msgbuf {
	long mtype;					/* msg type */
	char mtext[MAXTEXT + 1];	/* msg body */
};

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
	struct msgbuf buf;
//	int msqid, msg_flags = IPC_CREAT | IPC_EXCL;	/* IPC_EXCL确保IPC对象是本进程创建 */
	int msqid, msg_flags = IPC_CREAT;	/* IPC_EXCL确保IPC对象是本进程创建 */
	key_t key;
	
	/* get key */
	if ((key = ftok(IPC_KEY_FILE, IPC_PJID)) == (key_t)-1) {
		err_sys("ftok error");
	}
	/* get msqid */
	if ((msqid = msgget(key, msg_flags | 0666)) == -1) {	/* rw-rw-rw- */
		err_sys("msgget error");
	}

	buf.mtype = 1;	/* 在这个例子中我们并不真正需要关注它 */
	while (printf("Enter lines of text, ^D to quit: "),
		fgets(buf.mtext, MAXTEXT + 1, stdin) != NULL) {
		size_t n = strlen(buf.mtext);		
		buf.mtext[n - 1] = '\0';
		if (msgsnd(msqid, &buf, sizeof(struct msgbuf), 0) == -1) {
			err_sys("msgsnd error");
		}	
	}

	if (msgctl(msqid, IPC_RMID, NULL) == -1) {
		err_sys("msgctl error");
	}

	if (ferror(stdin)) {
		err_sys("fgets error");
	}
	if (feof(stdin)) {
		printf("send %ld EOF\n", (long)getpid());
	}

	exit(EXIT_SUCCESS);
}

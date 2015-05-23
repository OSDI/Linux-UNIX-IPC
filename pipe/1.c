/**
 *	@演示管道用于父子进程之间的通信
 *	@Using a pipe to communicate between a parent and child process
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifdef _DEBUG
#include <sys/types.h>
#include <sys/stat.h>
#endif

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
	int	fd[2];	/* pipe file descriptors 
				 * fd[0] -> read end 
				 * fd[1] -> write end
				 */	
	pid_t pid;
	char buf;
	ssize_t	rsize;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s + strings\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* create the pipe */
	if (pipe(fd) == -1) {
		err_sys("pipe error: ");
	}
#ifdef _DEBUG
	struct stat st;
	int i;
	for (i = 0; i < 2; i++) {
		if (fstat(fd[i], &st) == -1) {
			err_sys("fstat error: ");
		}	
		if (S_ISFIFO(st.st_mode)) {
			printf("fd[%d]: FIFO/pipe\n", i);	
		}
	}
#endif
	
	switch (pid = fork()) {
		case -1:	/* error */
			err_sys("fork error: ");

		case 0:		/* child -- read frome pipe */
			if (close(fd[1]) == -1) {	/* close the unused write end fd[1]	*/
				err_sys("child close error: ");
			}
			/* read data from pipe, and echo on stdout */
			for ( ;; ) {
				/* just read a char */
				rsize = read(fd[0], &buf, sizeof(char));
				if (-1 == rsize) {			/* read error */
					err_sys("read error: ");
				}
				if (0 == rsize) break;		/* reach EOF */
				write(STDOUT_FILENO, &buf, rsize);
			}

			write(STDOUT_FILENO, "\n", 1);
			if (close(fd[0]) == -1) {
				err_sys("child close error: ");
			}
			_exit(EXIT_SUCCESS);

		default:	/* parent -- write to pipe */
			if (close(fd[0]) == -1) {	/* close the unused read end fd[0]	*/
				err_sys("parent close error: ");
			}
			/* write argv[1] to pipe */
			if (write(fd[1], argv[1], strlen(argv[1])) != strlen(argv[1])) {
				err_sys("parent write error: ");	
			}
			/* close write end, then reader(child process) will see EOF */
			if (close(fd[1]) == -1) {
				err_sys("parent close error: ");
			}
			/* wait child process to die */
			wait(NULL);
			break;
	}

	exit(EXIT_SUCCESS);
}

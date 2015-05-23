#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

/**
 *	parent						child
 *		-> 	+---------------+ ->
 *	fd1[1]	|				|	fd1[0]
 *			+---------------+
 *
 *  	<-	+---------------+ <-
 *  fd2[0]	|				|	fd2[1]
 *			+---------------+
 */

#define MAXLINE	(4096)

static void err_sys(const char *err_msg, ...)
{
	va_list ap;

	va_start(ap, err_msg);
	vfprintf(stderr, err_msg, ap);
	va_end(ap);
	fflush(NULL);

	exit(EXIT_FAILURE);
}

static void sig_pipe(int signo)
{
	printf("parent: SIGPIPE caught\n");
	exit(-1);
}

int main(int argc, const char *argv[])
{
	int fd1[2], fd2[2];
	pid_t pid;
	char line[MAXLINE];
	size_t nw;
	ssize_t nr;

	if (signal(SIGPIPE, sig_pipe) == SIG_ERR) {
		err_sys("signal error: ");
	}

	if (pipe(fd1) == -1 || pipe(fd2) == -1) {
		err_sys("pipe error: ");
	}

	if ((pid = fork()) == -1) {	/* error */
		err_sys("fork error: ");
	} else if (pid > 0) {	/* parent */
		close(fd1[0]);
		close(fd2[1]);

#if 1
	while (fgets(line, MAXLINE, stdin) != NULL) {
		if (line[0] == '\n')	continue;
		nw = strlen(line);		
		if (write(fd1[1], line, nw) != nw) {
			err_sys("write error: ");
		}
#if 0
//		close(fd1[1]);
//		close(fd2[0]);
		exit(0);
#endif

#if 0
		while ((nr = read(fd2[0], line, MAXLINE)) > 0) {
			line[nr] = '\0';
			if (fputs(line, stdout) == EOF) {
				err_sys("fputs error: ");
			}
		}
		if (nr < 0) {
			err_sys("read error: ");
		}
		if (nr == 0) {
			fprintf(stderr, "child closed pipe\n");
			break;
		}
#endif
#if 1
		if ((nr = read(fd2[0], line, MAXLINE)) < 0) {
			err_sys("read error: ");
		}
		if (nr == 0) {
			fprintf(stderr, "child closed pipe\n");
			break;
		}
		line[nr] = '\0';
		if (fputs(line, stdout) == EOF) {
			err_sys("fputs error: ");
		}
#endif
	}
#else
		FILE *parent_read_end = fdopen(fd2[0], "r");
		if (!parent_read_end) {
			err_sys("fdopen parent read end error: ");
		}
		setvbuf(parent_read_end, NULL, _IOLBF, 0);
		FILE *parent_write_end = fdopen(fd1[1], "w");
		if (!parent_write_end) {
			err_sys("fdopen parent write end error: ");	
		}
		setvbuf(parent_write_end, NULL, _IOLBF, 0);
		while (fgets(line, MAXLINE, stdin) != NULL) {
			if (line[0] == '\n')	continue;
			if (fputs(line, parent_write_end) == EOF) {
				err_sys("fputs error: ");
			}		
			if (fgets(line, MAXLINE, parent_read_end) != NULL) {
				if (fputs(line, stdout) == EOF) {
					err_sys("fputs error: ");
				}
			} else {
				if (feof(parent_read_end)) {
					fprintf(stderr, "child closed pipe\n");
					break;
				}
				if (ferror(parent_read_end)) {
					err_sys("fgets error: ");
				}
			}
		}
#endif
		if (ferror(stdin)) {
			err_sys("fgets error: ");
		}
		close(fd1[1]);
		close(fd2[0]);
		exit(0);
	} else {	/* child */
		close(fd1[1]);	
		close(fd2[0]);	
		/* stdin -> fd1[0]	*/
		if (fd1[0] != STDIN_FILENO) {
			if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO) {
				err_sys("dup2 error to stdin: ");
			}
			close(fd1[0]);
		}	
		/* stdout -> fd2[0]	*/
		if (fd2[1] != STDOUT_FILENO) {	
			if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO) {
				err_sys("dup2 error to stdout: ");
			}
			close(fd2[1]);
		}

		execl("./add2", "add2", (char *)NULL);
		close(fd1[0]);
		close(fd2[1]);
		_exit(EXIT_FAILURE);
	}
}

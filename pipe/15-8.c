#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define MAXLINE	(4096)

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

static void sig_pipe(int signo)
{
	fprintf(stderr, "child: SIGPIPE caught\n");
	exit(-1);
}

int main(int argc, const char *argv[])
{
	int int1, int2;	
	char line[MAXLINE];
	ssize_t n;

	if (signal(SIGPIPE, sig_pipe) == SIG_ERR) {
		err_sys("signal error: ");
	}

	while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0) {
		if (line[0] == '\n')	continue;
		line[n]	= '\0';
		if (sscanf(line, "%d %d", &int1, &int2) == 2) {
			sprintf(line, "%d\n", int1 + int2);
			if (write(STDOUT_FILENO, line, strlen(line)) != strlen(line)) {
				err_sys("write error: ");
			}
		} else {
			if (write(STDOUT_FILENO, "invalid args\n", 13) != 13) {
				err_sys("write error: ");
			}
		}
	}
	if (n == 0) {
		fprintf(stderr, "parent close the pipe\n");
		exit(-1);
	}

	exit(EXIT_SUCCESS);
}

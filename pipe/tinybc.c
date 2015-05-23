#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE	(1024)

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

static void sig_pipe(int signo)
{
	fprintf(stderr, "parent: SIGPIPE caught\n");
	exit(-1);
}

int main(int argc, const char *argv[])
{
	int todc[2], fromdc[2];	
	pid_t pid;

	if (signal(SIGPIPE, sig_pipe) == SIG_ERR) {
		err_sys("signal error");
	}

	if (pipe(todc) == -1 || pipe(fromdc) == -1) {
		err_sys("pipe error");
	}

	if ((pid = fork()) == -1) {

	} else if (pid == 0) {	/* child */
		close(todc[1]);
		if (todc[0] != STDIN_FILENO) {
			if (STDIN_FILENO != dup2(todc[0], STDIN_FILENO)) {
				err_sys("dup2 to stdin error");
			}
			close(todc[0]);
		}

		close(fromdc[0]);
		if (fromdc[1] != STDOUT_FILENO) {
			if (STDOUT_FILENO != dup2(fromdc[1], STDOUT_FILENO)) {
				err_sys("dup2 to stdout error");
			}
			close(fromdc[1]);
		}

		if (execl("/usr/bin/dc", "dc", (char *)NULL) < 0) {
			err_sys("execl error");
		}
	} else {	/* parent */
		FILE *fpin, *fpout;
		int num1, num2;
		char op[MAXLINE] = { '\0' };
		char line[MAXLINE] = { '\0' };

		close(todc[0]);
		close(fromdc[1]);

		fpin = fdopen(fromdc[0], "r"); 
		fpout = fdopen(todc[1], "w");
		if (!fpin || !fpout) {
			err_sys("fdopen error");
		}
		setvbuf(fpin, NULL, _IOLBF, 0);
		setvbuf(fpout, NULL, _IOLBF, 0);

		while (printf("tinybc: "), fgets(line, MAXLINE, stdin) != NULL) {
			if (line[0] == '\n')	continue;
			if (sscanf(line, "%d %[+-*/^] %d", &num1, op, &num2) != 3) {
				fprintf(stderr, "syntax error\n");
				continue;
			}

			fprintf(fpout, "%d %d %c p\n", num1, num2, *op);	/* must contail '\n' */

			if (fgets(line, MAXLINE, fpin) == NULL) {
				if (ferror(fpin)) {
					err_sys("fgets error");
				}
				if (feof(fpin)) {
					fprintf(stderr, "child close pipe\n");
					break;
				}
			}
			/* line中包含有'\n' */
			fprintf(stdout, "%d %c %d = %s", num1, *op, num2, line);	
		}
		if (ferror(stdin)) {
			err_sys("fgets stdin error");
		}
		if (feof(stdin)) {
			printf("EOF\n");
		}

		fclose(fpout);	/* child will see EOF */
		fclose(fpin);
		waitpid(pid, NULL, 0);
		exit(0);
	}
}

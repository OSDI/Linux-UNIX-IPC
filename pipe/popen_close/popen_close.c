#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *popen_m(const char *command, const char *type)
{
	int fd[2];
	pid_t pid;
	FILE *ret;

	if (pipe(fd) == -1) {
		return NULL;
	}

	if ((pid = fork()) < 0) {	/* error, return NULL */
		return NULL;
	} else if (pid == 0) {	/* child */
		if (type[0] == 'r') {
			close(fd[0]);
			if (fd[1] != STDOUT_FILENO) {	/* stdout -> fd[1] */
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}
		} else if (type[0] == 'w') {
			close(fd[1]);
			if (fd[0] != STDIN_FILENO) {	/* stdin -> fd[0] */
				dup2(fd[0], STDIN_FILENO);
				close(fd[0]);
			}
		} 
		/* excute command */
		execl("/bin/sh", "sh", "-c", command, (char *)NULL);
		_exit(127);
	} else {	/* parent */
		if (type[0] == 'r') {
			close(fd[1]);
			ret = fdopen(fd[0], "r");
		} else if (type[0] == 'w') {
			close(fd[0]);
			ret = fdopen(fd[1], "w");
		}
		setvbuf(ret, NULL, _IOFBF, 0);
		return ret;		
	}
}
// popen("/bin/more", "w");

int pclose_m(FILE *stream)
{

}


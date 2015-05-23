/**
 *	@Copy file to pager program (/bin/more)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h> 	/* basename */
#include <sys/types.h>
#include <sys/wait.h>

#define DEF_PAGER	"/bin/more"
#define MAXLINE		(4096)

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
	int fd[2];	
	pid_t pid;
	size_t n;
	char *pager, *argv0;
	char line[MAXLINE];
	FILE *fp;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s + <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* open file */
	if (!(fp = fopen(argv[1], "r"))) {
		err_sys("fopen error: ");
	}

	/* create pipe */
	if (pipe(fd) == -1) {
		err_sys("pipe error: ");
	}

	if ((pid = fork()) == -1) {
		err_sys("fork error: ");
	} else if (pid > 0) {	/* parent */
		close(fd[0]);	/* close unused read end fd[0] */
		while (fgets(line, sizeof(line), fp) != NULL) {
			n = strlen(line);
			if (write(fd[1], line, n) != n) {
				err_sys("write error: ");
			}
		}
		if (ferror(fp)) {
			err_sys("fgets error");
		}
		close(fd[1]);
		if (waitpid(pid, NULL, 0) < 0) {
			err_sys("waitpid error: ");	
		}
	} else {	/* child */
		close(fd[1]);	/* close unused write end fd[1] */
		/* stdin -> read end fd[0] of pipe */
		if (fd[0] != STDIN_FILENO) {
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
		}
		if (!(pager = getenv("PAGER"))) {
			pager = DEF_PAGER; 	
		}
		argv0 = basename(pager);
		execl(pager, argv0, (char *)NULL);
		_exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}


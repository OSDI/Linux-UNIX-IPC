#include <stdio.h>
#include <stdlib.h>

#define MAXLINE	(4096)

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}


int main(int argc, const char *argv[])
{
	char line[MAXLINE];	
	FILE *fpin;

	if (!(fpin = popen("./myuclc", "r"))) {
		err_sys("popen error: ");	
	}

	for ( ;; ) {
		fputs("prompt> ", stdout);
		fflush(stdout);
		if (fgets(line, MAXLINE, fpin) == NULL)
			break;
		if (line[0] == '\n')	continue;
		if (fputs(line, stdout) == EOF) {
			err_sys("fputs error to pipe ");	
		}
	}
	if (pclose(fpin) == -1) {
		err_sys("pclose error: ");
	}
#if 1
	putchar('\n');
#endif
	exit(EXIT_SUCCESS);
}

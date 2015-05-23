#include <stdio.h>
#include <stdlib.h>

#define MAXLINE	(4096)
#define PAGER	"${PAGER:-more}"

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
	FILE *fpin, *fpout;
	char line[MAXLINE];
	extern FILE *popen_m(const char *command, const char *type);
	extern int pclose_m(FILE *stream);
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s + <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* open file */
	if (!(fpin = fopen(argv[1], "r"))) {
		err_sys("fopen error: ");
	}
	
	if (!(fpout = popen_m(PAGER, "w"))) {
		err_sys("popen error: ");
	}

	while (fgets(line, MAXLINE, fpin) != NULL) {
		if (fputs(line, fpout) == EOF) {
			err_sys("fputs error: ");
		}
	}
	if (ferror(fpin)) {
		err_sys("fgets error: ");
	}
	if (pclose(fpout) == -1) {
		err_sys("pclose error: ");
	}

	exit(EXIT_SUCCESS);
}

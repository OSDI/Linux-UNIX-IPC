#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define MAXLINE	(4096)
//#define MAXLINE	(8)

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

static void pr_stdio(const char *name, FILE *fp)
{
	fprintf(stderr, "stream = %s, ", name);
	if (fp->_IO_file_flags & _IO_UNBUFFERED)
		fprintf(stderr, "unbuffered");
	else if (fp->_IO_file_flags & _IO_LINE_BUF)
		fprintf(stderr, "line buffered");
	else 
		fprintf(stderr, "fully buffered");
	fprintf(stderr, ", buffer size = %d\n", fp->_IO_buf_end - fp->_IO_buf_base);	
}

int main(int argc, const char *argv[])
{
	int int1, int2;
	char line[MAXLINE];

	if (signal(SIGPIPE, sig_pipe) == SIG_ERR) {
		err_sys("signal error: ");
	}
#if 0
	if (setvbuf(stdin, NULL, _IOLBF, 0)) {
		err_sys("setvbuf stdin error: ");
	}
	if (setvbuf(stdout, NULL, _IOLBF, 0)) {
		err_sys("setvbuf stdout error: ");
	}
#endif


	while (fgets(line, MAXLINE + 1, stdin) != NULL) {
//		fprintf(stderr, "line: %s", stdin->_IO_read_ptr);
//		char *p = stdin->_IO_read_ptr;
#if 0
		char *p = line;
		while (*p) {
			fprintf(stderr, "%c(0x%02x) ", *p, *p);
			++p;
		}
		fprintf(stderr, "%c(0x%02x) ", *p, *p);
#endif
		
//		if (line[0] == '\n')	continue;
		if (sscanf(line, "%d %d", &int1, &int2) == 2) {
			printf("%d\n", int1 + int2);	/* full buffered when stdout is pipe */
			fflush(stdout);
		} else {
			printf("invalid args\n");
			fflush(stdout);
		}
	}
	if (ferror(stdin)) {
		err_sys("fgets error");
		exit(-1);
	}
	if (feof(stdin)) {
		fprintf(stderr, "parent close the pipe\n");
		exit(-1);
	}

	exit(EXIT_SUCCESS);
}

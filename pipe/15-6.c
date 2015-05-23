#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
	int c;
	
	while ((c = getchar()) != EOF) {
		if (isupper(c))
			c = tolower(c);
		if (putchar(c) == EOF)
			err_sys("putchar error: ");
		if (c == '\n')	/* pipe is full buffered */
			fflush(stdout);
	}	

	exit(EXIT_SUCCESS);
}

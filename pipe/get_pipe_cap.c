#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

static void err_sys(const char *err_msg)
{
	perror(err_msg);
	exit(EXIT_FAILURE);
}

int main(int argc, const char *argv[])
{
	int fds[2];	
	int cnt = 0;

	if (pipe(fds) < 0) 
		err_sys("pipe error");
	
	int flags = fcntl(fds[1], F_GETFL);
	if (fcntl(fds[1], F_SETFL, flags | O_NONBLOCK) < 0)
		err_sys("fcntl error");	

	while (1) {
		int ret = write(fds[1], "A", sizeof(char));	
		if (ret == -1) {
			fprintf(stderr, "errno: %s\n", strerror(errno));
			break;
		}
		++cnt;
	}
	printf("cap: %d\n", cnt);
	exit(EXIT_SUCCESS);
}

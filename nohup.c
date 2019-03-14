#define _XOPEN_SOURCE 700
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NOHUP_UTILITY_NOT_FOUND		127
#define NOHUP_UTILITY_NOT_INVOKED	126

int main(int argc, char *argv[])
{
	int c;

	setlocale(LC_ALL, "");

	while ((c = getopt(argc, argv, "")) != -1) {
		switch (c) {
		default:
			return 1;
		}
	}

	argv += optind;

	if (argv[0] == NULL) {
		fprintf(stderr, "nohup: utility required\n");
		return NOHUP_UTILITY_NOT_INVOKED;
	}

	signal(SIGHUP, SIG_IGN);

	if (isatty(STDOUT_FILENO)) {
		char out[FILENAME_MAX] = "nohup.out";
		int fd = open(out, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
		if (fd == -1) {
			char *home = getenv("HOME");
			if (home == NULL) {
				fprintf(stderr, "nohup: $HOME unset\n");
				return NOHUP_UTILITY_NOT_INVOKED;
			}
			snprintf(out, sizeof(out), "%s/nohup.out", home);
			fd = open(out, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
			if (fd == -1) {
				perror("nohup: couldn't open nohup.out or $HOME/nohup.out");
				return NOHUP_UTILITY_NOT_INVOKED;
			}
		}

		fprintf(stderr, "nohup: appending to %s\n", out);
		dup2(fd, STDOUT_FILENO);
	}

	if (isatty(STDERR_FILENO)) {
		dup2(STDOUT_FILENO, STDERR_FILENO);
	}

	execvp(argv[0], argv);

	perror("nohup");

	if (errno == ENOENT) {
		return NOHUP_UTILITY_NOT_FOUND;
	}

	return NOHUP_UTILITY_NOT_INVOKED;
}

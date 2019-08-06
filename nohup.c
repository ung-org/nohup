/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2019, Jakob Kaivo <jkk@ung.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _XOPEN_SOURCE 500
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define UTILITY_NOT_INVOKED	(126)
#define UTILITY_NOT_FOUND	(127)

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	while (getopt(argc, argv, "") != -1) {
		return 1;
	}

	if (optind >= argc) {
		fprintf(stderr, "nohup: missing operand\n");
		return UTILITY_NOT_INVOKED;
	}

	signal(SIGHUP, SIG_IGN);

	if (isatty(STDOUT_FILENO)) {
		char out[FILENAME_MAX] = "nohup.out";
		int fd = open(out, O_WRONLY | O_CREAT | O_APPEND,
			S_IRUSR | S_IWUSR);

		if (fd == -1) {
			char *home = getenv("HOME");
			if (home == NULL) {
				fprintf(stderr, "nohup: $HOME unset\n");
				return UTILITY_NOT_INVOKED;
			}

			snprintf(out, sizeof(out), "%s/nohup.out", home);
			fd = open(out, O_WRONLY | O_CREAT | O_APPEND,
				S_IRUSR | S_IWUSR);

			if (fd == -1) {
				perror("nohup: nohup.out or $HOME/nohup.out");
				return UTILITY_NOT_INVOKED;
			}
		}

		fprintf(stderr, "nohup: appending to %s\n", out);
		dup2(fd, STDOUT_FILENO);
	}

	if (isatty(STDERR_FILENO)) {
		dup2(STDOUT_FILENO, STDERR_FILENO);
	}

	execvp(argv[optind], argv + optind);

	fprintf(stderr, "nohup: %s: %s\n", argv[optind], strerror(errno));
	return errno == ENOENT ? UTILITY_NOT_FOUND : UTILITY_NOT_INVOKED;
}

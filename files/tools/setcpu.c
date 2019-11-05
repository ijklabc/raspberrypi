#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>

void show_usage(char *name)
{
	fprintf(stderr, "Usage: \t%s pid cpu0 cpux ...\n", name);
	fprintf(stderr, "Example: \n\t%s %u 2 3\n", name, getpid());
	exit(0);
}

int main(int argc, char **argv)
{
	int cpu;
	pid_t pid;
	cpu_set_t mask;

	if (argc < 3)
		show_usage(argv[0]);

	pid = atol(argv[1]);
	CPU_ZERO(&mask);
	for (int i = 2; i < argc; i++) {
		cpu = atoi(argv[i]);
		CPU_SET(cpu, &mask);
	}
	if (sched_setaffinity(pid, sizeof(cpu_set_t), &mask)) {
		fprintf(stderr, "Failed: %s\n", strerror(errno));
		return 1;
	}
	return 0;
}

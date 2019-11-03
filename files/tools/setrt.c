#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>

void show_usage(char *name)
{
	fprintf(stderr, "Usage: \t%s pid priority cpu0 cpux ...\n", name);
	fprintf(stderr, "Example: \n\t%s 3456 70 2 3\n", name);
	exit(0);
}

int set_process_attr(pid_t pid, int priority, cpu_set_t *mask)
{
	struct sched_param param;
	
	if (sched_setaffinity(pid, sizeof(cpu_set_t), mask)) {
		fprintf(stderr, "set process affinity failed: %s\n", strerror(errno));
		return errno;
	}

	param.sched_priority = priority;
	if (sched_setscheduler(pid, SCHED_FIFO, &param)) {
		fprintf(stderr, "set fifo and priority failed: %s\n", strerror(errno));
		return errno;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int cpu, i, priority;
	pid_t pid;
	cpu_set_t mask;

	if (argc < 4)
		show_usage(argv[0]);

	pid = atoi(argv[1]);
	priority = atoi(argv[2]);
	CPU_ZERO(&mask);
	for (i = 3; i < argc; i++) {
		cpu = atoi(argv[i]);
		CPU_SET(cpu, &mask);
	}
	if (set_process_attr(pid, priority, &mask))
		return errno;
	return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

static uint64_t rand_long() {
	return ((uint64_t) rand() << 32) | (uint64_t) rand();
}

int run() {
	uint64_t jaeger_trace_id = rand_long();
	uint64_t jaeger_parent_id = rand_long();

	printf("jaeger_trace_id: %llu, jaeger_parent_id: %llu\n", jaeger_trace_id, jaeger_parent_id);

	FILE* procfile = fopen("/proc/lttng_jaeger", "w");
	uint64_t* buf = malloc(sizeof(uint64_t) * 2);

	buf[0] = jaeger_trace_id;
	buf[1] = jaeger_parent_id;

	fwrite(buf, sizeof(uint64_t), 2, procfile);
	fclose(procfile);

	printf("Hello world pid=%lu tid=%lu\n", getpid(), syscall(SYS_gettid));
	FILE* testfile = fopen("/tmp/meow", "a");
	fprintf(testfile, "MEOW %ld from jaeger_trace_id %llu\n", time(NULL), jaeger_trace_id);
	fclose(testfile);

	return 0;
}

int main() {
	srand(time(NULL));
	run();
	run();
	run();
	run();
	return 0;
}


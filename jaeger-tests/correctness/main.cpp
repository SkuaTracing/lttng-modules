#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <thread>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifndef NO_JAEGER
#include <jaegertracing/Tracer.h>
#include <opentracing/tracer.h>

using namespace opentracing;
#endif

#define NUM_THREAD 50

int work() {
	// syscalls w/o arguments
	pid_t pid = getpid();
	pid_t ppid = getppid();
	pid_t tid = syscall(SYS_gettid);

	// gettimeofday
	struct timeval tv;
	gettimeofday(&tv, NULL);

	// print
	printf("Hello world pid=%lu ppid=%lu tid=%lu time %ld.%06ld\n", pid, ppid, tid, tv.tv_sec, tv.tv_usec);

	// write to file
	FILE* testfile = fopen("/tmp/meow", "a");
	fprintf(testfile, "MEOW %ld\n", time(NULL));
	fclose(testfile);

	// sleep
	struct timespec t;
	t.tv_sec = 0;
	t.tv_nsec = 250000000L; // 0.25 secs
	nanosleep(&t, NULL);

	return 0;
}

int no_parent_span(std::shared_ptr<opentracing::v1::Tracer> tracer) {
    #ifndef NO_JAEGER
    std::unique_ptr<opentracing::v1::Span> parent_span =
        tracer->StartSpan("request");
    #endif

    for (int i = 0; i < 10; i++) {
	// create a child span
	auto child_span = tracer->StartSpan("work", {opentracing::ChildOf(&parent_span->context())});
	struct timespec t = {0, 100000000L};
	nanosleep(&t, NULL);
    	work();
	nanosleep(&t, NULL);
	
    	// end child span
	child_span->Finish();
    }

    #ifndef NO_JAEGER
    parent_span->Finish();
    #endif
}

int main() {
  #ifndef NO_JAEGER
  std::cout << "Starting jaeger tracer" << std::endl;
  auto constant = true;
  auto sampler =
      constant
          ? jaegertracing::samplers::Config(jaegertracing::kSamplerTypeConst, 1)
          : jaegertracing::samplers::Config(
                jaegertracing::kSamplerTypeProbabilistic, 0.001);

  auto config = jaegertracing::Config(
      false, sampler,
      jaegertracing::reporters::Config(
          jaegertracing::reporters::Config::kDefaultQueueSize,
          std::chrono::seconds(1), true));
  auto tracer = jaegertracing::Tracer::make("correctness-tester", config);
  #endif

  std::thread thrs[NUM_THREAD];
  for (int i = 0; i < NUM_THREAD; i++) {
	  thrs[i] = std::thread(no_parent_span, tracer);
  }
  std::cout << "All threads started" << std::endl;

  for (int i = 0; i < NUM_THREAD; i++) {
	  thrs[i].join();
  }
  std::cout << "All threads completed" << std::endl;

}


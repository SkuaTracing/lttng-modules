#include <cassert>
#include <chrono>
#include <iostream>
#include <jaegertracing/Tracer.h>
#include <opentracing/tracer.h>
#include <sstream>
#include <thread>
#include <uWS/uWS.h>
#include <unordered_map>

using namespace opentracing;
// using namespace opentracing::mocktracer;

using namespace uWS;

void
inject_jaeger(uint64_t jaeger_trace_id, uint64_t jaeger_parent_id)
{
  //printf("jaeger_trace_id: %llu, jaeger_parent_id: %llu\n",
  //       jaeger_trace_id,
  //       jaeger_parent_id);

  FILE* procfile = fopen("/proc/lttng_jaeger", "w");
  uint64_t* buf = (uint64_t*)malloc(sizeof(uint64_t) * 2);

  buf[0] = jaeger_trace_id;
  buf[1] = jaeger_parent_id;

  fwrite(buf, sizeof(uint64_t), 2, procfile);
  fclose(procfile);
}

int
main()
{
  auto config = jaegertracing::Config(
    false,
    jaegertracing::samplers::Config(jaegertracing::kSamplerTypeConst, 1),
    jaegertracing::reporters::Config(
      jaegertracing::reporters::Config::kDefaultQueueSize,
      std::chrono::seconds(1),
      true));
  auto tracer = jaegertracing::Tracer::make("uws-web-cpp", config);

  Hub h;
  std::string response = "Hello!";

  h.onHttpRequest([&](HttpResponse* res,
                      HttpRequest req,
                      char* data,
                      size_t length,
                      size_t remainingBytes) {
    // std::cout << length << std::endl;
    std::unique_ptr<opentracing::v1::Span> _parent_span =
      tracer->StartSpan("request");
    jaegertracing::Span* parent_span = (jaegertracing::Span*) _parent_span.get();
    jaegertracing::SpanContext ctx = parent_span->contextNoLock();
    uint64_t trace_id = ctx.traceID().low();
    uint64_t span_id = ctx.spanID();
    inject_jaeger(trace_id, span_id);

    // std::cout << trace_id << ", " << span_id << std::endl;
    // assert(parent_span);
    // parent_span->Finish();

//    parent_span->Log({ { "event", "simple log" }, { "abc", 123 } });

    res->end(response.data(), response.length());
    parent_span->Finish();
  });

  if (h.listen(3000)) {
    h.run();
  }
}

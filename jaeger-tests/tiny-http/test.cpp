#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <uWS/uWS.h>
#include <unordered_map>

#ifndef NO_JAEGER
#include <jaegertracing/Tracer.h>
#include <opentracing/tracer.h>

using namespace opentracing;
#endif

using namespace uWS;

int main() {
  #ifndef NO_JAEGER
  std::cout << "Starting jaeger tracer" << std::endl;
  auto constant = false;
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
  auto tracer = jaegertracing::Tracer::make("uws-web-cpp", config);
  #endif

  Hub h;
  std::string response = "";
  for (int i = 0; i < 1000; i++) {
    response.append("A");
  }

  h.onHttpRequest([&](HttpResponse *res, HttpRequest req, char *data,
                      size_t length, size_t remainingBytes) {
    #ifndef NO_JAEGER
    std::unique_ptr<opentracing::v1::Span> parent_span =
        tracer->StartSpan("request");
    #endif

    res->end(response.data(), response.length());

    #ifndef NO_JAEGER
    parent_span->Finish();
    #endif
  });

  if (h.listen(3000)) {
    h.run();
  }
}

#include "uWS/uWS.h"
using namespace uWS;

int main() {
    Hub h;
    std::string response = "Hello!";

    h.onHttpRequest([&](HttpResponse *res, HttpRequest req, char *data, size_t length,
                        size_t remainingBytes) {
        res->end(response.data(), response.length());
    });

    if (h.listen(3000)) {
        h.run();
    }
}


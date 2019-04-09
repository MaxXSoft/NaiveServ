#include <server/responder/impl/debug.h>

#include <sstream>

#include <util/version.h>
#include <server/responder/router.h>
#include <util/strop.h>

namespace {

static constexpr const char *kMethodString[] = {"Get", "Post", "Other"};

}

DebugResponder::DebugResponder(const ArgList &args) {
    // generate debug info
    std::ostringstream oss;
    // display debug info
    oss << "<h2>Debug Info</h2>" << std::endl;
    oss << "first argument: ";
    oss << (args.size() >= 1 ? args[0] : "<i>NULL</i>") << std::endl;
    // display server info
    oss << "<h2>Server Info</h2>" << std::endl;
    oss << "server name: " << APP_NAME << std::endl;
    oss << "server version: " << APP_VERSION << std::endl;
    // display responder info
    const auto &router = Router::Instance();
    oss << "<h2>Responder Info</h2>" << std::endl;
    router.PrintAllResponders(oss);
    // display router info
    oss << "<h2>Router Info</h2>" << std::endl;
    router.PrintRouterRules(oss, "<br>");
    // store all info
    debug_info_ = oss.str();
}

HTTPResponse DebugResponder::AcceptRequest(
        const HTTPParser &parser) const {
    // check if valid request
    auto url = NormalizeURL(parser.url());
    if (StrEndsWith(url, ".ico")) return HTTPResponse(404);
    // generate HTML
    std::ostringstream oss;
    oss << "<h1>NaiveServ Debug Information</h1><br>" << std::endl;
    // display HTTP info
    oss << "<h2>HTTP Info</h2>" << std::endl;
    oss << "HTTP method: ";
    oss << kMethodString[static_cast<int>(parser.method())] << std::endl;
    oss << "current URL: " << url << std::endl;
    oss << "UA: " << parser.GetFieldValue("User-Agent") << std::endl;
    // send response
    HTTPResponse response;
    auto data = oss.str() + debug_info_;
    auto ptr = reinterpret_cast<std::uint8_t *>(data.data());
    response.set_data(ptr, data.size());
    return response;
}

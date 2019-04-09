#include <server/responder/impl/debug.h>

#include <sstream>

#include <util/version.h>
#include <server/responder/router.h>
#include <util/strop.h>

namespace {

static constexpr const char *kMethodString[] = {"Get", "Post", "Other"};

}

std::string DebugResponder::GenerateHTML(const HTTPParser &parser) const {
    // generate HTML
    std::ostringstream oss;
    oss << "<h1>NaiveServ Debug Information</h1><br>" << std::endl;
    // display HTTP info
    oss << "<h2>HTTP Info</h2>" << std::endl;
    oss << "HTTP method: ";
    oss << kMethodString[static_cast<int>(parser.method())] << std::endl;
    oss << "<br>";
    oss << "current URL: " << parser.url() << std::endl;
    oss << "<br>";
    oss << "UA: " << parser.GetFieldValue("User-Agent") << std::endl;
    oss << "<br>";
    // display debug info
    oss << debug_info_;
    // display responder info
    const auto &router = Router::Instance();
    oss << "<h2>Responder Info</h2>" << std::endl;
    router.PrintAllResponders(oss);
    oss << "<br>";
    // display router info
    oss << "<h2>Router Info</h2>" << std::endl;
    router.PrintRouterRules(oss, "<br>");
    oss << "<br>";
    return oss.str();
}

DebugResponder::DebugResponder(const ArgList &args) {
    // generate debug info
    std::ostringstream oss;
    // display debug info
    oss << "<h2>Debug Info</h2>" << std::endl;
    oss << "first argument: ";
    oss << (args.size() >= 1 ? args[0] : "<i>NULL</i>") << std::endl;
    oss << "<br>";
    // display server info
    oss << "<h2>Server Info</h2>" << std::endl;
    oss << "server name: " << APP_NAME << "<br>" << std::endl;
    oss << "server version: " << APP_VERSION << "<br>" << std::endl;
    // store all info
    debug_info_ = oss.str();
}

HTTPResponse DebugResponder::AcceptRequest(
        const HTTPParser &parser) const {
    // check if valid request
    auto url = NormalizeURL(parser.url());
    if (StrEndsWith(url, ".ico")) return HTTPResponse(404);
    // send response
    HTTPResponse response;
    auto data = GenerateHTML(parser);
    auto ptr = reinterpret_cast<std::uint8_t *>(data.data());
    response.set_field("Content-Type", "text/html");
    response.set_data(ptr, data.size());
    return response;
}

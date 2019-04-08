#include <server/responder/impl/normal.h>

#include <map>
#include <fstream>
#include <vector>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include <config/config.h>

namespace {

static const std::map<std::string, std::string> kTypeMap = {
    {"html", "text/html"},
    {"htm", "text/html"},
    {"ico", "image/x-icon"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"gif", "image/gif"},
    {"png", "image/png"},
};

inline const char *GetTypeByExt(const std::string &ext) {
    auto it = kTypeMap.find(ext);
    if (it != kTypeMap.end()) {
        return it->second.c_str();
    }
    else {
        return "application/octet-stream";
    }
}

inline std::string GetFileExtension(const std::string &file) {
    auto dot_pos = file.rfind('.');
    if (dot_pos == std::string::npos) return "";
    return file.substr(dot_pos + 1);
}

inline std::size_t GetFileSize(std::ifstream &ifs) {
    auto pos = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    auto size = ifs.tellg();
    ifs.seekg(pos);
    return size;
}

} // namespace

HTTPResponse NormalResponder::AcceptRequest(
        const HTTPParser &parser) const {
    // check method
    if (parser.method() != HTTPParser::Method::Get) {
        // not implemented
        return HTTPResponse(501);
    }
    // get actual url
    assert(parser.url().find(url_prefix_) != std::string::npos);
    auto url = parser.url().substr(url_prefix_.size());
    // get file path
    auto path = ConfigReader::Instance().www_root();
    path += url;
    if (path.back() == '/') path += index_page_;
    // open file
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) {
        // file not found
        return HTTPResponse(404);
    }
    // get content type
    auto type = GetTypeByExt(GetFileExtension(path));
    // read file to buffer
    std::vector<std::uint8_t> buffer;
    buffer.resize(GetFileSize(ifs));
    ifs.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
    // initialize response
    HTTPResponse response(200);
    response.set_data(buffer);
    response.set_field("Content-Type", type);
    return response;
}

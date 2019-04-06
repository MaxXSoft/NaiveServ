#ifndef NAIVESERV_HTTP_PARSER_H_
#define NAIVESERV_HTTP_PARSER_H_

#include <string>
#include <map>
#include <vector>
#include <cstdint>

// parse HTTP request
class HTTPParser {
public:
    using DataArray = std::vector<std::uint8_t>;

    enum class Method {
        Get, Post, Other
    };

    HTTPParser() {}
    HTTPParser(const std::string &request) { Parse(request); }

    // parse specific request
    void Parse(const std::string &request);

    // get the value of specific field name in current HTTP request.
    // if field name not found, returns empty string
    const std::string &GetFieldValue(const std::string &field_name) {
        return field_info_[field_name];
    }

    // getters
    Method method() const { return method_; }
    const std::string &url() const { return url_; }
    int major_version() const { return major_version_; }
    int minor_version() const { return minor_version_; }
    const DataArray &data() const { return data_; }

private:
    // HTTP request method.
    // GET, POST... and so on
    Method method_;
    // requested URL
    std::string url_;
    // HTTP protocol version (e.g. 1.1)
    int major_version_, minor_version_;
    // storing field information.
    // e.g. field_info_["Host"] = "www.baidu.com";
    std::map<std::string, std::string> field_info_;
    // requested data.
    // this field is valid when client transfer data using POST method
    DataArray data_;
};

#endif // NAIVESERV_HTTP_PARSER_H_

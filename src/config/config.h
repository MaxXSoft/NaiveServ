#ifndef NAIVESERV_CONFIG_CONFIG_H_
#define NAIVESERV_CONFIG_CONFIG_H_

#include <string>
#include <map>
#include <cstdint>

#include <rapidjson/document.h>

#include <util/singleton.h>

class ConfigReader : public Singleton<ConfigReader> {
public:
    using ResponderRules = std::map<std::string, std::string>;

    const char *GetRule(const std::string &url) const;

    // getters
    std::uint16_t port() const { return port_; }
    const std::string &www_root() const { return www_root_; }
    std::uint32_t sock_buffer_size() const { return sock_buf_size_; }
    const std::string &default_responder() const { return default_resp_; }
    const ResponderRules &responder_rules() const { return resp_rules_; }

    bool is_error() const { return is_error_; }

private:
    friend class Singleton<ConfigReader>;

    ConfigReader();

    bool LogError(const char *message);
    void SetAsDefault();
    bool CheckHasMember(const char *name);
    bool ReadConfig();

    std::string current_path_;
    bool is_error_;
    // DOM of JSON
    rapidjson::Document document;
    // configurations
    std::uint16_t port_;
    std::string www_root_;
    std::uint32_t sock_buf_size_;
    std::string default_resp_;
    ResponderRules resp_rules_;
};

#endif // NAIVESERV_CONFIG_CONFIG_H_

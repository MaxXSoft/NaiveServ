#ifndef NAIVESERV_CONFIG_CONFIG_H_
#define NAIVESERV_CONFIG_CONFIG_H_

#include <string>
#include <cstdint>

#include <rapidjson/document.h>

#include <util/singleton.h>
#include <server/responder/responder.h>

class ConfigReader : public Singleton<ConfigReader> {
public:
    bool ReloadConfig(const std::string &file);

    // getters
    std::uint16_t port() const { return port_; }
    const std::string &www_root() const { return www_root_; }
    std::uint32_t sock_buffer_size() const { return sock_buf_size_; }
    const ResponderRule &default_rule() const { return default_rule_; }
    const ResponderRules &responder_rules() const { return resp_rules_; }

    bool is_error() const { return is_error_; }

private:
    friend class Singleton<ConfigReader>;

    ConfigReader();

    bool LogError(const char *message);
    void SetAsDefault();
    bool CheckHasMember(const char *name);
    bool ReadRule(const rapidjson::Value &v,
            std::string &name, ArgList &args);
    bool ReadRule(const rapidjson::Value &v,
            std::string &url, std::string &name, ArgList &args);
    bool ReadConfig();

    std::string current_path_;
    bool is_error_;
    // DOM of JSON
    rapidjson::Document document;
    // configurations
    std::uint16_t port_;
    std::string www_root_;
    std::uint32_t sock_buf_size_;
    ResponderRule default_rule_;
    ResponderRules resp_rules_;
};

#endif // NAIVESERV_CONFIG_CONFIG_H_

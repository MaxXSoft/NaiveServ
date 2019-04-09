#include <config/config.h>

#include <fstream>
#include <sstream>

#include <rapidjson/istreamwrapper.h>

#include <util/os.h>
#include <util/version.h>
#include <util/logger.h>

#ifdef UTIL_OS_WINDOWS
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

namespace {

static constexpr const char *kDefaultConfigFile = "config.json";
static constexpr const char *kCurrentPathPlaceholder = "${currentPath}";
static constexpr int kPlaceHolderLen = 14;

// TODO: this function returns current working directory,
// but not the program directory. consider rewrite
std::string GetCurrentDir() {
    char buffer[512];
    getcwd(buffer, sizeof(buffer));
    return buffer;
}

// check if is valid version
bool IsValidVersion(const char *version) {
    const int cur_ver[] = {APP_VERSION_MAJOR,
            APP_VERSION_MINOR, APP_VERSION_PATCH};
    int version_num;
    char dot;
    std::istringstream iss(version);
    for (int i = 0; i < 3; ++i) {
        if (i) {
            // check dot
            iss >> dot;
            if (dot != '.') return false;
        }
        // check version number
        iss >> version_num;
        if (version_num > cur_ver[i]) return false;
    }
    return true;
}

} // namespace

ConfigReader::ConfigReader() {
    // initialize current path
    current_path_ = GetCurrentDir();
    // try to read configuration
    ReloadConfig(current_path_ + "/" + kDefaultConfigFile);
}

bool ConfigReader::LogError(const char *message) {
    ulog << "[ConfigReader " << utime << "] ERROR: ";
    ulog << message << std::endl;
    return !(is_error_ = true);
}

void ConfigReader::SetAsDefault() {
    port_ = 80;
    www_root_ = current_path_ + "/www";
    sock_buf_size_ = 1024;
    default_rule_ = {"Normal", {}};
    resp_rules_.clear();
}

bool ConfigReader::CheckHasMember(const char *name) {
    if (!document.HasMember(name)) {
        using namespace std::string_literals;
        return LogError(("missing '"s + name + "'").c_str());
    }
    return true;
}

// TODO: rewrite this function
bool ConfigReader::ReadRule(const rapidjson::Value &v,
        std::string &name, ArgList &args) {
    // existence check
    if (!v.IsObject() || !v.HasMember("name") || !v.HasMember("args")) {
        LogError("invalid responder rule");
    }
    // check if type is correct
    const auto &n = v["name"], &a = v["args"];
    if (!n.IsString() || !a.IsArray()) {
        return LogError("invalid content of rule");
    }
    name = n.GetString();
    // get argument array
    for (const auto &arg : a.GetArray()) {
        if (!arg.IsString()) return LogError("invalid argument list");
        args.push_back(arg.GetString());
    }
    return true;
}

// TODO: rewrite this function
bool ConfigReader::ReadRule(const rapidjson::Value &v,
        std::string &url, std::string &name, ArgList &args) {
    if (!ReadRule(v, name, args)) return false;
    // existence check
    if (!v.HasMember("url")) LogError("invalid responder rule");
    // check if type is correct
    const auto &u = v["url"];
    if (!u.IsString()) return LogError("invalid content of rule");
    url = u.GetString();
    return true;
}

// TODO: rewrite this function
bool ConfigReader::ReadConfig() {
    // check version
    if (!CheckHasMember("version")) return false;
    const auto &ver = document["version"];
    if (!ver.IsString() || !IsValidVersion(ver.GetString())) {
        return LogError("configuration file version is too high");
    }
    // check port
    if (!CheckHasMember("port")) return false;
    const auto &port = document["port"];
    if (!port.IsUint() || port.GetUint() > 65535) {
        return LogError("invalid port");
    }
    port_ = port.GetUint();
    // check root directory
    if (!CheckHasMember("rootDirectory")) return false;
    const auto &root_dir = document["rootDirectory"];
    if (!root_dir.IsString()) return LogError("invalid root directory");
    www_root_ = root_dir.GetString();
    // replace placeholder of current path
    auto pos = www_root_.find(kCurrentPathPlaceholder);
    if (pos != std::string::npos) {
        www_root_.replace(pos, kPlaceHolderLen, current_path_);
    }
    // check socket buffer size
    if (!CheckHasMember("sockBufferSize")) return false;
    const auto &buf_size = document["sockBufferSize"];
    if (!buf_size.IsUint()) return LogError("invalid buffer size");
    sock_buf_size_ = buf_size.GetUint();
    // check responders
    if (!CheckHasMember("responder")) return false;
    const auto &resp = document["responder"];
    if (!resp.IsObject() || !resp.HasMember("default") ||
            !resp.HasMember("rules")) {
        return LogError("invalid responder object");
    }
    // check default responder
    if (!ReadRule(resp["default"], default_rule_.first,
            default_rule_.second)) return false;
    // check responder rules
    const auto &rules = resp["rules"];
    if (!rules.IsArray()) return LogError("invalid rule array");
    for (const auto &i : rules.GetArray()) {
        std::string url;
        ResponderRule rule;
        if (!ReadRule(i, url, rule.first, rule.second)) return false;
        resp_rules_[url] = rule;
    }
    return true;
}

bool ConfigReader::ReloadConfig(const std::string &file) {
    // try to open configuration file
    std::ifstream ifs(file);
    if (!ifs.is_open()) {
        SetAsDefault();
        return LogError("invalid configuration file path");
    }
    // parse JSON to DOM
    rapidjson::IStreamWrapper isw(ifs);
    document.ParseStream(isw);
    if (document.HasParseError() || !document.IsObject()) {
        SetAsDefault();
        return LogError("invalid configuration file");
    }
    // check config file
    auto ret = ReadConfig();
    if (!ret) SetAsDefault();
    return ret;
}

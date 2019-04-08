#include <config/config.h>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>

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
    // try to open configuration file
    std::ifstream ifs(current_path_ + "/config.json");
    if (!ifs.is_open()) {
        LogError("invalid configuration file path");
        SetAsDefault();
        return;
    }
    // parse JSON to DOM
    rapidjson::IStreamWrapper isw(ifs);
    document.ParseStream(isw);
    if (document.HasParseError()) {
        LogError("invalid configuration file");
        SetAsDefault();
        return;
    }
    // set all configurations to default
    SetAsDefault();
    // check config file
    ReadConfig();
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
    default_resp_ = "Normal";
    resp_rules_.clear();
}

bool ConfigReader::CheckHasMember(const char *name) {
    if (!document.HasMember(name)) {
        std::string err_info = "missing '";
        err_info = err_info + name + "'";
        return LogError(err_info.c_str());
    }
    return true;
}

// TODO: rewrite this function
bool ConfigReader::ReadConfig() {
    // check version
    if (CheckHasMember("version")) return false;
    const auto &ver = document["version"];
    if (!ver.IsString() || !IsValidVersion(ver.GetString())) {
        return LogError("configuration file version is too high");
    }
    // check port
    if (CheckHasMember("port")) return false;
    const auto &port = document["port"];
    if (!port.IsUint() || port.GetUint() > 65535) {
        return LogError("invalid port");
    }
    port_ = port.GetUint();
    // check root directory
    if (CheckHasMember("rootDirectory")) return false;
    const auto &root_dir = document["rootDirectory"];
    if (!root_dir.IsString()) return LogError("invalid root directory");
    www_root_ = root_dir.GetString();
    // replace placeholder of current path
    auto pos = www_root_.find(kCurrentPathPlaceholder);
    if (pos != std::string::npos) {
        www_root_.replace(pos, current_path_.size(), current_path_);
    }
    // check socket buffer size
    if (CheckHasMember("sockBufferSize")) return false;
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
    const auto &def_resp = resp["default"];
    if (!def_resp.IsString()) return LogError("invalid responder name");
    default_resp_ = def_resp.GetString();
    // check responder rules
    const auto &rules = resp["rules"];
    if (!rules.IsArray()) return LogError("invalid rule array");
    for (const auto &i : rules.GetArray()) {
        if (!i.IsObject() || !i.HasMember("url") || !i.HasMember("name")) {
            LogError("invalid responder rule");
        }
        const auto &url = i["url"], &name = i["name"];
        if (!url.IsString() || !name.IsString()) {
            return LogError("'url' or 'name' must be string");
        }
        resp_rules_[url.GetString()] = name.GetString();
    }
}

const char *ConfigReader::GetRule(const std::string &url) const {
    int max_prefix_len = 0;
    const char *cur_resp = nullptr;
    // find in all of rules
    for (const auto &it : resp_rules_) {
        // check if is prefix
        const auto &cur = it.first;
        auto pref = std::mismatch(cur.begin(), cur.end(), url.begin());
        if (pref.first == cur.end()) {
            // check if length is longest
            if (cur.size() > max_prefix_len) {
                max_prefix_len = cur.size();
                cur_resp = it.second.c_str();
            }
        }
    }
    return cur_resp;
}

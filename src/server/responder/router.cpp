#include <server/responder/router.h>

#include <utility>
#include <algorithm>

#include <server/responder/table.h>
#include <server/responder/impl/normal.h>
#include <config/config.h>
#include <util/logger.h>

namespace {

static constexpr const char *kAllResponders[] = {
    RESPONDER_EXPAND(RESPONDER_NAME_ARRAY, ALL_RESPONDERS)
};

} // namespace

Router::Router() {
    auto &config = ConfigReader::Instance();
    // read default rule
    const auto &def_rule = config.default_rule();
    default_resp_ = NewResponder(def_rule.first, def_rule.second);
    if (!default_resp_) {
        LogError("invalid name of default responder, use 'Normal'");
        default_resp_ = std::make_shared<NormalResponder>(ArgList());
    }
    // initialize router table
    for (const auto &it : config.responder_rules()) {
        const auto &rule = it.second;
        auto resp = NewResponder(rule.first, rule.second);
        if (!resp) {
            using namespace std::string_literals;
            LogError(("invalid responder name '"s
                    + rule.first + "'").c_str());
        }
        else {
            table_[it.first] = std::move(resp);
        }
    }
}

void Router::LogError(const char *message) {
    ulog << "[Router " << utime << "] ERROR: ";
    ulog << message << std::endl;
}

void Router::PrintRule(std::ostream &os, const ResponderRule &rule) const {
    os << "{name: " << rule.first << ", args: [";
    int count = 0;
    for (const auto &i : rule.second) {
        if (count++) os << ", ";
        os << "(" << i << ")";
    }
    os << "]}";
}

Responder Router::GetResponder(const std::string &url) const {
    int max_prefix_len = 0;
    Responder cur_resp = nullptr;
    // find in all of rules
    for (const auto &it : table_) {
        // check if is prefix
        const auto &cur = it.first;
        auto pref = std::mismatch(cur.begin(), cur.end(), url.begin());
        if (pref.first == cur.end()) {
            // check if length is longest
            if (cur.size() > max_prefix_len) {
                max_prefix_len = cur.size();
                cur_resp = it.second;
            }
        }
    }
    return cur_resp ? cur_resp : default_resp_;
}

void Router::PrintAllResponders(std::ostream &os,
        const char *delimiter) const {
    int count = 0;
    for (const auto &i : kAllResponders) {
        if (count++) os << delimiter;
        os << i;
    }
}

void Router::PrintRouterRules(std::ostream &os,
        const char *delimiter) const {
    auto &config = ConfigReader::Instance();
    // print default responder
    const auto &def_rule = config.default_rule();
    os << "default: ";
    if (IsResponderValid(def_rule.first)) {
        PrintRule(os, def_rule);
    }
    else {
        os << "{name: Normal, args: []}";
    }
    // print rest of rules
    for (const auto &it : config.responder_rules()) {
        const auto &url = it.first;
        const auto &rule = it.second;
        if (IsResponderValid(rule.first)) {
            os << delimiter << url << ": ";
            PrintRule(os, rule);
        }
    }
}

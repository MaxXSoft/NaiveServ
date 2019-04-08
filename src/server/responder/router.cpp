#include <server/responder/router.h>

#include <algorithm>

#include <server/responder/table.h>
#include <config/config.h>

Router::Router() {
    auto &config = ConfigReader::Instance();
    // read default rule
    const auto &def_rule = config.default_rule();
    default_resp_ = NewResponder(def_rule.first, def_rule.second);
    // initialize router table
    for (const auto &it : config.responder_rules()) {
        const auto &rule = it.second;
        table_[it.first] = NewResponder(rule.first, rule.second);
    }
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

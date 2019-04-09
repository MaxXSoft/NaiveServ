#ifndef NAIVESERV_UTIL_STROP_H_
#define NAIVESERV_UTIL_STROP_H_

#include <string>
#include <functional>
#include <cstddef>

inline bool StrEndsWith(const std::string &l, const std::string &r) {
    if (l.length() < r.length()) return false;
    return !l.compare(l.length() - r.length(), r.length(), r);
}

inline std::string GetStrSuffix(const std::string &s, char find_c = '.') {
    auto pos = s.rfind(find_c);
    if (pos == std::string::npos) return "";
    return s.substr(pos + 1);
}

inline void StrSplit(const std::string &s, const std::string &delim,
        std::function<void(const std::string &)> callback) {
    std::size_t start = 0, end = s.find(delim);
    while (end != std::string::npos) {
        callback(s.substr(start, end - start));
        start = end + delim.length();
        end = s.find(delim, start);
    }
}

inline std::string NormalizeURL(const std::string &url) {
    std::string ans;
    // split and traversal
    StrSplit(url, "/", [&ans](const std::string &s) {
        // skip invalid path
        if (s.empty() || s == "." || s == "..") return;
        // push back to result
        ans += "/" + s;
    });
    // add slash in end of result
    auto ext = GetStrSuffix(ans);
    if (ext.find('/') != std::string::npos) ans += '/';
    return ans;
}

#endif // NAIVESERV_UTIL_STROP_H_

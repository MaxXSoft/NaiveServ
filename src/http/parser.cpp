#include <http/parser.h>

#include <cstring>

void HTTPParser::Parse(const std::string &request) {
    int now_pos = 0;
    // fetch method information
    switch (request[0]) {
        case 'G': method_ = Method::Get; break;
        case 'P': method_ = Method::Post; break;
        default: method_ = Method::Other;
    }
    // fetch requested URL
    int st = 0, ed;
    for (int i = 0; ; ++i) {
        if(request[i] == ' ') {
            if(st == 0) {
                st = i;
            }
            else {
                ed = i;
                break;
            }
        }
    }
    url_ = request.substr(st + 1, ed - st - 1);
    // fetch HTTP version
    for (int i = 4; ; ++i) {
        if (request.substr(i - 4, 5) == "HTTP/") {
            int j = i + 1;
            std::string num = "\0";
            while (request[j] != '.') {
                num += request[j];
                j++;
            }
            int tmp = 1;
            major_version_ = 0;
            for (int k = num.size() - 1; k >= 0; --k) {
                major_version_ += tmp * (num[k] - '0');
                tmp *= 10;
            }
            num.clear();
            j++;
            while (request[j] >= '0' && request[j] <= '9') {
                num += request[j];
                j++;
            }
            tmp = 1;
            minor_version_ = 0;
            for (int k = num.size() - 1; k >= 0; --k) {
                minor_version_ += tmp * (num[k] - '0');
                tmp *= 10;
            }
            now_pos = j; // '\r' of the first line
            break;
        }
    }
    // fetch field information
    bool have_data = false;
    while (now_pos < request.size()) {
        // first position of this line
        now_pos += 2;
        int st = now_pos;
        bool is_find = false;
        std::string field_;
        std::string info_;
        while (request[now_pos] != '\r' && request[now_pos + 1] != '\n') {
            if (!is_find && request[now_pos] == ':') {
                field_ = request.substr(st, now_pos - st);
                is_find = true;
                now_pos++;
                st = now_pos + 1;
            }
            now_pos++;
        }
        // now_pos is '\r' of this line
        if (is_find) {
            info_ = request.substr(st, now_pos - st);
            field_info_[field_] = info_;
        }
        if (request[now_pos + 2] == '\r' && request[now_pos + 3] == '\n') {
            have_data = true;
            break;
        }
    }
    // fetch requested data
    if (have_data) {
        now_pos += 4;   // first position of the data
        data_.resize(request.size() - now_pos);
        std::memcpy(data_.data(), request.data() + now_pos, data_.size());
    }
}

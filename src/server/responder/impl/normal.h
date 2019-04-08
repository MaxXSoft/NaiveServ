#ifndef NAIVESERV_SERVER_RESPONDER_IMPL_NORMAL_H_
#define NAIVESERV_SERVER_RESPONDER_IMPL_NORMAL_H_

#include <string>

#include <server/responder/responder.h>

class NormalResponder : public ResponderInterface {
public:
    NormalResponder(const ArgList &args) {
        if (args.size() >= 1) url_prefix_ = args[0];
        // read index page
        if (args.size() >= 2) {
            index_page_ = args[1];
        }
        else {
            index_page_ = "index.html";
        }
    }

    HTTPResponse AcceptRequest(const HTTPParser &parser) const override;

private:
    std::string url_prefix_, index_page_;
};

#endif // NAIVESERV_SERVER_RESPONDER_IMPL_NORMAL_H_

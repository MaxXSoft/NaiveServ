#ifndef NAIVESERV_SERVER_RESPONDER_IMPL_NORMAL_H_
#define NAIVESERV_SERVER_RESPONDER_IMPL_NORMAL_H_

#include <string>

#include <server/responder/responder.h>

class NormalResponder : public ResponderInterface {
public:
    NormalResponder(const ArgList &args);

    HTTPResponse AcceptRequest(const HTTPParser &parser) const override;

private:
    std::string url_prefix_, index_page_, www_root_;
};

#endif // NAIVESERV_SERVER_RESPONDER_IMPL_NORMAL_H_

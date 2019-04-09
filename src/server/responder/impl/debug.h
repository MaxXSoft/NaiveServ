#ifndef NAIVESERV_SERVER_RESPONDER_IMPL_DEBUG_H_
#define NAIVESERV_SERVER_RESPONDER_IMPL_DEBUG_H_

#include <string>

#include <server/responder/responder.h>

class DebugResponder : public ResponderInterface {
public:
    DebugResponder(const ArgList &args);

    HTTPResponse AcceptRequest(const HTTPParser &parser) const override;

private:
    std::string debug_info_;
};

#endif // NAIVESERV_SERVER_RESPONDER_IMPL_DEBUG_H_

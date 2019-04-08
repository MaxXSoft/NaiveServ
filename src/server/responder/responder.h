#ifndef NAIVESERV_SERVER_RESPONDER_RESPONDER_H_
#define NAIVESERV_SERVER_RESPONDER_RESPONDER_H_

#include <memory>

#include <http/parser.h>
#include <http/response.h>

// interface of all responders
class ResponderInterface {
public:
    virtual ~ResponderInterface() = default;
    virtual HTTPResponse AcceptRequest(const HTTPParser &parser) const = 0;
};

using Responder = std::shared_ptr<ResponderInterface>;

#endif // NAIVESERV_SERVER_RESPONDER_RESPONDER_H_

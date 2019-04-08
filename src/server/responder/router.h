#ifndef NAIVESERV_SERVER_RESPONDER_ROUTER_H_
#define NAIVESERV_SERVER_RESPONDER_ROUTER_H_

#include <string>
#include <map>

#include <server/responder/responder.h>
#include <util/singleton.h>

// factory of responder (singleton)
class Router : public Singleton<Router> {
public:
    Responder GetResponder(const std::string &url) const;

    // getters
    const Responder &default_responder() const { return default_resp_; }

private:
    using ResponderTable = std::map<std::string, Responder>;

    friend class Singleton<Router>;

    Router();

    Responder default_resp_;
    ResponderTable table_;
};

#endif // NAIVESERV_SERVER_RESPONDER_ROUTER_H_

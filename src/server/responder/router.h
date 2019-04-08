#ifndef NAIVESERV_SERVER_RESPONDER_ROUTER_H_
#define NAIVESERV_SERVER_RESPONDER_ROUTER_H_

#include <string>
#include <map>

#include <server/responder/responder.h>
#include <util/singleton.h>

// factory of responder (singleton)
class Router : public Singleton<Router> {
public:
    const Responder &GetDefaultResponder() { return default_responder_; }
    const Responder &GetResponder(const std::string &name);
    const Responder &GetResponderByURL(const std::string &url);

private:
    using ResponderTable = std::map<std::string, Responder>;

    friend class Singleton<Router>;

    Router();

    Responder default_responder_;
    ResponderTable responders_;
};

#endif // NAIVESERV_SERVER_RESPONDER_ROUTER_H_

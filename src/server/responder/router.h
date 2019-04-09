#ifndef NAIVESERV_SERVER_RESPONDER_ROUTER_H_
#define NAIVESERV_SERVER_RESPONDER_ROUTER_H_

#include <string>
#include <map>
#include <ostream>

#include <server/responder/responder.h>
#include <util/singleton.h>

// factory of responder (singleton)
class Router : public Singleton<Router> {
public:
    Responder GetResponder(const std::string &url) const;
    void PrintAllResponders(std::ostream &os,
            const char *delimiter = ", ") const;
    void PrintRouterRules(std::ostream &os,
            const char *delimiter = "\n") const;

    // getters
    const Responder &default_responder() const { return default_resp_; }

private:
    using ResponderTable = std::map<std::string, Responder>;

    friend class Singleton<Router>;

    Router();

    void LogError(const char *message);
    void PrintRule(std::ostream &os, const ResponderRule &rule) const;

    Responder default_resp_;
    ResponderTable table_;
};

#endif // NAIVESERV_SERVER_RESPONDER_ROUTER_H_

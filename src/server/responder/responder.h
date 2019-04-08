#ifndef NAIVESERV_SERVER_RESPONDER_RESPONDER_H_
#define NAIVESERV_SERVER_RESPONDER_RESPONDER_H_

#include <memory>
#include <string>
#include <map>

class ResponderInterface {
public:
    virtual ~ResponderInterface() = default;
    //
};

using Responder = std::shared_ptr<ResponderInterface>;

// factory of responder (singleton)
class ResponderFactory {
public:
    static ResponderFactory &Instance();

    const Responder &GetDefaultResponder() { return default_responder_; }
    const Responder &GetResponder(const std::string &name);

private:
    ResponderFactory();

    // singleton
    static ResponderFactory *factory_instance_;
    Responder default_responder_;
    std::map<std::string, Responder> responders_;
};

#endif // NAIVESERV_SERVER_RESPONDER_RESPONDER_H_

#ifndef NAIVESERV_SERVER_RESPONDER_RESPONDER_H_
#define NAIVESERV_SERVER_RESPONDER_RESPONDER_H_

#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <map>

#include <http/parser.h>
#include <http/response.h>

// interface of all responders
class ResponderInterface {
public:
    virtual ~ResponderInterface() = default;
    virtual HTTPResponse AcceptRequest(const HTTPParser &parser) const = 0;
};

// type definitions about responder
using Responder = std::shared_ptr<ResponderInterface>;
using ArgList = std::vector<std::string>;
using ResponderRule = std::pair<std::string, ArgList>;
using ResponderRules = std::map<std::string, ResponderRule>;

#endif // NAIVESERV_SERVER_RESPONDER_RESPONDER_H_

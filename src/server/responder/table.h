#ifndef NAIVESERV_SERVER_RESPONDER_TABLE_H_
#define NAIVESERV_SERVER_RESPONDER_TABLE_H_

#include <string>

#include <server/responder/responder.h>
#include <server/responder/normal.h>

#define RESPONDER_DECLARE(name_var, resp) \
    if (##name_var == #resp) { \
        return std::make_shared<resp##Responder>(); \
    }

#define RESPONDER_EXPAND(name_var, responders) \
    responders(name_var, RESPONDER_DECLARE)

// declare all responders here
#define ALL_RESPONDERS(name_var, f) \
    f(name_var, Normal)

// get responders
inline Responder NewResponderByName(const std::string &name) {
    RESPONDER_EXPAND(name, ALL_RESPONDERS);
    // default path
    return nullptr;
}

#endif // NAIVESERV_SERVER_RESPONDER_TABLE_H_

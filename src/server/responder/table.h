#ifndef NAIVESERV_SERVER_RESPONDER_TABLE_H_
#define NAIVESERV_SERVER_RESPONDER_TABLE_H_

#include <string>

#include <server/responder/responder.h>
#include <server/responder/impl/normal.h>


// declare all responders here
#define ALL_RESPONDERS(f) \
    f(Normal)


// some other magic macros
#define RESPONDER_DECLARE(resp) \
    if (name == #resp) { \
        return std::make_shared<resp##Responder>(args); \
    }

#define RESPONDER_EXPAND(responders) \
    responders(RESPONDER_DECLARE)

// get responder
inline Responder NewResponder(const std::string &name,
        const ArgList &args) {
    RESPONDER_EXPAND(ALL_RESPONDERS);
    // default path
    return nullptr;
}

#endif // NAIVESERV_SERVER_RESPONDER_TABLE_H_

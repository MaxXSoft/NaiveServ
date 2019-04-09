#ifndef NAIVESERV_SERVER_RESPONDER_TABLE_H_
#define NAIVESERV_SERVER_RESPONDER_TABLE_H_

#include <string>

#include <server/responder/responder.h>
#include <server/responder/impl/normal.h>


// declare all responders here
#define ALL_RESPONDERS(f) \
    f(Normal)


// some other magic macros
#define NEW_RESPONDER(r) \
    if (name == #r) { \
        return std::make_shared<r##Responder>(args); \
    }

#define RESPONDER_NAME_ARRAY(r)     #r,

#define RESPONDER_VALID(r)          if (name == #r) { return true; }

#define RESPONDER_EXPAND(macro, responders) \
    responders(macro)

// create responder
inline Responder NewResponder(const std::string &name,
        const ArgList &args) {
    RESPONDER_EXPAND(NEW_RESPONDER, ALL_RESPONDERS);
    // default path
    return nullptr;
}

// check responder name is valid
inline bool IsResponderValid(const std::string &name) {
    RESPONDER_EXPAND(RESPONDER_VALID, ALL_RESPONDERS);
    return false;
}

#endif // NAIVESERV_SERVER_RESPONDER_TABLE_H_

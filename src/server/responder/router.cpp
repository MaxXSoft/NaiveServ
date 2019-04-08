#include <server/responder/router.h>

#include <server/responder/table.h>

Router::Router() {
    // initialize factory
    default_responder_ = std::make_shared<NormalResponder>();
    // TODO: read config
    // TODO: implement NormalResponder
    // TODO: implement Worker
}

const Responder &Router::GetResponder(const std::string &name) {
    auto it = responders_.find(name);
    if (it != responders_.end()) {
        return it->second;
    }
    else {
        return default_responder_;
    }
}

const Responder &Router::GetResponderByURL(
        const std::string &url) {
    // TODO
}

#include <server/responder/responder.h>

#include <server/responder/normal.h>

// initialize instance
ResponderFactory *ResponderFactory::factory_instance_ =
        new ResponderFactory();

ResponderFactory::ResponderFactory() {
    // initialize factory
    default_responder_ = std::make_shared<NormalResponder>();
    // TODO: read config
    // TODO: add submodule of RapidJSON
    // TODO: implement NormalResponder
    // TODO: implement Worker
}

const Responder &ResponderFactory::GetResponder(const std::string &name) {
    auto it = responders_.find(name);
    if (it != responders_.end()) {
        return it->second;
    }
    else {
        return default_responder_;
    }
}

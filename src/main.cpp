#include <server/core.h>

namespace {

//

} // namespace

int main(int argc, const char *argv[]) {
    auto &core = Core::Instance();
    core.StartListen();
    return 0;
}

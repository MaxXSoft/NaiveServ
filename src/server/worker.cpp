#include <server/worker.h>

#include <thread>

namespace {

//

} // namespace

void Worker::StartWork() {
    std::thread work(&Worker::HandleConnection, this);
    work.detach();
}

void Worker::HandleConnection() {
    //
}

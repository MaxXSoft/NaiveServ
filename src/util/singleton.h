#ifndef NAIVESERV_UTIL_SINGLETON_H_
#define NAIVESERV_UTIL_SINGLETON_H_

#include <mutex>

// a base template class of singleton (thread-safe)

template <typename T>
class Singleton {
public:
    Singleton(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    virtual ~Singleton() = default;

    const Singleton &operator=(const Singleton &) = delete;
    const Singleton &operator=(Singleton &&) = delete;

    static T &Instance() {
        static std::once_flag flag;
        std::call_once(flag, &Singleton::GetInstance);
        return GetInstance();
    }

protected:
    Singleton() {}

private:
    static T &GetInstance() {
        static T instance;
        return instance;
    }
};

#endif // NAIVESERV_UTIL_SINGLETON_H_

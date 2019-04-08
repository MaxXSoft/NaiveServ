#include <iostream>

#include <util/singleton.h>

using namespace std;

class Test : public Singleton<Test> {
public:
    int NextNum() { return counter_++; }

private:
    friend class Singleton<Test>;

    Test() : counter_(0) {}

    int counter_;
};

int main(int argc, const char *argv[]) {
    cout << Test::Instance().NextNum() << endl;
    cout << Test::Instance().NextNum() << endl;
    cout << Test::Instance().NextNum() << endl;
    return 0;
}

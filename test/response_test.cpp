#include <iostream>
#include <string>

#include <http/response.h>

using namespace std;

void GetResponse(int status_code, const char *text) {
    HTTPResponse response(status_code);
    auto data = reinterpret_cast<const uint8_t *>(text);
    response.set_data(data, strlen(text));
    cout << response.ToString() << endl;
}

int main(int argc, const char *argv[]) {
    if (argc < 2 || !strcmp(argv[1], "-d")) {
        GetResponse(200, "你好，我是Response.");
    }
    else if (argc >= 2 || !strcmp(argv[1], "-t")) {
        cout << "Please input the content: " << endl;
        string s;
        getline(cin, s);
        s += '\n';
        GetResponse(200, s.c_str());
    }
    return 0;
}

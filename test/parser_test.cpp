#include <iostream>

#include <http/parser.h>

using namespace std;

namespace {

static constexpr const char *kMethodString[] = {
	"Get", "Post", "Other"
};

} // namespace

int main(int argc, const char *argv[]) {
	// input:
	// POST /mix/76.html?name=kelvin&password=123456 HTTP/1.1
	// Host: www.fishbay.cn
	// Upgrade-Insecure-Requests: 1
	// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36
	// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
	// 
	// yingyingying
	// miaomiaomaio
 	// fafafa

	string s;
	s = "POST /mix/76.html?name=kelvin&password=123456 HTTP/1.1\r\n";
	s += "Upgrade-Insecure-Requests: 1\r\n";
	s += "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36\r\n";
	s += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
	s += "\r\n";
	s += "yingyingying\r\n";
	s += "miaomiaomaio\r\n";
	s += "fafafa";
	cout << s.size() << endl;
	HTTPParser test(s);
	cout << "Method: ";
	cout << kMethodString[static_cast<int>(test.method())] << endl;
	cout << "URL: " << test.url() << endl;
	cout << "Version: ";
	cout << test.major_version() << "." << test.minor_version() << endl;
	cout << "User-Agent: " << test.GetFieldValue("User-Agent") << endl;
	cout << "Accept: " << test.GetFieldValue("Accept") << endl;
	cout << "Data Length: " << test.data().size() << endl;
	return 0;
}

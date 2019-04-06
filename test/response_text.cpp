#include <http/response.h>

#include <iostream>
#include <string>
using namespace std;

void GetResponse(int status_code, const char* text){
	HTTPResponse response(status_code);

	response.set_data(reinterpret_cast<const uint8_t*>(text), strlen(text));

	cout << response.ToString();
}

int main(int argc, const char* argv[]){
	
	if (argc < 2 || !strcmp(argv[1], "-d")) {
		const char* text = "你好，我是Response.";
		GetResponse(200, text);
	}

	if(argc >= 2 || !strcmp(argv[1], "-t")) {
		cout << "Please input the content: " << endl;

		string s;
		getline(cin, s);
		s += '\n';

		const char* text = s.c_str();
		GetResponse(200, text);
	}

	return 0;
}

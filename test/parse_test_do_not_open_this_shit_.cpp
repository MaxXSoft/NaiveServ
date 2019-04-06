#include<bits/stdc++.h>
using namespace std;

// copy from parser.cpp & parser.h
// because i am too lazy to tian_jia_tou_wen_jian


typedef std::string DataArray;

class HTTPParser {
public:
    

    enum class Method {
        Get, Post, Other
    };

    HTTPParser() {}
    HTTPParser(const std::string &request) { Parse(request); }

    // parse specific request
    void Parse(const std::string &request);

    // get the value of specific field name in current HTTP request.
    // if field name not found, returns empty string
    const std::string &GetFieldValue(const std::string &field_name) {
        return field_info_[field_name];
    }

    // getters
    Method method() const { return method_; }
    const std::string &url() const { return url_; }
    int major_version() const { return major_version_; }
    int minor_version() const { return minor_version_; }
    const DataArray &data() const { return data_; }

private:
    // HTTP request method.
    // GET, POST... and so on
    Method method_;
    // requested URL
    std::string url_;
    // HTTP protocol version (e.g. 1.1)
    int major_version_, minor_version_;
    // storing field information.
    // e.g. field_info_["Host"] = "www.baidu.com";
    std::map<std::string, std::string> field_info_;
    // requested data.
    // this field is valid when client transfer data using POST method
    DataArray data_;
};

void HTTPParser::Parse(const std::string &request) {
    // TODO: implement this method
 	int now_pos(0);

	// the first line consis of:
	// method url HTTP/vesion

	// GET, POST... and so on
    //Method method_;
    /*
    enum class Method {
        Get, Post, Other
    };
    */
   	
   	/*
   	switch(request[0]) {
   		case 'G' : method_ = Method::Get; break;
   		case 'P' : method_ = Method::Post; break;
   		default : method_ = Method::Other;
   	}
    */

    // requested URL
    // std::string url_;
    int st(0),ed;
    for(int i=0; ; ++i) {
    	if(request[i] == ' ') {
    		if(st == 0)
    			st = i;
    		else {
				ed = i;
    			break;
    		}
    	}
    }
    url_ = request.substr(st+1, ed-st-1);

    // HTTP protocol version (e.g. 11.11)
    // int major_version_, minor_version_;
    // request[i] is '/'
    for(int i=4; ; ++i) { 
    	if(request.substr(i-4, 5) == "HTTP/") {
    		int j = i+1;
    		std::string num = "\0";
    		while(request[j] != '.') {
    			num += request[j];
    			j++;
    		}
    		int tmp(1);
    		major_version_ = 0;
    		for(int k=num.size()-1; k>=0; --k) {
    			major_version_ += tmp * (num[k] - '0');
    			tmp *= 10;
    		}

    		num.clear();
    		j++;
    		while(request[j] >= '0' && request[j] <= '9') {
    			num += request[j];
    			j++;
    		}
    		tmp = 1;
    		minor_version_ = 0;
    		for(int k=num.size()-1; k>=0; --k) {
    			minor_version_ += tmp * (num[k] - '0');
    			tmp *= 10;
    		}
    		
    		now_pos = j; // \r of the first line
    		break;
    	}
    }

    
    // storing field information.
    // e.g. field_info_["Host"] = "www.baidu.com";
    // std::map<std::string, std::string> field_info_;
    bool havedata = false;
    while(now_pos < request.size()) {
    	now_pos += 2;//first position of this line
    	int st = now_pos; 
    	bool isfind = false;
    	std::string field_;
    	std::string info_;

    	while(request[now_pos] != '\r' && request[now_pos+1] != '\n') {
    		if(!isfind && request[now_pos] == ':') {
    			field_ = request.substr(st, now_pos-st);
    			isfind = true;
    			now_pos++;
    			st = now_pos+1;
    		}
    		now_pos++;
    	}
    	//now_pos is \r of this line
    	
    	if(isfind) {
    		info_ = request.substr(st,now_pos-st);
    		field_info_[field_] = info_;
    	}
    	
    	if(request[now_pos+2] == '\r' && request[now_pos+3] == '\n') {
    		havedata = true;
    		break;
    	}
    }


    // requested data.
    // this field is valid when client transfer data using POST method
    // DataArray data_;
    if(havedata) {
    	now_pos += 4; // first position of the data 
    	data_ = request.substr(now_pos,request.size()-now_pos);
    }
}

int main()
{
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
	s = "POST /mix/76.html?name=kelvin&password=123456 HTTP/1.1";
	s += "\r\n";
	s += "Upgrade-Insecure-Requests: 1";
	s += "\r\n";
	s += "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36";
	s += "\r\n";
	s += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
	s += "\r\n";
	s += "\r\n";
	s += "yingyingying";
	s += "\r\n";
	s += "miaomiaomaio";
	s += "\r\n";
	s += "fafafa";
	cout << s.size() << endl;
	HTTPParser test(s);
	//std::cout << "method==" << test.method() << std::endl;
	std::cout << "url :\n" << test.url() << std::endl;
	std::cout << "version :\n" << test.major_version() << "." << test.minor_version() << std::endl;
	std::cout << "User-Agent :\n" << test.GetFieldValue("User-Agent") << std::endl;
	std::cout << "DataArray :\n" << test.data() << std::endl;
	
}


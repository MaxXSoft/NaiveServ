#include <http/parser.h>

namespace {

// TODO: put all of internal constants, variables or functions here,
//       or just delete this scope

} // namespace

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
   
   	switch(request[0]) {
   		case 'G' : method_ = Method::Get; break;
   		case 'P' : method_ = Method::Post; break;
   		default : method_ = Method::Other;
   	}
    

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
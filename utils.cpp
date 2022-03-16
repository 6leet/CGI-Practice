#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

vector<string> cgi_envs{"REQUEST_METHOD", "REQUEST_URI", "SERVER_PROTOCOL", "HTTP_HOST"};

int _setenv(string var, string value) {
    return setenv(var.c_str(), value.c_str(), 1);
}

int _printenv(string var) {
    cout << var << '=' << flush;
    char* env = getenv(var.c_str());
    if (env != NULL) {
        cout << env << flush;
    }
    cout << '\n' << flush;
    return 0;
}

void printcgienv() {
    _printenv("REQUEST_METHOD");
    _printenv("REQUEST_URI");
    _printenv("QUERY_STRING");
    _printenv("SERVER_PROTOCOL");
    _printenv("HTTP_HOST");
    _printenv("SERVER_ADDR");
    _printenv("SERVER_PORT");
    _printenv("REMOTE_ADDR");
    _printenv("REMOTE_PORT");
}

int _parse_request_uri_query_string() {
    string _tmp_request_uri(getenv("REQUEST_URI"));
    int split = _tmp_request_uri.find('?');
    if (split == string::npos) {
        string _request_uri = "." + _tmp_request_uri;
        _setenv("REQUEST_URI", _request_uri);
        return -1;
    }
    string _request_uri = "." + _tmp_request_uri.substr(0, split);
    string _query_string = _tmp_request_uri.substr(split + 1, _tmp_request_uri.size() - split - 1);
    _setenv("REQUEST_URI", _request_uri);
    _setenv("QUERY_STRING", _query_string);
    return 0;
}

int parse_http(string http_header) {
    stringstream ss(http_header);
    string buf;
    int ei = 0;

    while (ss >> buf) {
        if (ei >= cgi_envs.size()) {
            break;
        }
        if (buf.back() == ':') {
            continue;
        }
        _setenv(cgi_envs[ei], buf);
        ei++;
    }
    _parse_request_uri_query_string();
    return 0;
}

void set_env(string server_addr, unsigned short server_port, string remote_addr, unsigned short remote_port) {
    _setenv("SERVER_ADDR", server_addr);
    _setenv("SERVER_PORT", to_string(server_port));
    _setenv("REMOTE_ADDR", remote_addr);
    _setenv("REMOTE_PORT", to_string(remote_port));
}


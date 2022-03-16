#include <stdlib.h>
#include <unistd.h>
// #include <stdio.h>
#include <iostream>

using namespace std;

void http200() {
    cout << "HTTP/1.1 200 OK\r\n" << flush;
}

int _cgi_handler(string cgi_name) {
    char *arg[2] = { const_cast<char*>(cgi_name.c_str()), NULL };
    execvp(cgi_name.c_str(), arg);
    if (errno == 2) { // 404
        return -1;
    }
    return 0;
}

void _debug() {
    cout << "Content-type: text/html\r\n\r\n" << flush;
}

void middleware() {
    http200();
    _cgi_handler(getenv("REQUEST_URI"));
}

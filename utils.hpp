#ifndef UTILS
#define UTILS

#include <string>

using namespace std;

int printcgienv();
int parse_http(string);
void set_env(string, unsigned short, string, unsigned short);

#endif
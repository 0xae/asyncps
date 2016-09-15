#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.cpp"

using std::string;
using std::cout;

int main(int argc, char *argv[]){
    string msg = "publish mongo 'hello world'\r\n";
    int f = msg.find("\r\n");
    cout << msg.substr(0, f) << "\n";
    return 0;
}

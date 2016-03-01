#include "microhttp_server.hpp"
#include <iostream>

using namespace std;

int main (int argc, char *const *argv) {
    const char *directory = "/Users/skar/Desktop/lextest";
    short port = 3001;
    if (argc == 3) {
        port = stoi(argv[1]);
        directory = argv[2];
    } else {
        printf("usage:\npx-cpp-server PORT HOME_DIR \n");
        return -1;
    }
    return startup_server(port, directory);
}
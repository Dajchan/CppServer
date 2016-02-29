#include "microhttp_server.hpp"
#include <iostream>

using namespace std;

int main (int argc, char *const *argv) {
    const char *directory = "/Users/skar/Desktop/lextest";
    short port = 3001;
    if (argc == 3) {
//        port =
//        directory =
    }
    return startup_server(port, directory);
}
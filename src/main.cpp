#include "microhttp_server.hpp"
#include <iostream>

using namespace std;

int main (int argc, char *const *argv) {
    short port = 3001;
    return startup_server(port);
}
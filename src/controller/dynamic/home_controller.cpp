//
//  home_controller.cpp
//  px
//
//  Created by Jonathan Cichon on 03.01.16.
//
//

#include "home_controller.hpp"
#include "application.hpp"

using namespace px;

unsigned int HomeController::body(std::ostringstream& stream, Hash_p params) {
//    auto entries = Application::Instance().blog_entries();

    return 200;
}

void HomeController::cache_key(Hash_p params, string &out) {
    out = "home";
}

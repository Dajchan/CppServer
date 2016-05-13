//
//  home_controller.cpp
//  px
//
//  Created by Jonathan Cichon on 03.01.16.
//
//

#include "home_controller.hpp"
#include "application.hpp"
#include "html_helper.hpp"

using namespace px;

unsigned int HomeController::body(std::ostringstream& stream, Hash_p params) {
    auto entries = Application::Instance().blog_entries();
    
    if (entries.size() > 3) {
        auto subset = vector<BlogEntry_p>(entries.begin(), entries.begin() + 3);
        px::render_list(stream, subset);
    } else {
        px::render_list(stream, entries);
    }
    
    return 200;
}

void HomeController::cache_key(Hash_p params, string &out) {
    out = "home";
}

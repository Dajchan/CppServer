#include "home_controller.hpp"
#include "application.hpp"
#include "html_helper.hpp"
#include "helper.hpp"

using namespace px;

unsigned int HomeController::body(std::ostringstream& stream, Hash_p params) {
    auto entries = Application::Instance().blog_entries();
    
    shared_ptr<std::pair<string, string>> html_parts = nullptr;
    
    auto res = Application::Instance().get_page("home.html");
    if (res) {
        size_t length;
        auto c = res->data(&length);
        if (c && length) {
            string out = string(c, length);
            vector<string> parts;
            px::split(out, "<!-- YIELD_CONTENT -->", parts);
            
            if (parts.size() == 2) {
                html_parts = shared_ptr<std::pair<string, string>>(new std::pair<string, string>(parts.front(), parts.back()));
            }
        }
    }
    
    if (html_parts) {
        stream << html_parts->first;
    }
    
    if (entries.size() > 3) {
        auto subset = vector<BlogEntry_p>(entries.begin(), entries.begin() + 3);
        px::render_list(stream, subset);
    } else {
        px::render_list(stream, entries);
    }
    
    if (html_parts) {
        stream << html_parts->second;
    }
    
    return 200;
}

void HomeController::cache_key(Hash_p params, string &out) {
    out = "home";
}

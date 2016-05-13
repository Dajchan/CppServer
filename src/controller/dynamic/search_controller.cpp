#include "search_controller.hpp"
#include "application.hpp"
#include "helper.hpp"
#include "html_helper.hpp"

using namespace px;

unsigned int SearchController::body(std::ostringstream& stream, Hash_p params) {
    auto query = params->get(px::Param::SearchQuery);
    if (query) {
        
        auto query_string = px::downcase(query->string_value());
        
        auto& entries = Application::Instance().blog_entries();
        
        vector<BlogEntry_p> selected;
        for (auto& entry : entries) {
            if (px::downcase(entry->title()).find(query_string) != std::string::npos || px::includes(entry->photos(), [&](const Photo_p& p){return px::downcase(p->title()).find(query_string) != std::string::npos;})) {
                selected.push_back(entry);
            }
        }
        
        if (selected.size() > 0) {
            px::render_list(stream, selected);
            return 200;
        } else {
            px::render_empty(stream, "No results found");
            return 200;
        } 
    }
    return 404;
}

void SearchController::cache_key(Hash_p params, string& out) {
    out = px::String::Empty();
}
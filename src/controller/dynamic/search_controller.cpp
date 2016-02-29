#include "search_controller.hpp"
#include "application.hpp"
#include "helper.hpp"

using namespace px;

unsigned int SearchController::body(std::ostringstream& stream, Hash_p params) {
    auto query = params->get(px::Param::SearchQuery);
    if (query) {
        
        auto& entries = Application::Instance().blog_entries();
        
        vector<BlogEntry_p> selected;
        for (auto& entry : entries) {
            if (entry->title().find(query->string_value()) != std::string::npos || px::includes(entry->photos(), [&](const Photo_p& p){return p->title().find(query->string_value()) != std::string::npos;})) {
                selected.push_back(entry);
            }
        }
        
        // TODO: possible share rendering with list_controller ... maybe start wie ViewRendering
    }
    return 404;
}

void SearchController::cache_key(Hash_p params, string& out) {
    out = px::String::Empty();
}
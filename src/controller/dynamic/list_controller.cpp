#include "list_controller.hpp"
#include "application.hpp"
#include "html_helper.hpp"

using namespace px;

unsigned int ListController::body(std::ostringstream& stream, Hash_p params) {
    auto entries = Application::Instance().blog_entries();
    if (entries.size() > 0) {
        px::render_list(stream, entries);
        return 200;
    } else {
        px::render_empty(stream, "No entries in category Blog");
        return 200;
    }
    return 404;
}

void ListController::cache_key(Hash_p params, string& out) {
    auto action = params->get(px::Param::ActionName);
    auto page_nr = params->get(px::Param::PageNR);
    if (action) {
        if (page_nr) {
            out = action->string_value() + page_nr->as_string()->string_value();
        } else {
            out = action->string_value();
        }
        return;
    }
    out = px::String::Empty();
}
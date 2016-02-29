#include "list_controller.hpp"
#include "application.hpp"

using namespace px;

static unsigned short page_size = 9;

unsigned int ListController::body(std::ostringstream& stream, Hash_p params) {
    LogDebug("body called");
    
    size_t p_nr = 0;
    auto page_nr = params->get(px::Param::PageNR);
    if (page_nr) {
        p_nr = page_nr->long_value();
    }
    
    auto entries = Application::Instance().blog_entries();
    if (entries.size() > page_size * p_nr) {
        
        LogDebug("nr entries: %d", (int)entries.size());
        
        
        for (size_t i = page_size*p_nr; i<entries.size(); i++) {
            auto& entry = entries[i];
            if (entry->photos().size() > 0) {
                auto photo = entry->photos().front(); // get default photo
                stream << "<div class=\"list-item\">" << std::endl;
                stream << "<div class=\"list-item-content-wrapper\">" << std::endl;
                stream << "<article class=\"list-item-content\">" << std::endl;
                
                stream << "<a href=\"/show?" << px::Param::BlogID << "=" << entry->identifier() << "\">" << std::endl;
                stream << "<img src=\"/image?" << px::Param::BlogID << "=" << entry->identifier() << "&" << px::Param::PhotoID << "=" << photo->identifier() << "\" />" << std::endl;
                stream << "</a>" << std::endl;
                
                stream << "</article>" << std::endl;
                stream << "</div>" << std::endl;
                stream << "</div>" << std::endl;
            }
            
        }
        
        // TODO: add pagination
        
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
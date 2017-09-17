#include "html_helper.hpp"
#include "url.hpp"
#include "helper.hpp"

namespace px {

    void render_list(std::ostringstream& stream, const vector<BlogEntry_p>& entries) {
        for (size_t i = 0; i<entries.size(); i++) {
            auto& entry = entries[i];
            if (entry->photos().size() > 0) {
                auto photo = entry->photos().front(); // get default photo
                stream << "<div class=\"list-item\">" << std::endl;
                stream << "<div class=\"list-item-content-wrapper\">" << std::endl;
                stream << "<article class=\"list-item-content\">" << std::endl;
                
                stream << "<a href=\"/show?" << px::Param::BlogID << "=" << entry->identifier() << "\">" << std::endl;
                stream << "<img src=\"/image?" << px::Param::BlogID << "=" << entry->identifier() << "&" << px::Param::PhotoID << "=" << photo->identifier() << "\" />" << std::endl;
                stream << "<div class=\"list-item-overlay\"><div class=\"vertical-center-box\"><p>" << entry->title();
                if (entry->description()) {
                    stream << "<br/>";
                    size_t size=0;
                    auto buffer = entry->description()->load(&size);
                    stream << px::xmlescape(string(buffer, size));
                    free(buffer);
                }
                stream << "</p></div></div>" << std::endl;
                stream << "</a>" << std::endl;
                
                stream << "</article>" << std::endl;
                stream << "</div>" << std::endl;
                stream << "</div>" << std::endl;
            }
        }
    }
    
    void render_empty(std::ostringstream& stream, const string& empty_message) {
        stream << "<p>" << std::endl;
        stream << empty_message << std::endl;
        stream << "</p>" << std::endl;
    }
    
}
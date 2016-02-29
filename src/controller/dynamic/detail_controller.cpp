#include "detail_controller.hpp"
#include "application.hpp"

using namespace px;

unsigned int DetailController::body(std::ostringstream& stream, Hash_p params) {
    auto blog_id = params->get(px::Param::BlogID);
    if (blog_id) {
        if (auto blog = Application::Instance().get(blog_id->string_value())) {
            
            stream << "<p>" << std::endl;
            
            auto& photos = blog->photos();
            
            for (auto& photo : photos) {
                
                stream << "<img src=\"/image?" << px::Param::BlogID << "=" << blog_id->string_value() << "&" << px::Param::PhotoID << "=" << photo->identifier() << "\" />";
                if (photo != photos.back()) {
                    stream << "</br>";
                }
                stream << std::endl;
            }
            stream << "</p>" << std::endl;
            
            return 200;
        }
        
        // TODO: add next and prev
    }
    return 404;
}

void DetailController::cache_key(Hash_p params, string& out) {
    auto action = params->get(px::Param::ActionName);
    auto blog_id = params->get(px::Param::BlogID);
    if (action && blog_id) {
        out = action->string_value() + blog_id->string_value();
        return;
    }
    out = px::String::Empty();
}
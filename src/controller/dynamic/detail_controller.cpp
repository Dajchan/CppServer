#include "detail_controller.hpp"
#include "application.hpp"
#include "mime_type.hpp"

using namespace px;

unsigned int DetailController::body(std::ostringstream& stream, Hash_p params) {
    auto blog_id = params->get(px::Param::BlogID);
    if (blog_id) {
        if (auto blog = Application::Instance().get(blog_id->string_value())) {

            bool image = false;
            auto& resources = blog->resources();
            for (auto& resource : resources) {
                
                
                if (auto photo = std::dynamic_pointer_cast<Photo>(resource)) {
                    
                    if (!image) {
                        stream << "<p>" << std::endl;
                    } else {
                        stream << "<br/>" << std::endl;
                    }
                    stream << "<img src=\"/image?" << px::Param::BlogID << "=" << blog_id->string_value() << "&" << px::Param::PhotoID << "=" << photo->identifier() << "\" />" << std::endl;
                    image = true;
                } else {
                    if (image) {
                        stream << "</p>" << std::endl;
                    }
                    
                    stream << "<p>" << std::endl;
                    size_t size=0;
                    auto buffer = resource->load(&size);
                    stream.write(buffer, size);
                    free(buffer);
                    stream << "</p>" << std::endl;
                    image = false;
                }
            }
            if (image) {
                stream << "</p>" << std::endl;
            }
            
            return 200;
        }
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
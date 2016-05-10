#include "static_controller.hpp"
#include "application.hpp"


using namespace px;

unsigned int StaticController::body(std::ostringstream& stream, Hash_p params) {
    if (auto page_name = params->get(px::Param::FileName)) {

        if (auto resource = Application::Instance().get_page(page_name->string_value() + ".html")) {
            size_t size=0;
            auto buffer = resource->load(&size);
            stream.write(buffer, size);
            free(buffer);
            return 200;
        }
    }
    return 404;
}

void StaticController::cache_key(Hash_p params, string& out) {
    if (auto page_id = params->get(px::Param::ActionName)) {
        out = page_id->string_value();
        return;
    }
    out = px::String::Empty();
}

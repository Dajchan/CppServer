#include "image_controller.hpp"
#include "application.hpp"

using namespace px;

Response_p ImageController::call(HTTP_Method method, Hash_p params) {
    if (method == HTTP_MethodGet) {
        auto blog_id = params->get(px::Param::BlogID);
        auto photo_id = params->get(px::Param::PhotoID);
        if (blog_id && photo_id) {
            if (auto blog = Application::Instance().get(blog_id->string_value())) {
                if (auto photo = blog->get(photo_id->string_value())) {
                    if (!photo->path().empty() && !photo->mime_type().empty()) {
                        return Response::New(photo, 200);
                    }
                }
            }
        } else {
            LogDebug("params incorrect %s", params->to_query().c_str());
        }
    }
    return nullptr;
}
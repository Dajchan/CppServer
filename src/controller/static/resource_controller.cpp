#include "resource_controller.hpp"
#include "application.hpp"

using namespace px;

Response_p ResourceController::call(HTTP_Method method, Hash_p params) {
    if (method == HTTP_MethodGet) {
        auto resource_id = params->get(px::Param::ResourceID);
        if (resource_id) {
            if (auto resource = Application::Instance().get_resource(resource_id->string_value())) {
                return Response::New(resource, 200);
            } else {
                LogDebug("could not get resource: %s", resource_id->string_value().c_str());
            }
        } else {
            LogDebug("no resource_id given: %s", params->to_query().c_str());
        }
    }
    return nullptr;
}

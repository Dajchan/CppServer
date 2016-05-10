#include "response.hpp"

using namespace px;

Response_p Response::New(const string& data, unsigned int status, const string& mime_type) {
    return Response_p(new Response(data, status, mime_type));
}

Response_p Response::New(Resource_p resource, unsigned int status) {
    auto r = Response_p(new Response(resource, status));
    if (resource) {
        r->set_last_modified(resource->last_modified());
    }
    return r;
}

Response::Response(const string& data, unsigned int status, const string& mime_type) : m_data(data), m_status_code(status), m_mime_type(mime_type) {
    
}

Response::Response(Resource_p resource, unsigned int status) : m_status_code(status), m_resource(resource) {
    
}

Response::~Response() {}

const string &Response::data() const {
    return m_data;
}

unsigned int Response::status_code() const {
    return m_status_code;
}

const string &Response::mime_type() const {
    if (m_resource) {
        return m_resource->mime_type();
    }
    return m_mime_type;
}

const Resource_p Response::resource() const {
    return m_resource;
}

const Date &Response::last_modified() const {
    return m_last_modified;
}

void Response::set_last_modified(const Date& date) {
    m_last_modified = date;
}

#pragma once
#include "defines.hpp"
#include "resource.hpp"

namespace px {
	class Response;
	typedef shared_ptr<Response> Response_p;
	
	class Response {
	public:
        static Response_p New(const string& data, unsigned int status, const string& mime_type);
        static Response_p New(Resource_p resource, unsigned int status);
        ~Response();
        
        const string &data() const;
        
        unsigned int status_code() const;
        
        const string &mime_type() const;
        
        const Resource_p resource() const;
        
	private:
		string m_data;
		unsigned int m_status_code;
        string m_mime_type;
        Resource_p m_resource;
        
        Response(const string& data, unsigned int status, const string& mime_type);
        Response(Resource_p resource, unsigned int status);
        Response(const Response& src) = delete;
        Response& operator=(const Response& rhs) = delete;
	};
}
#pragma once
#include "defines.hpp"
#include "resource.hpp"
#include "date.hpp"

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
        
        const Date &last_modified() const;
        
        void set_last_modified(const Date& date);
        
	private:
		string m_data;
		unsigned int m_status_code;
        string m_mime_type;
        Resource_p m_resource;
        Date m_last_modified;
        
        Response(const string& data, unsigned int status, const string& mime_type);
        Response(Resource_p resource, unsigned int status);
        Response(const Response& src) = delete;
        Response& operator=(const Response& rhs) = delete;
	};
}
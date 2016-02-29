#pragma once
#include "defines.hpp"
#include "date.hpp"

namespace px {
    class Resource;
    typedef shared_ptr<Resource> Resource_p;
    
    class Resource {
    public:
        static Resource_p New(const string& identifier, const string& path, bool cache=false);
        virtual ~Resource();
        
        const string& identifier() const;
        const string& path() const;
        const string& mime_type() const;
        const char * data(size_t* length) const;
        const Date& last_modified() const;
        
        char * load(size_t *length) const;
        
    protected:
        Resource(const string& identifier, const string& path, bool cache=false);
        
    private:
        string m_identifier;
        string m_path;
        string m_mime_type;
        char * m_data = nullptr;
        size_t m_data_length = 0;
        Date m_last_modified;
        
        Resource() = delete;
        Resource(const Resource& src) = delete;
        Resource& operator=(const Resource& rhs) = delete;
    };
}
#pragma once
#include "defines.hpp"
#include "resource.hpp"

namespace px {
    class Photo;
    typedef shared_ptr<Photo> Photo_p;
    
    class Photo : public Resource {
    public:
        static Photo_p New(const string& title, const string& identifier, const string& path);
        virtual ~Photo();
        
        const string& title() const;
        
    private:
        string m_title;
        
        Photo(const string& title, const string& identifier, const string& path);
        Photo(const Photo& src) = delete;
        Photo& operator=(const Photo& rhs) = delete;
    };
}

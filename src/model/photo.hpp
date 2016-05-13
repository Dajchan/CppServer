#pragma once
#include "defines.hpp"
#include "resource.hpp"

namespace px {
    class Photo;
    typedef shared_ptr<Photo> Photo_p;
    
    class Photo : public Resource {
    public:
        static Photo_p New(const string& title, const string& identifier, const string& path, const string& caption = px::String::Empty());
        virtual ~Photo();
        
        const string& title() const;
        const string& caption() const;
        
    private:
        string m_title;
        string m_caption;
        
        Photo(const string& title, const string& identifier, const string& path, const string& caption);
        Photo(const Photo& src) = delete;
        Photo& operator=(const Photo& rhs) = delete;
    };
}

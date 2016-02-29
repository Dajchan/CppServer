#pragma once
#include "defines.hpp"
#include "photo.hpp"

namespace px {
    class BlogEntry;
    typedef shared_ptr<BlogEntry> BlogEntry_p;
    
    class BlogEntry {
    public:
        static BlogEntry_p New(const string& title, const string& identifier);
        
        ~BlogEntry();
        
        void append(Photo_p photo);
        
        Photo_p get(const string& identifier) const;
        
        const vector<Photo_p>& photos() const;
        const string& title() const;
        const string& identifier() const;
        
        
        
    private:
        string m_title;
        string m_identifier;
        vector<Photo_p> m_photos;
        unordered_map<string, Photo_p> m_photos_map;
        
        BlogEntry(const string& title, const string& identifier);
        BlogEntry() = delete;
        BlogEntry(const BlogEntry& src) = delete;
        BlogEntry& operator=(const BlogEntry& rhs) = delete;
    };
}
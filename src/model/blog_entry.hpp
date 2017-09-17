#pragma once
#include "defines.hpp"
#include "photo.hpp"

namespace px {
    class BlogEntry;
    typedef shared_ptr<BlogEntry> BlogEntry_p;
    
    class BlogEntry {
    public:
        static BlogEntry_p New(const string& title, const string& identifier, Resource_p description);
        
        ~BlogEntry();
        
        void append(Resource_p resource);
        
        Photo_p get(const string& identifier) const;
        
        const vector<Resource_p>& resources() const;
        const vector<Photo_p>& photos() const;
        const string& title() const;
        const string& identifier() const;
        
        const Resource_p description() const;
        
    private:
        string m_title;
        string m_identifier;
        Resource_p m_description;
        
        vector<Resource_p> m_resources;
        vector<Photo_p> m_photos;
        unordered_map<string, Photo_p> m_photos_map;
        
        BlogEntry(const string& title, const string& identifier, Resource_p description);
        BlogEntry() = delete;
        BlogEntry(const BlogEntry& src) = delete;
        BlogEntry& operator=(const BlogEntry& rhs) = delete;
    };
}
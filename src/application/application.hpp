#pragma once
#include "defines.hpp"
#include "blog_entry.hpp"
#include "resource.hpp"
#include "logger.hpp"

namespace px {
    class Application {
    public:
        static bool Initialize(const char *directory);
        static Application& Instance();
        ~Application();
        
        const vector<BlogEntry_p>& blog_entries() const;
        
        BlogEntry_p get(const string& identifier) const;
        
        Resource_p get_resource(const string& identifier) const;
        
        Resource_p get_page(const string& identifier) const;
        
    private:
        static std::unique_ptr<Application> m_instance;
        static std::once_flag m_onceFlag;
        static std::mutex m_mutex;
        
        string m_directory;
        time_t m_last_modification_time=0;
        
        vector<BlogEntry_p> m_blog_entries;
        unordered_map<string, BlogEntry_p> m_blog_entries_map;
        unordered_map<string, Resource_p> m_resources;
        unordered_map<string, Resource_p> m_static_pages;
        
        string _blog_dir();
        string _asset_dir();
        string _static_dir();
        
        void _check_file_changes();
        
        void _load_content();
        void _flush_cache();
        
        void _load_blogs(const string& dir);
        
        Application(const char *directory);
        Application(const Application& src) = delete;
        Application& operator=(const Application& rhs) = delete;
    };
}
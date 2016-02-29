#include "application.hpp"
#include "logger.hpp"
#include "helper.hpp"
#include "base64.hpp"
#include "timer.hpp"
#include "controller.hpp"

using namespace px;


std::unique_ptr<Application> Application::m_instance;
std::once_flag Application::m_onceFlag;
std::mutex Application::m_mutex;

bool Application::Initialize(const char *directory) {
    bool is_dir = false;
    if (px::file_exists(directory, &is_dir) && is_dir) {
        m_instance.reset(new Application(directory));
        LogDebug("Application initialized");
        return true;
    }
    LogError("Application directory <%s> does not exist or is not a directory", directory);
    return false;
}

Application& Application::Instance() {
    if (!m_instance) {
        LogError("Application needs to be initialized before use");
        exit(1);
    }
    return *m_instance.get();
}

Application::Application(const char *directory) : m_directory(directory) {
    _check_file_changes();
    
    Timer::Create(10, [&]{
        this->_check_file_changes();
    });
}

Application::~Application() {}

const vector<BlogEntry_p>& Application::blog_entries() const {
    return m_blog_entries;
}

BlogEntry_p Application::get(const string& identifier) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto itr = m_blog_entries_map.find(identifier);
    if (itr != m_blog_entries_map.end()) {
        return itr->second;
    }
    return nullptr;
}

Resource_p Application::get_resource(const string& identifier) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto itr = m_resources.find(identifier);
    if (itr != m_resources.end()) {
        return itr->second;
    }
    return nullptr;
}

Resource_p Application::get_page(const string& identifier) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto itr = m_static_pages.find(identifier);
    if (itr != m_static_pages.end()) {
        return itr->second;
    }
    return nullptr;
}

string Application::_blog_dir() {
    return m_directory + "/blogs";
}

string Application::_asset_dir(){
    return m_directory + "/assets";
}

string Application::_static_dir() {
    return m_directory + "/static";
}

void Application::_check_file_changes() {
    string blog_dir_name = _blog_dir();
    string asset_dir_name = _asset_dir();
    string static_dir_name = _static_dir();
    time_t current_time = m_last_modification_time;
    
    for (auto t : {px::file_modification_time(asset_dir_name), px::file_modification_time(static_dir_name), px::file_modification_time(blog_dir_name)}) {
        if (t>current_time) {
            current_time = t;
        }
    }
    if (current_time == m_last_modification_time) {
        const vector<string> dir_names = {blog_dir_name, asset_dir_name, static_dir_name};
        auto blog_dirs = px::dir_content(blog_dir_name, FileTypeDir);
        auto asset_files = px::dir_content(asset_dir_name, FileTypeFile);
        auto static_pages = px::dir_content(static_dir_name, FileTypeFile);
        
        size_t i = 0;
        for (auto& dirs_or_files : {blog_dirs, asset_files, static_pages}) {
            for (auto& dir_or_file : dirs_or_files) {
                auto current = dir_names[i] + "/" + dir_or_file;
                auto t = px::file_modification_time(current);
                if (t>current_time) {
                    current_time = t;
                }
            }
            i++;
        }
    }
    
    if (current_time != m_last_modification_time) {
        m_last_modification_time = current_time;
        _flush_cache();
    }
}

void Application::_load_content() {
    string blog_dir_name = _blog_dir();
    string asset_dir_name = _asset_dir();
    string static_dir_name = _static_dir();
    
    auto blog_dirs = px::dir_content(blog_dir_name, FileTypeDir);
    for (auto& blog_name : blog_dirs) {
        // optional: Get info File
        auto current_dir = blog_dir_name + "/" + blog_name;
        auto photo_files = px::dir_content(current_dir, FileTypeFile);
        
        if (!photo_files.empty()) {
            BlogEntry_p entry = BlogEntry::New(blog_name, px::strip(base64_encode(blog_name), "="));
            m_blog_entries_map[entry->identifier()] = entry;
            m_blog_entries.push_back(entry);
            
            for (auto& photo_name : photo_files) {
                Photo_p photo = Photo::New(photo_name, px::strip(base64_encode(photo_name), "="), current_dir + "/" + photo_name);
                entry->append(photo);
            }
        }
    }
    
    auto asset_files = px::dir_content(asset_dir_name, FileTypeFile);
    for (auto& asset_name : asset_files) {
        Resource_p resource = Resource::New(asset_name, asset_dir_name + "/" + asset_name, true);
        m_resources[resource->identifier()] = resource;
    }
    
    auto static_pages = px::dir_content(static_dir_name, FileTypeFile);
    for (auto& page_name : static_pages) {
        Resource_p resource = Resource::New(page_name, static_dir_name + "/" + page_name, true);
        m_static_pages[resource->identifier()] = resource;
    }
}

void Application::_flush_cache() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_blog_entries.clear();
    m_blog_entries_map.clear();
    m_resources.clear();
    m_static_pages.clear();
    _load_content();
    
    HTMLController::FlushCache();
}


















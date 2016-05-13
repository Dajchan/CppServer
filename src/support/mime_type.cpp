#include "mime_type.hpp"
#include "helper.hpp"
#include "logger.hpp"

namespace px {
    
    // TODO: MimeType for icon missing
    
    namespace MimeType {
        namespace Image {
            const string JPG = "image/jpg";
            const string PNG = "image/png";
            const string SVG = "image/svg+xml";
        }
        namespace Text {
            const string HTML = "text/html";
            const string PLAIN = "text/plain";
            const string CSS = "text/css";
        }
    }
    
    namespace Extension {
        namespace Image {
            const string JPG = "jpg";
            const string JPEG = "jpeg";
            const string PNG = "png";
            const string SVG = "svg";
        }
        namespace Text {
            const string TXT = "txt";
            const string HTML = "html";
            const string CSS = "css";
        }
    }
    
    static const std::set<string> g_image_extensions = {Extension::Image::JPG, Extension::Image::JPEG, Extension::Image::PNG};
    
    static const unordered_map<string, string> g_mimetype_map = {
        {Extension::Image::JPG , MimeType::Image::JPG},
        {Extension::Image::JPEG , MimeType::Image::JPG},
        {Extension::Image::PNG , MimeType::Image::PNG},
        {Extension::Image::SVG , MimeType::Image::SVG},
        
        {Extension::Text::TXT , MimeType::Text::PLAIN},
        {Extension::Text::HTML , MimeType::Text::HTML},
        {Extension::Text::CSS , MimeType::Text::CSS},
    };
    
    const string& mime_type_from_path(const string& path) {
        string ext = px::downcase(file_extension(path));
        auto itr = g_mimetype_map.find(ext);
        if (itr != g_mimetype_map.end()) {
            return itr->second;
        }
        return MimeType::Text::PLAIN;
    }
    
    
    bool is_image_file(const string& path) {
        string ext = px::downcase(file_extension(path));
        if (g_image_extensions.find(ext) != g_image_extensions.end()) {
            return true;
        }
        return false;
    }
    
    bool is_html_file(const string& path) {
        string ext = px::downcase(file_extension(path));
        return ext == Extension::Text::HTML;
    }
    
    bool is_txt_file(const string& path) {
        string ext = px::downcase(file_extension(path));
        return ext == Extension::Text::TXT;
    }
}
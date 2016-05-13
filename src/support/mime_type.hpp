#pragma once
#include "defines.hpp"

namespace px {
    namespace MimeType {
        namespace Image {
            extern const string JPG;
            extern const string PNG;
            extern const string SVG;
        }
        namespace Text {
            extern const string HTML;
            extern const string PLAIN;
            extern const string CSS;
        }
    }
    
    namespace Extension {
        namespace Image {
            extern const string JPG;
            extern const string JPEG;
            extern const string PNG;
            extern const string SVG;
        }
        namespace Text {
            extern const string TXT;
            extern const string HTML;
            extern const string CSS;
        }
    }
    
    const string& mime_type_from_path(const string& path);
    
    bool is_image_file(const string& path);
    bool is_html_file(const string& path);
    bool is_txt_file(const string& path);
}

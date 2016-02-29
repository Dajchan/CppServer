#include "resource.hpp"
#include "helper.hpp"

using namespace px;

namespace px {
    
    // TODO: MimeType for icon, svg and other resources missing
    
    namespace MimeType {
        namespace Image {
            static const string JPG = "image/jpg";
            static const string PNG = "image/png";
            static const string SVG = "image/svg+xml";
        }
        namespace Text {
            static const string HTML = "text/html";
            static const string PLAIN = "text/plain";
            static const string CSS = "text/css";
        }
    }
    
    namespace Extension {
        namespace Image {
            static const string JPG = "jpg";
            static const string JPEG = "jpeg";
            static const string PNG = "png";
            static const string SVG = "svg";
        }
        namespace Text {
            static const string TXT = "txt";
            static const string HTML = "html";
            static const string CSS = "css";
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
        string ext = file_extension(path);
        auto itr = g_mimetype_map.find(ext);
        if (itr != g_mimetype_map.end()) {
            return itr->second;
        }
        return MimeType::Text::PLAIN;
    }
}

Resource_p Resource::New(const string& identifier, const string& path, bool cache) {
    return Resource_p(new Resource(identifier, path, cache));
}

Resource::Resource(const string& identifier, const string& path, bool cache) : m_identifier(identifier), m_path(path) {
    if (cache) {
        m_data = load(&m_data_length);
    }
    m_mime_type = mime_type_from_path(path);
    m_last_modified = Date(px::file_modification_time(path));
}

Resource::~Resource() {
    if (m_data) {
        free(m_data);
    }
}

const string& Resource::identifier() const {
    return m_identifier;
}

const string& Resource::path() const {
    return m_path;
}

const string& Resource::mime_type() const {
    return m_mime_type;
}

const char * Resource::data(size_t* length) const {
    *length = m_data_length;
    return m_data;
}

const Date& Resource::last_modified() const {
    return Date::Empty();
}

char * Resource::load(size_t *length) const {
    FILE *fp;
    size_t lSize;
    char *buffer;
    
    fp = fopen ( m_path.c_str() , "rb" );
    if( !fp ) {
        perror(m_path.c_str()),exit(1);
    }
    
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );
    
    /* allocate memory for entire content */
    buffer = (char *)malloc(lSize+1 );
    if( !buffer ) {
        fclose(fp),fputs("memory alloc fails",stderr),exit(1);
    }
    
    /* copy the file into the buffer */
    if( 1!=fread( buffer , lSize, 1 , fp) ) {
        fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);
    }
    
    fclose(fp);
    *length = lSize+1;
    return buffer;
}


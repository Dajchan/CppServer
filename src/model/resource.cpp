#include "resource.hpp"
#include "helper.hpp"
#include "logger.hpp"
#include "mime_type.hpp"

using namespace px;

Resource_p Resource::New(const string& identifier, const string& path, bool cache) {
    return Resource_p(new Resource(identifier, path, cache));
}

Resource::Resource(const string& identifier, const string& path, bool cache) : m_identifier(identifier), m_path(path) {
    if (cache) {
        m_data = load(&m_data_length);
    }
    m_mime_type = mime_type_from_path(path);
    m_last_modified = Date(file_modification_time(path));
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
    return m_last_modified;
}

char * Resource::load(size_t *length) const {
    FILE *fp;
    size_t lSize;
    char *buffer = nullptr;
    
    fp = fopen ( m_path.c_str() , "rb" );
    if( !fp ) {
        perror(m_path.c_str()),exit(1);
    }
    
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    
    if (lSize) {
        rewind( fp );
        
        /* allocate memory for entire content */
        buffer = (char *)malloc(lSize);
        if( !buffer ) {
            fclose(fp),fputs("memory alloc fails",stderr),exit(1);
        }
        
        /* copy the file into the buffer */
        if( 1!=fread( buffer , lSize, 1 , fp) ) {
            fclose(fp),free(buffer),fputs("entire read fails",stderr),exit(1);
        }
    }
    
    fclose(fp);
    *length = lSize;
    return buffer;
}


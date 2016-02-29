#include "url.hpp"
#include "helper.hpp"
#include "hash.hpp"
#include "logger.hpp"

using namespace px;

string URL::m_default_host = "localhost:5555";
void URL::DefaultHost(const string& host) {
    m_default_host = host;
}

string URL::m_default_schema = "http";
void URL::DefaultSchema(const string& schema) {
    m_default_schema = schema;
}

URL_p URL::Create(const string& relative_url, const string& host, const string& schema) {
    string query = px::String::Empty();
    string path = px::String::Empty();
    size_t index = relative_url.find('?');
    
    LogDebug("%s", relative_url.c_str());
    
    if (index != string::npos) {
        if (index) {
            path = relative_url.substr(0, index);
        }
        query = relative_url.substr(index);
    } else {
        path = relative_url;
    }
    return URL_p(new URL(schema, host, path, query));
}

URL::URL(const string& schema, const string& host, const string& path, const string& query) : m_schema(schema), m_host(host), m_path(path), m_query(query) {
    
}

URL::~URL() {
    if (m_query_params) {
        delete m_query_params;
    }
}

const string& URL::param(const string& param) const {
    URL * unconst = (URL *)this;
    auto params = unconst->_query_params();
    auto itr = params.find(param);
    if (itr != params.end()) {
        return itr->second;
    }
    return px::String::Empty();
}

void URL::set_param(const string& param, const string& value) {
    string& old = _query_params()[param];
    if (old != value) {
        old = value;
        m_dirty = true;
    }
}

const string& URL::query() const {
    if (m_dirty) {
        URL * unconst = (URL *)this; // hack to set during const, not that clean, but as m_dirty does not realy change the object its fine!
        unconst->m_dirty = false;
        string& query = unconst->m_query;
        query.clear();
        for (auto& pair : unconst->_query_params()) {
            if (query.empty()) {
                query += "?";
            } else {
                query += "&";
            }
            query += pair.first + "=" + pair.second;
        }
    }
    return m_query;
}

const string& URL::path() const {
    return m_path;
}

string m_controller;
string m_action;

void URL::url_string(string& out, bool append) const {
    if (!append) {
        out.clear();
    }
    out.append(m_schema);
    out.append("://");
    out.append(m_host);
    out.append("/");
    relative_url_string(out, true);
}

void URL::relative_url_string(string& out, bool append) const {
    if (!append) {
        out.clear();
    }
    out += m_path;
    out += query();
}

unordered_map<string, string>& URL::_query_params() {
    if (!m_query_params) {
        m_query_params = new unordered_map<string, string>();
        if (!m_query.empty()) {
            size_t length = m_query.length();
            const char *chars = m_query.c_str();
            size_t i=0;
            if (chars[0] == '?') {
                ++i;
            }
            
            string key_buffer;
            string value_buffer;
            bool v = false;
            for (; i<length; ++i) {
                if (v) {
                    if (chars[i] != '&') {
                        value_buffer += chars[i];
                    } else {
                        if (!key_buffer.empty()) {
                            (*m_query_params)[key_buffer] = value_buffer;
                        }
                        value_buffer.clear();
                        key_buffer.clear();
                        v = false;
                    }
                } else {
                    if (chars[i] != '=') {
                        key_buffer += chars[i];
                    } else {
                        v = true;
                    }
                }
            }
            if (!key_buffer.empty() && !value_buffer.empty()) {
                (*m_query_params)[key_buffer] = value_buffer;
            }
        }
    }
    return *m_query_params;
}

void URL::each_param(const function<void(const string& key, const string& value)>& block) const {
    URL * unconst = (URL *)this;
    auto params = unconst->_query_params();
    for (auto& e : params) {
        block(e.first, e.second);
    }
}

Hash_p URL::params() const {
    URL * unconst = (URL *)this;
    auto params = unconst->_query_params();
    if (!params.empty()) {
        Hash_p ret = Hash::Create();
        for (auto& e : params) {
            ret->set(e.first, Value::Create(e.second));
        }
        return ret;
    }
    return nullptr;
}



namespace px {
    namespace Param {
        const string PageNR = "page";
        const string BlogID = "b";
        const string PhotoID = "p";
        const string ResourceID = "res";
        const string ActionName = "action";
        const string SearchQuery = "q";
        
        static vector<string> valid_params = {PageNR, BlogID, PhotoID, ResourceID, SearchQuery};
        const vector<string>& ValidParams() {
            return valid_params;
        }
    }
}















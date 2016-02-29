#pragma once
#include "defines.hpp"

namespace px {
    class URL;
    typedef shared_ptr<URL> URL_p;
    
    class URL {
    public:
        static void DefaultHost(const string& host);
        static void DefaultSchema(const string& schema);
        static URL_p Create(const string& relative_url, const string& host=m_default_host, const string& schema=m_default_schema);
        
        const string& param(const string& param) const;
        void set_param(const string& param, const string& value);
        
        const string& query() const;
        const string& path() const;
        
        void url_string(string& out, bool append=false) const;
        void relative_url_string(string& out, bool append=false) const;
        
        void each_param(const function<void(const string& key, const string& value)>& block) const;
        
        Hash_p params() const;
        
        ~URL();
        
    private:
        static string m_default_host;
        static string m_default_schema;
        
        
        string m_schema;
        string m_host;
        string m_path;
        string m_query;
        bool m_dirty;
        unordered_map<string, string> *m_query_params = nullptr;
        
        unordered_map<string, string>& _query_params();
        
        URL(const string&, const string&, const string&, const string&);
        URL() = delete;
        URL(const URL& src) = delete;
        URL& operator=(const URL& rhs) = delete;
    };
    
    namespace Param {
        extern const string PageNR;
        extern const string BlogID;
        extern const string PhotoID;
        extern const string ResourceID;
        extern const string ActionName;
        extern const string SearchQuery;
        
        const vector<string>& ValidParams();
    }
}
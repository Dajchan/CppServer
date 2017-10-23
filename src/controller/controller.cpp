#include "controller.hpp"
#include "application.hpp"
#include "helper.hpp"
#include "logger.hpp"

using namespace px;

unordered_map<string, Response_p> HTMLController::m_page_cache;
std::mutex HTMLController::m_mutex;
shared_ptr<std::vector<string>> HTMLController::m_html_parts;

Response_p HTMLController::call(HTTP_Method method, Hash_p params) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    string key;
    cache_key(params, key);

    if (!key.empty()) {
        auto itr = m_page_cache.find(key);
        if (itr != m_page_cache.end()) {
            return itr->second;
        }
    }
    
    std::ostringstream ss;
    
    if (!m_html_parts) {
        
        auto res = Application::Instance().get_page("layout.html");
        if (res) {
            size_t length;
            auto c = res->data(&length);
            if (c && length) {
                string out = string(c, length);
                vector<string> parts;
                px::split(out, "<!-- YIELD_CONTENT -->", parts);
                
                if (parts.size() == 2) {
                    vector<string> top_parts;
                    px::split(parts.front(), "<!-- YIELD_PAGE_NAME -->", top_parts);

                    m_html_parts = shared_ptr<std::vector<string>>(new std::vector<string>({
                        top_parts.front(),
                        top_parts.back(),
                        parts.back()
                    }));
                }
            }
        } else {
            LogDebug("could not get layout.html");
        }
    }
    if (m_html_parts) {
        ss << m_html_parts->front();
        auto v = params->get(px::Param::ActionName);
        if (v && v->string_value().size() >= 2) {
            
            ss << v->string_value().substr(1, v->string_value().size()-1);
        } else {
            ss << key;
        }
        
        
        ss << m_html_parts->at(1);
        auto status = body(ss, params);
        if (status == 404) {
            return nullptr;
        }
        
        ss << m_html_parts->back();
        
        auto response = Response::New(ss.str(), status, "text/html");
        if (!key.empty() && response) {
            response->set_last_modified(Date());
            m_page_cache[key] = response;
        }
        return response;
    }

    
    return nullptr;
}

void HTMLController::FlushCache() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_html_parts = nullptr;
    m_page_cache.clear();
}


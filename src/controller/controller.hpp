#pragma once
#include "response.hpp"
#include "photo.hpp"
#include "url.hpp"
#include "hash.hpp"

typedef enum : unsigned short {
    HTTP_MethodGet,
    HTTP_MethodPost,
    HTTP_MethodUnknown,
} HTTP_Method;

namespace px {
	class Controller;
	typedef shared_ptr<Controller> Controller_p;
	
    /*******************************
     
     Basic
     
     *******************************/
    
	class Controller {
	public:
        virtual Response_p call(HTTP_Method method, Hash_p params) = 0;
	};
    
    /*******************************
    
     HTML
     
     *******************************/
    
    class HTMLController : public Controller {
    public:
        virtual Response_p call(HTTP_Method method, Hash_p params) final;
        
        static void FlushCache();
        
    protected:
        virtual unsigned int body(std::ostringstream& stream, Hash_p params) = 0;
        virtual void cache_key(Hash_p params, string& out) = 0;
//        virtual void title(Hash_p params, string& out) = 0;
        
    private:
        static unordered_map<string, Response_p> m_page_cache;
        static std::mutex m_mutex;
        static shared_ptr<std::pair<string, string>> m_html_parts;
    };
}



#define ControllerInterface(__CLASS__, __SUPER__) \
namespace px { \
class __CLASS__ : public __SUPER__ { \
public: \
    static Controller_p New() { \
        return Controller_p(new __CLASS__()); \
    } \
    virtual ~__CLASS__() {}; \
    

#define ControllerInterfaceEnd(__CLASS__)\
private:\
    __CLASS__() {};\
    __CLASS__(const __CLASS__& src) = delete;\
    __CLASS__& operator=(const __CLASS__& rhs) = delete;\
};\
}\

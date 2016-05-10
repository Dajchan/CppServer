#pragma once
#include "defines.hpp"

namespace px {
    /*************************
     
     String Helper
     
     *************************/
    
    vector<string> &split(const string &s, const string &delim, vector<string> &elems);
    vector<string> split(const string &s, const string &delim);
    vector<string> &split(const string &s, char delim, vector<string> &elems);
    vector<string> split(const string &s, char delim);
    
    string strip(const string& s, const string& c);
    
    string camelcase(const string& s, bool uppercase_first=false);
    
    string upcase(const string& s);
    string upcase(const string& s, unsigned int length);
    
    string downcase(const string& s);
    string downcase(const string& s, unsigned int length);
    
    /*************************
     
     File Helper
     
     *************************/
    string first_component(const string& path);
    string base_name(const string& path);
    string file_stem(const string& path);
    string file_extension(const string& path);
    bool file_exists(const char *path, bool* is_directory);

    typedef enum {
        FileTypeFile = 1<<0,
        FileTypeDir = 1<<1,
        FileTypeAll = FileTypeDir | FileTypeFile
    } FileType;
    
    vector<string> dir_content(const string& path, FileType type=FileTypeAll);
    vector<string> dir_content(const char* path, FileType type=FileTypeAll);
    
    time_t file_modification_time(const string& path);
    time_t file_modification_time(const char* path);
    
    /*************************
     
     XML Escape Sequences
     
     *************************/
    
    string xmlescape(const string& s);
    void xmlescape(const string& s, string& out);
    
    string xmlunescape(const string& s);
    void xmlunescape(const string& s, string& out);
    
    /*************************
     
     String <> Number 
     
     *************************/
    
    unsigned long long random(unsigned long long max=ULLONG_MAX);
    
    string to_string(unsigned long src, int radix, int length=0, char padding=0);
    string to_string(unsigned long long src, int radix, int length=0, char padding=0);
    
    long hex_to_long(const string& s);
    
    time_t timestamp();
    
    long timestampms();
    
    double system_timestamp();
    
    static inline time_t timegm( struct tm *tm ) {
        time_t t = std::mktime( tm );
        struct tm _t;
        auto local = localtime_r( &t, &_t);
        return t + local->tm_gmtoff - ((local->tm_isdst ? 1 : 0)*60*60);
    }
    
    string create_uniq_id(long user_seed);
    
    string create_id(unsigned long milliseconds, unsigned long user_seed, unsigned long random_counter);
    
    string vstringfmt(const char *fmt, va_list argList);
    
    
#ifdef __printflike
    string stringfmt(const char *fmt, ...) __printflike(1, 2);
#else
    string stringfmt(const char *fmt, ...);
#endif
    
    template<typename T> bool ptr_comp(std::shared_ptr<T> a, std::shared_ptr<T> b) {
        return *a < *b;
    }
    
    template<typename T> inline bool nil_chk(const T& obj) {
        return obj != 0;
    }
    
    template<typename T> inline bool nil_chk(const std::shared_ptr<T>& obj) {
        return obj != nullptr;
    }
    
    template<> inline bool nil_chk<string>(const string& obj) {
        return !obj.empty();
    }
    
    template<typename T> bool includes(const T& elements, const function<bool(const typename T::value_type& e)>& block) {
        for (const auto& e : elements) {
            if (block(e)) {
                return true;
            }
        }
        return false;
    }
    
    template<typename T> bool includes(const T& elements, const typename T::value_type& element) {
        return includes<T>(elements, [&element](const typename T::value_type& e) -> bool {
            return (bool)(element == e);
        });
    }
    
    template<typename Ta, typename Tb> inline std::vector<Tb> collect(const std::vector<Ta>& elements, const function<Tb(const Ta& element)>& block, bool skip_nil=true) {
        std::vector<Tb> vec;
        for (auto& e : elements) {
            auto r = block(e);
            if (!skip_nil || nil_chk(r)) {
                vec.push_back(r);
            }
        }
        return vec;
    }
    
    template<typename Tvalue, typename Tkey> std::unordered_map<Tkey, std::vector<Tvalue>> group_by(const std::vector<Tvalue>& elements, const std::function<Tkey(const Tvalue& element)>& block) {
        std::unordered_map<Tkey, std::vector<Tvalue>> map;
        for (auto& e : elements) {
            Tkey k = block(e);
            auto& vec = map[k];
            vec.push_back(e);
        }
        return map;
    }
    
    template<typename Tkey, class InputIterator> std::unordered_map<Tkey, const typename std::iterator_traits<InputIterator>::value_type> index_by(InputIterator first, InputIterator last, const std::function<Tkey(const typename std::iterator_traits<InputIterator>::value_type& e)>& block) {
        std::unordered_map<Tkey, typename std::iterator_traits<InputIterator>::value_type> map;
        while (first != last) {
            auto k = block(*first);
            if (nil_chk(k)) {
                map[k] = *first;
            }
            first++;
        }
        return map;
    }
    
    template<typename Tvalue, typename Tkey> std::unordered_map<Tkey, Tvalue> index_by(const std::vector<Tvalue>& elements, const std::function<Tkey(const Tvalue& element)>& block) {
        std::unordered_map<Tkey, Tvalue> map;
        for (auto& e : elements) {
            Tkey k = block(e);
            map[k] = e;
        }
        return map;
    }
    
    template<class InputIterator> std::string join(InputIterator first, InputIterator last, const std::string& delim, const function<std::string(const typename std::iterator_traits<InputIterator>::value_type& e)>& block) {
        std::string str;
        int i = 0;
        while (first != last) {
            if (i != 0) {
                str += delim;
            }
            str += block(*first);
            ++first;
            ++i;
        }
        return str;
    }
    
    template<typename T> std::string join(const std::vector<T>& elements, const std::string& delim, const function<std::string(const T& e)>& block) {
        return join(elements.begin(), elements.end(), delim, block);
    }
    
    std::string join(const std::vector<std::string>& elements, const std::string& delim);
    
    template<typename Ta, typename Tb> std::vector<Tb> convert(const std::vector<Ta>& elements) {
        std::vector<Tb> vec;
        for (auto& e : elements) {
            Tb a = e;
            vec.push_back(a);
        }
        return vec;
    }
    
    template<typename T, typename F> void reject(T& elements, const F& block) {
        for (size_t i=0; i<elements.size();) {
            if (block(elements[i])) {
                elements.erase(elements.begin()+i);
            } else {
                ++i;
            }
        }
    }
    
    template<typename T> std::shared_ptr<T> find(const std::vector<std::shared_ptr<T>>& elements, const function<bool(const std::shared_ptr<T>&)>& block) {
        for (auto& e : elements) {
            if (block(e)) {
                return e;
            }
        }
        return nullptr;
    }
    
    template<class InputIterator, class Tkey> std::set<Tkey> map_uniq(InputIterator first, InputIterator last, const function<Tkey(const typename std::iterator_traits<InputIterator>::value_type& e)>& block) {
        std::set<Tkey> set;
        while (first != last) {
            Tkey k = block(*first);
            set.insert(k);
            first++;
        }
        return set;
    }
    
    template<class Tkey, class T, class F> std::set<Tkey> map_uniq(const T& elements, const F& block) {
        std::set<Tkey> set;
        auto itr = elements.begin();
        auto last = elements.end();
        while (itr != last) {
            Tkey k = block(*itr);
            set.insert(k);
            itr++;
        }
        return set;
    }
    
}
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <sstream>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <condition_variable>

namespace std_patch {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
    
    // for some reason android doesn't ship with to_string
    // implement it here
    template<typename T>
    std::string to_string( const T& n ) {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
using std::string;

using std::string;
using std::vector;
using std::unordered_map;
using std::map;
using std::function;
using std::shared_ptr;


namespace px {
    class Array;
    typedef std::shared_ptr<px::Array> Array_p;
    
    class Hash;
    typedef std::shared_ptr<px::Hash> Hash_p;
    
    class Value;
    typedef std::shared_ptr<px::Value> Value_p;
    
    class Error;
    typedef std::shared_ptr<px::Error> Error_p;

    namespace String {
        const string& Empty();
    }
}


#define XSTRINGIFY(s) STRINGIFY(s)
#define STRINGIFY(s) #s


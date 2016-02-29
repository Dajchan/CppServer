#pragma once
#include "value.hpp"

namespace px {
    class Hash {
    public:
        static Hash_p Create();
        static Hash_p Create(std::initializer_list<std::pair<string, Value_p>>);
        static Hash_p Copy(Hash_p src);
        
        unordered_map<string, Value_p>::iterator          begin() noexcept;
        unordered_map<string, Value_p>::const_iterator    begin() const noexcept;
        unordered_map<string, Value_p>::iterator          end() noexcept;
        unordered_map<string, Value_p>::const_iterator    end() const noexcept;
        
        size_t size() const;
        size_t count(const function<bool(const string&, const Value_p&)>& block) const;
        
        bool eql(const Hash& other) const;
        
        Value_p& operator[] (const string& k);
        
        void set(const string&, Value_p);
        Value_p get(const string&);
        
        void each_pair(const function<void(const string&, const Value_p&)>& block) const;
        
        void each_value(const function<void(const Value_p&)>& block);
        
        void each_key(const function<void(const string&)>& block);
        
        Array_p collect(const function<Value_p(const string&, const Value_p&)>& block) const;
        
        void delete_if(const function<bool(const string&, const Value_p&)>& block);
        
        void remove(const string&);
        
        Hash_p copy();
        
        Hash_p merge(Hash_p);
        
        void append(Hash_p);
        
        string to_query();
        
    private:
        unordered_map<string, Value_p> m_map;
        
        Hash();
        Hash(unordered_map<string, Value_p>& map);
        Hash(std::initializer_list<std::pair<string, Value_p>>);
        Hash(const Hash& src);
        Hash& operator=(const Hash& rhs);
    };
}
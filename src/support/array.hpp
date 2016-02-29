#pragma once
#include "value.hpp"

namespace px {
    class Array {
    public:
        static Array_p Create();
        static Array_p Create(std::initializer_list<Value_p>);
        static Array_p Copy(Array_p src);
        
        template<class T> static Array_p Convert(const vector<T> vec) {
            Array_p ret = Create();
            for (auto& e : vec) {
                ret->add(Value::Create(e));
            }
            return ret;
        }
        
        vector<Value_p>::iterator          begin() noexcept;
        vector<Value_p>::const_iterator    begin() const noexcept;
        vector<Value_p>::iterator          end() noexcept;
        vector<Value_p>::const_iterator    end() const noexcept;
        
        size_t size() const;
        size_t count(const function<bool(const Value_p&)>& block) const;
        bool eql(const Array& other) const;
        
        void add(Value_p);
        template<class T> void add(T item) {
            add(Value::Create(item));
        }
        
        void set(Value_p item, size_t index);
        template<class T> void set(T item, size_t index) {
            set(Value::Create(item), index);
        }
        
        void insert(Value_p item, size_t index);
        template<class T> void insert(T item, size_t index) {
            insert(Value::Create(item), index);
        }
        
        Value_p remove(size_t index);
        
        Value_p get(size_t index) const;
        
        Value_p first() const;
        
        Value_p last() const;
        
        void each(const function<void(const Value_p&)>& block) const;
        
        void each(const function<void(const Value_p&, const size_t)>& block) const;
        
        Array_p collect(const function<Value_p(const Value_p&)>& block) const;
        
        Array_p collect(const function<Value_p(const Value_p&, const size_t)>& block) const;
        
        Value_p find(const function<bool(const Value_p&)>& block) const;
        
        Array_p select(const function<bool(const Value_p&)>& block) const;
        
        Array_p sort(const function<bool(const Value_p&, const Value_p&)>& block) const;
        
        void sort_inplace(const function<bool(const Value_p&, const Value_p&)>& block);
        
        string join(const string& delim) const;
        string join(const string& delim, const function<string(const Value_p&)>& block) const;
        
        Hash_p index_by(const function<string(const Value_p&)>& block) const;
        
        Hash_p group_by(const function<string(const Value_p&)>& block) const;
        
        Array_p copy() const;
        
        void delete_if(const function<bool(const Value_p&)>& block);
        
        size_t index_of(const Value_p&) const;
        size_t index_of(const function<bool(const Value_p&)>& block);
        
    private:
        vector<Value_p> m_vector;
        
        Array();
        Array(size_t n);
        Array(vector<Value_p>& vec);
        Array(std::initializer_list<Value_p>);
        Array(const Array& src);
        Array& operator=(const Array& rhs);
    };
}

#pragma once
#include "defines.hpp"
#include "date.hpp"

namespace px {
    typedef enum : unsigned int {
        NullType,
        BooleanType,
        LongType,
        DoubleType,
        StringType,
        DateType,
        ArrayType,
        HashType,
        AnyType
    } ValueType;
    
    class Value {
    public:
        ~Value();
        static Value_p Create(bool);
        static Value_p Create(int);
        static Value_p Create(long);
        static Value_p Create(double);
        static Value_p Create(const char *chars);
        static Value_p Create(const string&);
        static Value_p Create(const Date&);
        static Value_p Create(const Array_p&);
        static Value_p Create(const Hash_p&);
        
        operator bool() const;
        operator long() const;
        operator double() const;
        operator string*() const;
        operator Date*() const;
        operator Array_p();
        operator Hash_p();
        
        const bool& bool_value() const;
        const long& long_value() const;
        const double& double_value() const;
        const string& string_value() const;
        const Date& date_value() const;
        Array_p array_value();
        Hash_p hash_value();
        
        bool is_null() const;
        bool is_bool() const;
        bool is_long() const;
        bool is_double() const;
        bool is_string() const;
        bool is_date() const;
        bool is_array() const;
        bool is_hash() const;
        
        ValueType type() const;
        
        string description() const;
        
        Value_p as_bool() const;
        Value_p as_long() const;
        Value_p as_double() const;
        Value_p as_string() const;
        Value_p as_date() const;
        
        Value_p convert(ValueType dest) const;
        
        Value_p copy() const;
        
        bool eql(const Value& other) const;
        bool operator==(const Value& other) const;
        
    private:
        Value(void) = delete;
        Value(const Value& src) = delete;
        Value& operator=(const Value& rhs) = delete;
        
        Value(bool);
        Value(long);
        Value(double);
        Value(const char *chars);
        Value(const string&);
        Value(const Date&);
        Value(const Array_p&);
        Value(const Hash_p&);
        
        ValueType m_type = NullType;
        union {
            long m_long = 0;
            bool m_bool;
            double m_double;
            string* m_string;
            Date* m_date;
            Array_p m_array;
            Hash_p m_hash;
        };
    };
}

bool operator==(const px::Value_p& lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const bool& rhs);
bool operator==(const bool& lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const long& rhs);
bool operator==(const long& lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const int& rhs);
bool operator==(const int& lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const double& rhs);
bool operator==(const double& lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const char* rhs);
bool operator==(const char* lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const string& rhs);
bool operator==(const string& lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const px::Date& rhs);
bool operator==(const px::Date& lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const px::Array_p& rhs);
bool operator==(const px::Array_p& lhs, const px::Value_p& rhs);

bool operator==(const px::Value_p& lhs, const px::Hash_p& rhs);
bool operator==(const px::Hash_p& lhs, const px::Value_p& rhs);



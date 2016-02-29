#include "value.hpp"
#include <iomanip>
#include "helper.hpp"
#include "array.hpp"
#include "hash.hpp"

using namespace px;

/*****************************
 
	Value
 
 ******************************/
Value::Value(bool v) : m_type(BooleanType), m_bool(v) {};
Value::Value(long v) : m_type(LongType), m_long(v) {};
Value::Value(double v) : m_type(DoubleType), m_double(v) {};
Value::Value(const char *v) : m_type(StringType), m_string(new std::string(v)) {};
Value::Value(const string& v) : m_type(StringType), m_string(new std::string(v)) {};
Value::Value(const Date& v) : m_type(DateType), m_date(new Date(v)) {};
Value::Value(const Array_p& v) : m_type(ArrayType), m_array(v) {};
Value::Value(const Hash_p& v) : m_type(HashType), m_hash(v) {};

Value::~Value() {
    if (m_type == StringType && m_string) {
        delete m_string;
    } else if (m_type == DateType && m_date) {
        delete m_date;
    }
}

Value_p Value::Create(bool v) {return Value_p(new Value(v));}
Value_p Value::Create(int v) {return Value_p(new Value((long)v));}
Value_p Value::Create(long v) {return Value_p(new Value(v));}
Value_p Value::Create(double v) {return Value_p(new Value(v));}
Value_p Value::Create(const char *v) {return Value_p(new Value(v));}
Value_p Value::Create(const string& v) {return Value_p(new Value(v));}
Value_p Value::Create(const Date& v) {return Value_p(new Value(v));}
Value_p Value::Create(const Array_p& v) {return Value_p(new Value(v));}
Value_p Value::Create(const Hash_p& v) {return Value_p(new Value(v));}

Value::operator bool() const {return bool_value();}
Value::operator long() const {return long_value();}
Value::operator double() const {return double_value();}
Value::operator string*() const {
    if(type() == StringType) {
        return m_string;
    }
    return nullptr;
}
Value::operator Date*() const {
    if(type() == DateType) {
        return m_date;
    }
    return nullptr;
}
Value::operator Array_p() {return array_value();}
Value::operator Hash_p() {return hash_value();}

const bool& Value::bool_value() const {
    if(type() == BooleanType) {
        return m_bool;
    }
    static bool ret=false;
    return ret;
}

const long& Value::long_value() const {
    if (type() == LongType) {
        return m_long;
    }
    static long ret=0;
    return ret;
}

const double& Value::double_value() const {
    if(type() == DoubleType) {
        return m_double;
    }
    static double ret=0;
    return ret;
}

const string& Value::string_value() const {
    if(type() == StringType && m_string) {
        return *m_string;
    }
    return px::String::Empty();
}

const Date& Value::date_value() const {
    if(type() == DateType && m_date) {
        return *m_date;
    }
    return px::Date::Empty();
}

Array_p Value::array_value() {
    if(type() == ArrayType) {
        return m_array;
    }
    return nullptr;
}

Hash_p Value::hash_value(){
    if(type() == HashType) {
        return m_hash;
    }
    return nullptr;
}

bool Value::is_null() const {return type() == NullType;};
bool Value::is_bool() const {return type() == BooleanType;};
bool Value::is_long() const {return type() == LongType;};
bool Value::is_double() const {return type() == DoubleType;};
bool Value::is_string() const {return type() == StringType;};
bool Value::is_date() const {return type() == DateType;};
bool Value::is_array() const {return type() == ArrayType;};
bool Value::is_hash() const {return type() == HashType;};

ValueType Value::type() const {
    if (this == nullptr){
        return NullType;
    }
    return m_type;
}

string Value::description() const {
    std::ostringstream stm;
    if (this == nullptr) {
        stm << "null";
    } else {
        switch (m_type) {
            case BooleanType:
                if (m_bool) {
                    stm << "true";
                } else {
                    stm << "false";
                }
                break;
            case LongType:
                stm << m_long;
                break;
            case DoubleType:
                stm << m_double;
                break;
            case StringType:
                stm << "\"" << *m_string << "\"" ;
                break;
            case DateType: {
                Value_p v = as_string();
                std::string * s = *v;
                if (s) {
                    stm << *s;
                }
                break;
            }
            case ArrayType:
                stm << "[";
                for (auto& v : *m_array) {
                    stm << v->description();
                }
                stm << "]";
                break;
            case HashType:
                stm << "{";
                for (auto& v : *m_hash) {
                    stm << v.first << ": " << v.second->description();
                }
                stm << "}";
                
                break;
            default:
                break;
        }
    }
    return stm.str();
}

Value_p Value::as_bool() const{
    switch (type()) {
        case NullType:
            return Value::Create(false);
        case BooleanType:
            return Value::Create(m_bool);
        case LongType:
            return Value::Create(m_long != 0);
        case DoubleType:
            return Value::Create(m_double != 0);
        case StringType:
            return Value::Create((m_string && !m_string->empty() && *m_string == "true"));
        default:
            return Value::Create(true);
            break;
    }
    return nullptr;
}

Value_p Value::as_long() const {
    switch (type()) {
        case NullType:
            return Value::Create((long)0);
        case BooleanType:
            return Value::Create((long)(m_bool ? 1 : 0));
        case LongType:
            return Value::Create(m_long);
        case DoubleType:
            return Value::Create((long)m_double);
        case StringType:
            if (m_string && !m_string->empty()) {
                long value = atol(m_string->c_str());
                return Value::Create(value);
            }
            break;
        case DateType:
            return Value::Create((long)m_date);
        default:
            break;
    }
    return nullptr;
}

Value_p Value::as_double() const {
    switch (type()) {
        case NullType:
            return Value::Create((double)0);
        case BooleanType:
            return Value::Create((double)(m_bool ? 1 : 0));
        case LongType:
            return Value::Create((double)m_long);
        case DoubleType:
            return Value::Create(m_double);
        case StringType:
            if (m_string && !m_string->empty()) {
                double value = atof(m_string->c_str());
                return Value::Create(value);
            }
            break;
        case DateType:
            return Value::Create((double)((long)m_date));
        default:
            break;
    }
    return nullptr;
}

Value_p Value::as_string() const {
    switch (type()) {
        case BooleanType:
            return Value::Create((m_bool ? "true" : "false"));
        case LongType:
            return Value::Create(px::stringfmt("%ld", m_long));
        case DoubleType:
            return Value::Create(px::stringfmt("%f", m_double));
        case StringType:
            return Value::Create(*m_string);
        case DateType:
            return Value::Create(m_date->to_string());
        default:
            break;
    }
    return nullptr;
}

Value_p Value::as_date() const{
    switch (type()) {
        case LongType:
            return Value::Create(Date(m_long));
        case DoubleType:
            return Value::Create(Date(m_double));
        case StringType: {
            std::tm t = {};
            std::istringstream ss(string_value());
            ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
            return Value::Create(Date(t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec));
        }
        case DateType:
            return Value::Create(m_date);
        default:
            break;
    }
    return nullptr;
}

Value_p Value::convert(ValueType dest) const {
    switch (dest) {
        case BooleanType:
            return as_bool();
        case LongType:
            return as_long();
        case DoubleType:
            return as_double();
        case StringType:
            return as_string();
        case DateType:
            return as_date();
        default:
            break;
    }
    return nullptr;
}

Value_p Value::copy() const {
    switch (type()) {
        case BooleanType:
            return as_bool();
        case LongType:
            return as_long();
        case DoubleType:
            return as_double();
        case StringType:
            return as_string();
        case DateType:
            return as_date();
        case ArrayType:
            return Value::Create(m_array->copy());
        case HashType:
            return Value::Create(m_hash->copy());
        default:
            break;
    }
    return nullptr;
}

bool Value::eql(const Value& other) const{
    if (type() == other.type()) {
        switch (type()) {
            case BooleanType:
                return m_bool == other.m_bool;
            case LongType:
                return m_long == other.m_long;
            case DoubleType:
                return m_double == other.m_double;
            case StringType:
                return *m_string == *other.m_string;
            case DateType:
                return *m_date == *other.m_date;
            case ArrayType:
                return m_array->eql(*other.m_array);
            case HashType:
                return m_hash->eql(*other.m_hash);
            default:
                return true;
        }
    }
    return false;
}

bool Value::operator==(const Value& other) const {
    return eql(other);
}




/*****************************
 
	Compare
 
 ******************************/

bool operator==(const px::Value_p& lhs, const px::Value_p& rhs) {
    return lhs->eql(*rhs.get());
}

bool operator==(const px::Value_p& lhs, const bool& rhs) {
    if (lhs->type() == px::BooleanType) {
        return rhs == (bool)*lhs;
    }
    return false;
}
bool operator==(const bool& lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::BooleanType) {
        return lhs == (bool)*rhs;
    }
    return false;
}

bool operator==(const px::Value_p& lhs, const long& rhs) {
    if (lhs->type() == px::LongType) {
        return rhs == (long)*lhs;
    }
    return false;
}
bool operator==(const long& lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::LongType) {
        return lhs == (long)*rhs;
    }
    return false;
}

bool operator==(const px::Value_p& lhs, const int& rhs) {
    if (lhs->type() == px::LongType) {
        return (long)rhs == (long)*lhs;
    }
    return false;
}
bool operator==(const int& lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::LongType) {
        return (long)lhs == (long)*rhs;
    }
    return false;
}

bool operator==(const px::Value_p& lhs, const double& rhs) {
    if (lhs->type() == px::DoubleType) {
        return rhs == (double)*lhs;
    }
    return false;
}
bool operator==(const double& lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::DoubleType) {
        return lhs == (double)*rhs;
    }
    return false;
}

bool operator==(const px::Value_p& lhs, const char* rhs) {
    if (lhs->type() == px::StringType) {
        return lhs->string_value() == rhs;
    }
    return false;
}

bool operator==(const char* lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::StringType) {
        return rhs->string_value() == lhs;
    }
    return false;
}

bool operator==(const px::Value_p& lhs, const string& rhs) {
    if (lhs->type() == px::StringType) {
        return lhs->string_value() == rhs;
    }
    return false;
}
bool operator==(const string& lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::StringType) {
        return rhs->string_value() == lhs;
    }
    return false;
}

bool operator==(const px::Value_p& lhs, const px::Date& rhs) {
    if (lhs->type() == px::DateType) {
        return lhs->date_value() == rhs;
    }
    return false;
}
bool operator==(const px::Date& lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::DateType) {
        return rhs->date_value() == lhs;
    }
    return false;
}

bool operator==(const px::Value_p& lhs, const px::Array_p& rhs) {
    if (lhs->type() == px::ArrayType) {
        px::Array_p ary = lhs->array_value();
        if (ary) {
            return ary->eql(*rhs.get());
        }
    }
    return false;
}

bool operator==(const px::Array_p& lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::ArrayType) {
        px::Array_p ary = rhs->array_value();
        if (ary) {
            return ary->eql(*lhs.get());
        }
    }
    return false;
}

bool operator==(const px::Value_p& lhs, const px::Hash_p& rhs) {
    if (lhs->type() == px::HashType) {
        px::Hash_p h = lhs->hash_value();
        if (h) {
            return h->eql(*rhs.get());
        }
    }
    return false;
}

bool operator==(const px::Hash_p& lhs, const px::Value_p& rhs) {
    if (rhs->type() == px::HashType) {
        px::Hash_p h = rhs->hash_value();
        if (h) {
            return h->eql(*lhs.get());
        }
    }
    return false;
}








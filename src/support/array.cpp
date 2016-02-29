//
//  array.cpp
//  px
//
//  Created by Jonathan Cichon on 03.01.16.
//
//

#include "array.hpp"
#include "hash.hpp"
#include "helper.hpp"

using namespace px;


Array::Array() {}
Array::Array(size_t n) : m_vector(std::vector<Value_p>(n)) {}
Array::Array(std::vector<Value_p>& vec) : m_vector(vec){}
Array::Array(std::initializer_list<Value_p> list) : m_vector(list) {}
Array::Array(const Array& src) : m_vector(src.m_vector) {}

Array_p Array::Create() {return Array_p(new Array());}
Array_p Array::Create(std::initializer_list<Value_p> list) {return Array_p(new Array(list));}
Array_p Array::Copy(Array_p src) {return Array_p(new Array(*src.get()));}

std::vector<Value_p>::iterator          Array::begin() noexcept {return m_vector.begin();}
std::vector<Value_p>::const_iterator    Array::begin() const noexcept {return m_vector.begin();}
std::vector<Value_p>::iterator          Array::end() noexcept {return m_vector.end();}
std::vector<Value_p>::const_iterator    Array::end() const noexcept {return m_vector.end();}

size_t Array::size() const {return m_vector.size();}

size_t Array::count(const function<bool(const Value_p&)>& block) const {
    size_t c = 0;
    for (auto& item : m_vector) {
        if (block(item)) {
            c++;
        }
    }
    return c;
}

bool Array::eql(const Array& other) const {
    if (size() == other.size()) {
        for (size_t i = 0; i<size(); ++i) {
            if(!get(i)->eql(*other.get(i))) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void Array::add(Value_p item) {
    if (item) {
        m_vector.push_back(item);
    }
}

void Array::set(Value_p item, size_t index) {
    m_vector[index] = item;
}

void Array::insert(Value_p item, size_t index) {
    m_vector.insert(m_vector.begin() + index, item);
}

Value_p Array::remove(size_t index) {
    Value_p item = nullptr;
    if (index<m_vector.size()) {
        item = m_vector[index];
        m_vector.erase(m_vector.begin() + index);
    }
    return item;
}

Value_p Array::get(size_t index) const {
    Value_p item = nullptr;
    if (index<m_vector.size()) {
        item = m_vector[index];
    }
    return item;
}

Value_p Array::first() const {
    Value_p item = nullptr;
    if (m_vector.size() > 0) {
        item = m_vector[0];
    }
    return item;
}

Value_p Array::last() const {
    Value_p item = nullptr;
    if (m_vector.size() > 0) {
        item = m_vector[m_vector.size()-1];
    }
    return item;
}

void Array::each(const function<void(const Value_p&)>& block) const {
    for (auto& item : m_vector) {
        block(item);
    }
}

void Array::each(const function<void(const Value_p&, const size_t)>& block) const {
    size_t index=0;
    for (auto& item : m_vector) {
        block(item, index);
        ++index;
    }
}

Array_p Array::collect(const function<Value_p(const Value_p&)>& block) const {
    Array_p array = Array::Create();
    for (auto& item : m_vector) {
        auto val = block(item);
        array->add(val);
    }
    return array;
}

Array_p Array::collect(const function<Value_p(const Value_p&, const size_t)>& block) const {
    size_t index=0;
    Array_p array = Array::Create();
    for (auto& item : m_vector) {
        auto val = block(item, index);
        array->add(val);
        ++index;
    }
    return array;
}

Value_p Array::find(const function<bool(const Value_p&)>& block) const {
    for (auto& item : m_vector) {
        if (block(item)) {
            return item;
        }
    }
    return nullptr;
}

Array_p Array::select(const function<bool(const Value_p&)>& block) const {
    Array_p ret = Create();
    for (auto& item : m_vector) {
        if (block(item)) {
            ret->add(item);
        }
    }
    return ret;
}

Array_p Array::sort(const function<bool(const Value_p&, const Value_p&)>& block) const {
    auto ret = Array_p(new Array(m_vector.size()));
    std::partial_sort_copy(m_vector.begin(), m_vector.end(), ret->m_vector.begin(), ret->m_vector.end(), block);
    return ret;
}

void Array::sort_inplace(const function<bool(const Value_p&, const Value_p&)>& block) {
    std::sort(m_vector.begin(), m_vector.end(), block);
}

string Array::join(const string& delim) const {
    return join(delim, [&](const Value_p& e) {
        Value_p s = e->convert(StringType);
        return s->string_value();
    });
}

string Array::join(const string& delim, const function<string(const Value_p&)>& block) const {
    return px::join(m_vector, delim, block);
}

Hash_p Array::index_by(const function<string(const Value_p&)>& block) const {
    Hash_p hash = Hash::Create();
    for (auto& item : m_vector) {
        hash->set(block(item), item);
    }
    return hash;
}

Hash_p Array::group_by(const function<std::string(const Value_p&)>& block) const {
    Hash_p hash = Hash::Create();
    for (auto& item : m_vector) {
        auto k = block(item);
        Array_p array = nullptr;
        auto a = hash->get(k);
        if(!a || a->type() != ArrayType) {
            array = Array::Create();
            hash->set(k, Value::Create(array));
        } else {
            array = a->array_value();
        }
        array->add(item);
    }
    return hash;
}

Array_p Array::copy() const {
    Array_p array = Array::Create();
    for (auto& item : m_vector) {
        array->add(item->copy());
    }
    return array;
}

void Array::delete_if(const function<bool(const Value_p&)>& block) {
    for (int i = 0; i<m_vector.size();) {
        Value_p v = m_vector[i];
        if (block(v)) {
            m_vector.erase(m_vector.begin()+i);
        } else {
            i++;
        }
    }
}

size_t Array::index_of(const Value_p& value) const {
    size_t index = 0;
    if (value) {
        for (auto& item : m_vector) {
            if(value->eql(*item.get())) {
                return index;
            }
            ++index;
        }
    }
    return -1;
}

size_t Array::index_of(const function<bool(const Value_p&)>& block) {
    size_t index = 0;
    for (auto& item : m_vector) {
        if(block(item)){
            return index;
        }
        ++index;
    }
    return -1;
}
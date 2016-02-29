#include "hash.hpp"
#include "helper.hpp"
#include "array.hpp"

using namespace px;

Hash::Hash() {}
Hash::Hash(std::unordered_map<std::string, Value_p>& map) : m_map(map) {};
Hash::Hash(std::initializer_list<std::pair<std::string, Value_p>> list) {
    for (auto& pair : list) {
        m_map[pair.first] = pair.second;
    }
}
Hash::Hash(const Hash& src) : m_map(src.m_map) {};

Hash_p Hash::Create() {return Hash_p(new Hash());}
Hash_p Hash::Create(std::initializer_list<std::pair<string, Value_p>> list) {return Hash_p(new Hash(list));}
Hash_p Hash::Copy(Hash_p src) {return Hash_p(new Hash(*src.get()));}

std::unordered_map<string, Value_p>::iterator          Hash::begin() noexcept {return m_map.begin();}
std::unordered_map<string, Value_p>::const_iterator    Hash::begin() const noexcept {return m_map.begin();}
std::unordered_map<string, Value_p>::iterator          Hash::end() noexcept {return m_map.end();}
std::unordered_map<string, Value_p>::const_iterator    Hash::end() const noexcept {return m_map.end();}

size_t Hash::size() const {
    return m_map.size();
}

size_t Hash::count(const function<bool(const string&, const Value_p&)>& block) const {
    size_t c = 0;
    for (auto& item : m_map) {
        if (block(item.first, item.second)) {
            c++;
        }
    }
    return c;
}

bool Hash::eql(const Hash& other) const {
    for (auto& pair : m_map) {
        auto itr = other.m_map.find(pair.first);
        if (itr == m_map.end() || itr->second != pair.second) {
            return false;
        }
    }
    return true;
}

Value_p& Hash::operator[] (const string& k) {
    return m_map[k];
}

void Hash::set(const string& k, Value_p v) {
    if(v) {
        m_map[k] = v;
    } else {
        m_map.erase(k);
    }
}

Value_p Hash::get(const string& k) {
    auto itr = m_map.find(k);
    if (itr != m_map.end()) {
        return itr->second;
    }
    return nullptr;
}

void Hash::each_pair(const function<void(const string&, const Value_p&)>& block) const {
    for (auto& pair : m_map) {
        block(pair.first, pair.second);
    }
}

void Hash::each_value(const function<void(const Value_p&)>& block) {
    for (auto& pair : m_map) {
        block(pair.second);
    }
}

void Hash::each_key(const function<void(const string&)>& block) {
    for (auto& pair : m_map) {
        block(pair.first);
    }
}

Array_p Hash::collect(const function<Value_p(const string&, const Value_p&)>& block) const {
    Array_p array = Array::Create();
    for (auto& pair : m_map) {
        auto v = block(pair.first, pair.second);
        if (v) {
            array->add(v);
        }
    }
    return array;
}

void Hash::delete_if(const function<bool(const string&, const Value_p&)>& block) {
    for (auto& pair : m_map) {
        if (block(pair.first, pair.second)) {
            m_map.erase(pair.first);
        }
    }
}

void Hash::remove(const string& k) {
    m_map.erase(k);
}

Hash_p Hash::copy() {
    Hash_p hash = Hash::Create();
    for (auto& item : m_map) {
        hash->set(item.first, item.second->copy());
    }
    return hash;
}

Hash_p Hash::merge(Hash_p other) {
    Hash_p cp = this->copy();
    cp->append(other);
    return cp;
}

void Hash::append(Hash_p other) {
    if (other) {
        for (auto& item : other->m_map) {
            m_map[item.first] = item.second;
        }
    }
}

string Hash::to_query() {
    string query_str;
    for (auto& pair : m_map) {
        auto v = pair.second->convert(StringType);
        if (v) {
            string str = v->string_value();
            if (query_str.empty()) {
                query_str += "?";
            } else {
                query_str += "&";
            }
            query_str += pair.first + "=" + str;
        }
    }
    return query_str;
}

//
//  blog_entry.cpp
//  px
//
//  Created by Jonathan Cichon on 04.01.16.
//
//

#include "blog_entry.hpp"
#include "logger.hpp"

using namespace px;

BlogEntry_p BlogEntry::New(const string& title, const string& identifier) {
    LogDebug("BlogEntry: %s %s", title.c_str(), identifier.c_str());
    return BlogEntry_p(new BlogEntry(title, identifier));
}

BlogEntry::BlogEntry(const string& title, const string& identifier) : m_title(title), m_identifier(identifier) {}

BlogEntry::~BlogEntry() {}

void BlogEntry::append(Photo_p photo) {
    if (photo) {
        m_photos.push_back(photo);
        m_photos_map[photo->identifier()] = photo;
    }
}

Photo_p BlogEntry::get(const string& identifier) const {
    auto itr = m_photos_map.find(identifier);
    if (itr != m_photos_map.end()) {
        return itr->second;
    }
    return nullptr;
}

const vector<Photo_p>& BlogEntry::photos() const {
    return m_photos;
}

const string& BlogEntry::title() const {
    return m_title;
}

const string& BlogEntry::identifier() const {
    return m_identifier;
}
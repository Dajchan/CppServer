//
//  blog_entry.cpp
//  px
//
//  Created by Jonathan Cichon on 04.01.16.
//
//

#include "blog_entry.hpp"
#include "logger.hpp"
#include "mime_type.hpp"

using namespace px;

BlogEntry_p BlogEntry::New(const string& title, const string& identifier, Resource_p description) {
    LogDebug("BlogEntry: %s %s", title.c_str(), identifier.c_str());
    return BlogEntry_p(new BlogEntry(title, identifier, description));
}

BlogEntry::BlogEntry(const string& title, const string& identifier, Resource_p description) : m_title(title), m_identifier(identifier), m_description(description) {}

BlogEntry::~BlogEntry() {}

void BlogEntry::append(Resource_p resource) {
    if (resource) {
        if (auto photo = std::dynamic_pointer_cast<Photo>(resource)) {
            m_photos.push_back(photo);
            m_photos_map[photo->identifier()] = photo;
        }
        m_resources.push_back(resource);
    }
}

Photo_p BlogEntry::get(const string& identifier) const {
    auto itr = m_photos_map.find(identifier);
    if (itr != m_photos_map.end()) {
        return itr->second;
    }
    return nullptr;
}

const vector<Resource_p>& BlogEntry::resources() const {
    return m_resources;
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

const Resource_p BlogEntry::description() const {
    return m_description;
}
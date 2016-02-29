#include "photo.hpp"
#include "logger.hpp"

using namespace px;

Photo_p Photo::New(const string& title, const string& identifier, const string& path) {
    LogDebug("Photo: %s %s %s", title.c_str(), identifier.c_str(), path.c_str());
    return Photo_p(new Photo(title, identifier, path));
}

Photo::Photo(const string& title, const string& identifier, const string& path) : Resource(identifier, path),   m_title(title) {}

Photo::~Photo() {}

const string& Photo::title() const {
    return m_title;
}



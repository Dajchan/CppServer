#pragma once
#include "defines.hpp"
#include "blog_entry.hpp"

namespace px {

    void render_list(std::ostringstream& stream, const vector<BlogEntry_p>& entries);
    
    void render_empty(std::ostringstream& stream, const string& empty_message);
    
}
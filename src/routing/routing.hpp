#pragma once
#include "defines.hpp"
#include "controller.hpp"

namespace px {
    struct routing_entry {
        const char * action;
        const char * mime_type;
        const char * method;
        Controller_p controller;
    };
       
    class RoutingTable {
    public:
        static RoutingTable& Instance();
        ~RoutingTable();
        
        const routing_entry * get(const string&) const;
        
    private:
        static std::unique_ptr<RoutingTable> m_instance;
        static std::once_flag m_onceFlag;
        
        unordered_map<string, const routing_entry*> m_entries;
        size_t m_max_path_length;
        
        RoutingTable(void);
        RoutingTable(const RoutingTable& src) = delete;
        RoutingTable& operator=(const RoutingTable& rhs) = delete;
    };
}

#include "routing.hpp"
#include "logger.hpp"
#include "static_controller.hpp"
#include "list_controller.hpp"
#include "detail_controller.hpp"
#include "home_controller.hpp"
#include "image_controller.hpp"
#include "resource_controller.hpp"
#include "search_controller.hpp"

using namespace px;

namespace px {
    namespace RoutingEntry {
        static const routing_entry Home = { "/home", "text/html", "GET", HomeController::New() };
        static const routing_entry Impressum = { "/impressum", "text/html", "GET", StaticController::New() };
        static const routing_entry About = { "/about", "text/html", "GET", Impressum.controller };
        static const routing_entry Contact = { "/contact", "text/html", "GET", Impressum.controller };
        static const routing_entry List = { "/blog", "text/html", "GET", ListController::New() };
        static const routing_entry Show = { "/show", "text/html", "GET", DetailController::New() };
        static const routing_entry Search = { "/search", "text/html", "GET", SearchController::New() };
        static const routing_entry Images = { "/image", "image/jpg", "GET", ImageController::New() };
        static const routing_entry Assets = { "/assets", "image/jpg", "GET", ResourceController::New() };
        static const routing_entry CSS = { "/css", "text/css", "GET", Assets.controller };
        
    }
}

std::unique_ptr<RoutingTable> RoutingTable::m_instance;
std::once_flag RoutingTable::m_onceFlag;

RoutingTable& RoutingTable::Instance() {
    std::call_once(m_onceFlag, [&] {
        m_instance.reset(new RoutingTable);
    });
    return *m_instance.get();
}

RoutingTable::RoutingTable() {
    for (auto e : {
        &RoutingEntry::Home,
        &RoutingEntry::Impressum,
        &RoutingEntry::About,
        &RoutingEntry::Contact,
        &RoutingEntry::List,
        &RoutingEntry::Show,
        &RoutingEntry::Search,
        &RoutingEntry::Images,
        &RoutingEntry::Assets,
        &RoutingEntry::CSS}) {
            m_entries[e->action] = e;
            size_t len = strlen(e->action);
            if (len>m_max_path_length) {
                m_max_path_length = len;
            }
    }
    m_entries["/"] = &RoutingEntry::Home;
}

RoutingTable::~RoutingTable() {}

const routing_entry * RoutingTable::get(const string& path) const {
    if (path.length() <= m_max_path_length) {
        auto itr = m_entries.find(path);
        if (itr != m_entries.end()) {
            return itr->second;
        }
    }
    return nullptr;
}

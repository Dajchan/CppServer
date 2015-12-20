#pragma once
#include "session.hpp"

namespace px {
	class Controller;
	typedef std::shared_ptr<Controller> Controller_p;
	
	class Controller {
	public:
		virtual Response_p get(Hash_p params, Session_p session) = 0;
		virtual Response_p post(Hash_p params, Session_p session) = 0;
	private:
	};
}
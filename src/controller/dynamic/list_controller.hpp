#pragma once
#include "controller.hpp"

ControllerInterface(ListController, HTMLController)

protected:
virtual unsigned int body(std::ostringstream& stream, Hash_p params);
virtual void cache_key(Hash_p params, string& out);

ControllerInterfaceEnd(ListController)
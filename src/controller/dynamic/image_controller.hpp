#pragma once
#include "controller.hpp"

ControllerInterface(ImageController, Controller)

virtual Response_p call(HTTP_Method method, Hash_p params);

ControllerInterfaceEnd(ImageController)
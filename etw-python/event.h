#pragma once
#include "py.h"
#include <krabs.hpp>
#include <sddl.h>

void event_callback(const EVENT_RECORD &record, PyObject *callback);

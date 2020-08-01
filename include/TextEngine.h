#pragma once

#include "IOLayer.h"

#include <cstdint>

namespace TextEngine {
    void init(int16_t menu_size);
    void handle_input(IO::keypress);
    void save(const char * filename);
};

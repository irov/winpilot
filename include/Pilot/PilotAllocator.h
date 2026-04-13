#pragma once

#include <cstddef>

namespace Pilot {

typedef void* (*PilotMallocFunc)(size_t size);
typedef void (*PilotFreeFunc)(void* ptr);

void pilot_set_allocator(PilotMallocFunc mallocFunc, PilotFreeFunc freeFunc);

void* pilot_malloc(size_t size);
void pilot_free(void* ptr);

} // namespace Pilot

#include "Pilot/PilotAllocator.h"

#include <cstdlib>

namespace Pilot {

static PilotMallocFunc s_mallocFunc = ::malloc;
static PilotFreeFunc s_freeFunc = ::free;

void pilot_set_allocator(PilotMallocFunc mallocFunc, PilotFreeFunc freeFunc) {
    s_mallocFunc = mallocFunc ? mallocFunc : ::malloc;
    s_freeFunc = freeFunc ? freeFunc : ::free;
}

void* pilot_malloc(size_t size) {
    return s_mallocFunc(size);
}

void pilot_free(void* ptr) {
    s_freeFunc(ptr);
}

} // namespace Pilot

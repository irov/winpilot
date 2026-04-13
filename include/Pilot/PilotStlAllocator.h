#pragma once

#include <Pilot/PilotAllocator.h>

#include <cstddef>

namespace Pilot {

template<class T>
class PilotStlAllocator {
public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    PilotStlAllocator() noexcept = default;

    template<class U>
    PilotStlAllocator(const PilotStlAllocator<U>&) noexcept {}

    T* allocate(size_type n) {
        return static_cast<T*>(pilot_malloc(n * sizeof(T)));
    }

    void deallocate(T* p, size_type) noexcept {
        pilot_free(p);
    }

    template<class U>
    bool operator==(const PilotStlAllocator<U>&) const noexcept { return true; }

    template<class U>
    bool operator!=(const PilotStlAllocator<U>&) const noexcept { return false; }
};

} // namespace Pilot

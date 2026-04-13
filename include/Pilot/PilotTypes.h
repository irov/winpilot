#pragma once

#include <Pilot/PilotStlAllocator.h>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <utility>

namespace Pilot {

//////////////////////////////////////////////////////////////////////////
// Container types with custom allocator
//////////////////////////////////////////////////////////////////////////

using PilotChar = char;
using PilotString = std::basic_string<PilotChar, std::char_traits<PilotChar>, PilotStlAllocator<PilotChar>>;

template<class T, class A = PilotStlAllocator<T>>
using PilotVector = std::vector<T, A>;

template<class K, class T, class L = std::less<K>, class A = PilotStlAllocator<std::pair<const K, T>>>
using PilotMap = std::map<K, T, L, A>;

template<class K, class T, class H = std::hash<K>, class E = std::equal_to<K>, class A = PilotStlAllocator<std::pair<const K, T>>>
using PilotUnorderedMap = std::unordered_map<K, T, H, E, A>;

//////////////////////////////////////////////////////////////////////////
// Smart pointer types
//////////////////////////////////////////////////////////////////////////

template<class T, class D = std::default_delete<T>>
using PilotUniquePtr = std::unique_ptr<T, D>;

//////////////////////////////////////////////////////////////////////////
// Function wrapper
//////////////////////////////////////////////////////////////////////////

template<class T>
using PilotFunction = std::function<T>;

//////////////////////////////////////////////////////////////////////////
// Utility types
//////////////////////////////////////////////////////////////////////////

template<class T1, class T2>
using PilotPair = std::pair<T1, T2>;

//////////////////////////////////////////////////////////////////////////
// Synchronization types
//////////////////////////////////////////////////////////////////////////

using PilotMutex = std::mutex;

template<class T>
using PilotAtomic = std::atomic<T>;

using PilotThread = std::thread;
using PilotConditionVariable = std::condition_variable;

//////////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////////

template<class T>
inline PilotString pilotToString(T val) {
    auto s = std::to_string(val);
    return PilotString(s.data(), s.size());
}

} // namespace Pilot

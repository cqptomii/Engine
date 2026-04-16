//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_RESOURCE_HANDLE_HPP
#define ENGINE_RESOURCE_HANDLE_HPP
#include <cstdint>
#include <limits>

template<typename T>
class ResourceHandle
{
public:
    static constexpr uint32_t INVALID_ID = std::numeric_limits<uint32_t>::max();

    uint32_t id = INVALID_ID;

    constexpr explicit operator bool() const noexcept
    {
        return this->id != INVALID_ID;
    }
};


#endif //ENGINE_RESOURCE_HANDLE_HPP
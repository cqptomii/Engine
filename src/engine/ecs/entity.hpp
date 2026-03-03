//
// Created by tomfr on 03/03/2026.
//

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cstdint>
#include <memory>


class Entity
{
    std::uint32_t _index;
    std::uint32_t _generation;
public:
    static constexpr std::uint32_t INVALID_ID = 0;

    constexpr Entity() : _index(INVALID_ID), _generation(0) {}
    constexpr Entity(const std::uint32_t index, const std::uint32_t generation) : _index(index), _generation(generation) {}

    constexpr std::uint32_t get_index() const noexcept
    {
        return this->_index;
    }
    constexpr std::uint32_t get_generation() const noexcept
    {
        return this->_generation;
    }

    constexpr bool operator!=(const Entity& other) const
    {
        return this->_index != other._index && this->_generation != other._generation;
    }
    constexpr explicit operator bool() const noexcept
    {
        return this->_index != INVALID_ID;
    }
};

#endif //ENTITY_HPP

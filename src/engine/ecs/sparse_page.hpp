//
// Created by tomfr on 10/03/2026.
//

#ifndef ENGINE_SPARSE_PAGE_HPP
#define ENGINE_SPARSE_PAGE_HPP
#include <array>
#include <limits>
#include <cstdint>

static constexpr std::size_t INVALID = std::numeric_limits<std::size_t>::max();
constexpr size_t PAGE_SIZE = 1024;
constexpr size_t PAGE_MASK = PAGE_SIZE - 1;

struct SparsePage
{
    std::array<size_t, PAGE_SIZE> data{};

    SparsePage()
    {
        data.fill(INVALID);
    }
};

#endif //ENGINE_SPARSE_PAGE_HPP
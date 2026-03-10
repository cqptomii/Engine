//
// Created by tomfr on 03/03/2026.
//

#ifndef COMPONENT_STORAGE_HPP
#define COMPONENT_STORAGE_HPP

#include "./entity.hpp"
#include "sparse_page.hpp"
#include <unordered_map>
#include <vector>
#include <cstdint>


class IComponentStorage
{
public:
    virtual ~IComponentStorage() = default;
    virtual void remove(const Entity& entity) = 0;
    virtual std::size_t size() const = 0;
};

template<typename T>
class ComponentStorage final : public IComponentStorage
{
    static constexpr std::size_t INVALID = std::numeric_limits<std::size_t>::max();

    // Dense structure which contains the Entity (index + generation) associated with the component
    struct Element
    {
        Entity entity;
        T component;
    };

    std::vector<std::unique_ptr<SparsePage>> pages;
    std::vector<Element> dense;

    static std::size_t page_index(const std::size_t entity_index) { return entity_index / PAGE_SIZE; }
    static std::size_t offset    (const std::size_t entity_index) { return entity_index & PAGE_MASK; }

    void ensure_page(const std::size_t page_idx)
    {
        if (page_idx >= pages.size())
            pages.resize(page_idx + 1);
    }

    // Writing access: Create the page if not defined
    size_t& sparse_at(const std::size_t entity_index)
    {
        std::size_t page_index = ComponentStorage::page_index(entity_index);
        std::size_t offset = ComponentStorage::offset(entity_index);

        if (page_index >= pages.size())
            pages.resize(page_index + 1);

        if (!pages[page_index])
            pages[page_index] = std::make_unique<SparsePage>();

        return this->pages[page_index]->data[offset];
    }

    // Reading access
    size_t sparse_at(const std::size_t entity_index) const
    {
        std::size_t page_index = ComponentStorage::page_index(entity_index);
        std::size_t offset = ComponentStorage::offset(entity_index);

        if (page_index >= pages.size() || !pages[page_index])
            return INVALID;

        return this->pages[page_index]->data[offset];
    }

public:
    void insert(const Entity& entity, T component)
    {
        const std::size_t entity_index = entity.get_index();
        // Get the sparse index to work with
        size_t& sparse_index = sparse_at(entity_index);

        if (sparse_index != INVALID)
        {
            dense[sparse_index].component = std::move(component);
            return;
        }

        // Link the entity index to the correct sparse page
        sparse_index = dense.size();
        // Fill dense vector
        dense.emplace_back({entity,std::move(component)});
    }
    void remove(const Entity& entity) override
    {
        if (!contains(entity))
            return;

        const uint32_t entity_index = entity.get_index();
        std::size_t dense_index = this->sparse_at(entity_index);
        std::size_t last_dense_index = dense.size() - 1;

        if (dense_index == INVALID)
            return;

        // Swap with the last index of the dense vector
        if (dense_index != last_dense_index)
        {
            dense[dense_index] = std::move(dense[last_dense_index]);
            this->sparse_at(dense[dense_index].entity.get_index()) = dense_index;
        }

        // Erase the last component from dense vectors
        dense.pop_back();
        // Erase the entity from the sparse map
        sparse_at(entity_index) = INVALID;
    }
    bool contains(const Entity& entity) const
    {
        size_t dense_index = this->sparse_at(entity.get_index());
        return dense_index != INVALID && dense[dense_index].entity == entity;
    }

    T& get(const Entity& entity)
    {
        size_t dense_index = this->sparse_at(entity.get_index());
        assert(contains(entity));
        return dense[dense_index].component;
    }
    const std::vector<Element>& data() const
    {
        return dense;
    }
    int size() const
    {
        return dense.size();
    }
};





#endif //COMPONENT_STORAGE_HPP

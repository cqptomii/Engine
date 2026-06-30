/**
 * @file resource_handle.hpp
 * @author Tom FRAISSE
 * @brief ResourceHandle is a class that represents a handle to a resource
 * @version 0.1
 * @date 2026-06-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef ENGINE_RESOURCE_HANDLE_HPP
#define ENGINE_RESOURCE_HANDLE_HPP
#include <cstdint>
#include <limits>


template<typename T>
class ResourceHandle
{
private:
    // INVALID_ID is used to represent an invalid resource handle
    static constexpr uint32_t INVALID_ID = std::numeric_limits<uint32_t>::max();
    
    // The id of the resource handle
    uint32_t m_id = INVALID_ID;

public:    
    /**
     * @brief Construct a new Resource Handle object with the default value
     * @details The default value is INVALID_ID
     */
    ResourceHandle() = default;
    
    /**
     * @brief Copy constructor
     * @details The copy constructor is used to copy a resource handle
     * @param other The resource handle to copy
     */
    ResourceHandle(const ResourceHandle&) = default;

    /**
     * @brief Copy assignment operator
     * @details The copy assignment operator is used to copy a resource handle
     * @param other The resource handle to copy
     * @return The resource handle
     */
    ResourceHandle& operator=(const ResourceHandle&) = default;

    /**
     * @brief Move constructor
     * @details The move constructor is used to move a resource handle
     * @param other The resource handle to move
     */
    ResourceHandle(ResourceHandle&& other) noexcept : m_id(other.m_id) { 
        other.m_id = INVALID_ID; 
    }

    /**
     * @brief Construct a new RessourceHandle based on an specified index
     * 
     * @param other_id 
     */
    ResourceHandle(uint32_t other_id) noexcept : m_id(other_id) {}

    /**
     * @brief Move assignment operator
     * @details The move assignment operator is used to move a resource handle
     * @param other The resource handle to move
     * @return The resource handle
     */
    ResourceHandle& operator=(ResourceHandle&& other) noexcept {
         m_id = other.m_id; 
         other.m_id = INVALID_ID; 
         return *this;
    }
    
    /**
     * @brief Destructor
     * @details The destructor is used to destroy a resource handle
     */
    ~ResourceHandle() = default;

    /**
     * @brief Check if the resource handle is valid
     * @details The check is done by comparing the id to INVALID_ID
     * @return true if the resource handle is valid, false otherwise
     */
    constexpr explicit operator bool() const noexcept
    {
        return this->m_id != INVALID_ID;
    }

    /**
     * @brief Getter for the id of the ressource handle
     * 
     * @return uint32_t  ressource handle id
     */
    uint32_t get_id() const{
        return m_id;
    }
};


#endif //ENGINE_RESOURCE_HANDLE_HPP
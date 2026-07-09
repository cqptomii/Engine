/**
 * @file aabb.hpp
 * @author Tom FRAISSE 
 * @brief Implementation of an Axis-Aligned Bounding Box
 * @version 0.1
 * @date 2026-07-09
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef AABB_HPP
#define AABB_HPP

#include <glm/glm.hpp>
#include "ray.hpp"

/**
 * @brief Enum class to represent the intersection type
 */
enum class Intersection {
    Inside,
    Outside,
    Intersecting
};

/**
 * @brief Class to represent an Axis-Aligned Bounding Box
 */
class AABB {
    public: 

        /**
         * @brief Default constructor
         * 
         */
        AABB(){
            m_min = glm::vec3(0.0f);
            m_max = glm::vec3(0.0f);
        }
        /**
         * @brief Constructor with min and max arguments
         * 
         * @param min The minimum point of the AABB
         * @param max The maximum point of the AABB
         */
        AABB(const glm::vec3& min, const glm::vec3& max){
            m_min = min;
            m_max = max;
        }
        
        /**
         * @brief Destroy the AABB object
         * 
         */
        ~AABB(){}
        
        /**
         * @brief Get the minimum point of the AABB
         * 
         * @return The minimum point of the AABB
         */
        glm::vec3 getMin() const{
            return m_min;
        }
        
        /**
         * @brief Get the maximum point of the AABB
         * 
         * @return The maximum point of the AABB
         */
        glm::vec3 getMax() const{
            return m_max;
        }

        /**
         * @brief Get the center point of the AABB
         * 
         * @return The center point of the AABB
         */
        glm::vec3 getCenter() const{
            return (m_min + m_max) / 2.0f;
        }
        /**
         * @brief Get the size of the AABB
         * 
         * @return The size of the AABB
         */
        glm::vec3 getSize() const{
            return m_max - m_min;
        }

        /**
         * @brief Set the AABB with min and max arguments
         * 
         * @param min The minimum point of the AABB
         * @param max The maximum point of the AABB
         */
        void set(const glm::vec3& min, const glm::vec3& max){
            m_min = min;
            m_max = max;
        }
        /**
         * @brief Merge the AABB with another AABB
         * 
         * @param other The other AABB
         */
        void merge(const AABB& other){
            m_min = glm::min(m_min, other.m_min);
            m_max = glm::max(m_max, other.m_max);
        }

        /**
         * @brief Check if the AABB intersects with another AABB
         * 
         * @param other The other AABB
         * @return The intersection type
         */
        Intersection intersects(const AABB& other) const{
            return (m_min.x <= other.m_max.x && m_max.x >= other.m_min.x &&
                    m_min.y <= other.m_max.y && m_max.y >= other.m_min.y &&
                    m_min.z <= other.m_max.z && m_max.z >= other.m_min.z) ? Intersection::Intersecting : Intersection::Outside;
        }

        /**
         * @brief Check if the AABB intersects with a point
         * 
         * @param point The point to check
         * @return The intersection type
         * @return Intersection 
         */
        Intersection intersects(const glm::vec3& point) const{
            return (point.x >= m_min.x && point.x <= m_max.x &&
                    point.y >= m_min.y && point.y <= m_max.y &&
                    point.z >= m_min.z && point.z <= m_max.z) ? Intersection::Intersecting : Intersection::Outside;
        }

        /**
         * @brief Check if the AABB intersects with a ray
         * 
         * @param ray The ray to check
         * @param t The distance from the origin of the ray to the intersection point
         * @return The intersection type
         */
        Intersection intersects(const Ray& ray, float& t) const{
            return Intersection::Intersecting;
        }

    private:

        /**
         * @brief The minimum point of the AABB
         */
        glm::vec3 m_min;
        
        /**
         * @brief The maximum point of the AABB
         */
        glm::vec3 m_max;
};
#endif
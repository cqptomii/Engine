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

#include <cfloat>
#include <limits>
#include <glm/glm.hpp>
#include <vector>
#include "ray.hpp"
#include "engine/resources/cpu/vertex.hpp"

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
         * @brief Create an AABB from a list of vertices
         * 
         * @param vertices The vertices of the AABB
         * @return The AABB
         */
        static AABB from_vertices(const std::vector<Vertex>& vertices){
            glm::vec3 min = vertices[0].vertice;
            glm::vec3 max = vertices[0].vertice;
            for(const auto& vertex : vertices){
                min = glm::min(min, vertex.vertice);
                max = glm::max(max, vertex.vertice);
            }
            return AABB(min, max);
        }


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
         * @brief Transform the AABB to the world space
         * 
         * @param transform The transform matrix
         * @return AABB 
         */
        AABB transform_to_world(const glm::mat4& model_matrix) const
        {
            const glm::vec3 corners[8] = {
                {m_min.x, m_min.y, m_min.z},
                {m_max.x, m_min.y, m_min.z},
                {m_min.x, m_max.y, m_min.z},
                {m_max.x, m_max.y, m_min.z},
                {m_min.x, m_min.y, m_max.z},
                {m_max.x, m_min.y, m_max.z},
                {m_min.x, m_max.y, m_max.z},
                {m_max.x, m_max.y, m_max.z},
            };

            glm::vec3 world_min(std::numeric_limits<float>::max());
            glm::vec3 world_max(std::numeric_limits<float>::lowest());

            for (const glm::vec3& corner : corners)
            {
                const glm::vec3 transformed = glm::vec3(model_matrix * glm::vec4(corner, 1.0f));
                world_min = glm::min(world_min, transformed);
                world_max = glm::max(world_max, transformed);
            }

            return AABB(world_min, world_max);
        }



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
         * @brief Check if a world-space ray intersects this world-space AABB.
         *
         * @param ray World-space ray (origin + direction already include camera transform).
         * @param t_out Distance along the ray to the nearest entry point.
         * @return true if the ray hits the box in front of the origin.
         */
        bool intersectsRay(const Ray& ray, float& t_out) const
        {
            const glm::vec3 origin = ray.getOrigin();
            const glm::vec3 direction = ray.getDirection();

            float t_min = 0.0f;
            float t_max = FLT_MAX;

            for (int axis = 0; axis < 3; ++axis)
            {
                const float origin_axis = origin[axis];
                const float direction_axis = direction[axis];
                    
                // Check if the direction is parallel to the axis
                if (std::abs(direction_axis) < 1e-8f)
                {
                    if (origin_axis < m_min[axis] || origin_axis > m_max[axis])
                    {
                        return false;
                    }
                    continue;
                }

                // Calculate the intersection points
                const float inv_direction = 1.0f / direction_axis;
                float t0 = (m_min[axis] - origin_axis) * inv_direction;
                float t1 = (m_max[axis] - origin_axis) * inv_direction;

                // Swap the intersection points if t0 is greater than t1
                if (t0 > t1)
                {
                    std::swap(t0, t1);
                }

                // Update the intersection points
                t_min = std::max(t_min, t0);
                t_max = std::min(t_max, t1);

                // Check if the intersection points are valid
                if (t_min > t_max)
                {
                    return false;
                }
            }

            // Check if the intersection points are valid
            if (t_max < 0.0f)
            {
                return false;
            }

            t_out = t_min >= 0.0f ? t_min : t_max;
            return true;
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
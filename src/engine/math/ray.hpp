#ifndef RAY_HPP
#define RAY_HPP

#include <glm/glm.hpp>

/**
 * @brief Structure to store the result of a ray cast
 */
struct RayCastHit {
    /**
     * @brief Whether the ray hit an object
     */
    bool hit = false;
    /**
     * @brief The distance from the ray origin to the hit point
     */
    float distance = 0.f;
    /**
     * @brief The point of the hit
     */
    glm::vec3 point{0.f};

}; 

/**
 * @brief Class to represent a ray
 * @details A ray is defined by an origin and a direction
 */
class Ray {
    public:
        /**
         * @brief Construct a new Ray object
         * @param origin The origin of the ray
         * @param direction The direction of the ray
         */
        Ray(const glm::vec3& origin, const glm::vec3& direction) : m_origin(origin), m_direction(direction) {
            //Normalize the direction
            m_direction = glm::normalize(direction);
        }
        
        /**
         * @brief Get the direction of the ray
         * @return The direction of the ray
         */
        glm::vec3 getDirection() const{
            return m_direction;
        }

        /**
         * @brief Get the point on the ray at the given distance
         * @param t The distance from the origin of the ray to the point
         * @return The point on the ray at the given distance
         */
        glm::vec3 at(float t) const{
            return m_origin + t * m_direction;
        }

    private:
        /**
         * @brief The origin of the ray
         */
        glm::vec3 m_origin;

        /**
         * @brief The direction of the ray
         */
        glm::vec3 m_direction;
    };


#endif

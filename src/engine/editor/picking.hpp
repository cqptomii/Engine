#ifndef PICKING_HPP
#define PICKING_HPP

#include "engine/scene/scene.hpp"
#include "engine/math/ray.hpp"

void pick_closest_entity(Scene& scene, const Ray& ray);

#endif
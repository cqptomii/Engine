#ifndef MESH_PRIMITIVE_2D_HPP
#define MESH_PRIMITIVE_2D_HPP

#include "engine/resources/mesh_resource.hpp"

class MeshPrimitive2D
{
public:
    MeshPrimitive2D() = default;
    ~MeshPrimitive2D() = default;

    static MeshResource* CreateQuad();
    static MeshResource* CreateCircle(unsigned int segments = 32);
    static MeshResource* CreatePolygon(unsigned int sides);
};


#endif // MESH_PRIMITIVE_2D_HPP
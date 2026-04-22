#ifndef MESH_PRIMITIVE_3D_HPP
#define MESH_PRIMITIVE_3D_HPP

#include "engine/resources/mesh_resource.hpp"
#include "engine/core/wrapper/VAO.hpp"
#include "engine/core/wrapper/Buffer.hpp"

class MeshPrimitive3D
{
public:
    MeshPrimitive3D() = default;
    ~MeshPrimitive3D() = default;

    static MeshResource* CreateCube();
    static MeshResource* CreateSphere(unsigned int segments = 32);
    static MeshResource* CreateCylinder(unsigned int segments = 32);
    static MeshResource* CreateCone(unsigned int segments = 32);
    static MeshResource* CreateTorus(unsigned int segments = 32, unsigned int rings = 16);

};


#endif // MESH_PRIMITIVE_3D_HPP
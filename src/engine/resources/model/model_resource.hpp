//
// Created by tomfr on 09/03/2026.
//

#ifndef ENGINE_MODEL_RESOURCE_HPP
#define ENGINE_MODEL_RESOURCE_HPP

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include "engine/resources/resource_handle.hpp"
#include "engine/resources/mesh_resource.hpp"
#include "engine/resources/material/material_instance.hpp"

class ResourceManager;

struct ModelNode
{
    glm::mat4 local_transform{};
    std::vector<int> mesh_indexes;
    std::vector<int> children;
};

class ModelResource
{
    std::vector<ModelNode> nodes;
    std::vector<ResourceHandle<MeshResource>> meshes;
    std::vector<ResourceHandle<MaterialInstance>> materials;
    std::unordered_map<unsigned int, ResourceHandle<MaterialInstance>> material_instances_by_index;
    std::filesystem::path directory;

    static glm::mat4 to_glm_matrix(const aiMatrix4x4& m);

    void load_model(const std::string& path, ResourceManager& resource_manager);
    int process_node(const aiNode* node, const aiScene* scene, ResourceManager& resource_manager);
    int process_mesh(aiMesh* mesh, const aiScene* scene, ResourceManager& resource_manager);
    std::vector<std::string> load_texture_paths(const aiMaterial* mat, aiTextureType type) const;

public:
    explicit ModelResource(const std::string& path, ResourceManager& resource_manager);
    ~ModelResource() = default;

    const std::vector<ModelNode>& get_nodes() const { return this->nodes; }
    const std::vector<ResourceHandle<MeshResource>>& get_meshes() const { return this->meshes; }
    const std::vector<ResourceHandle<MaterialInstance>>& get_materials() const { return this->materials; }
};

#endif //ENGINE_MODEL_RESOURCE_HPP
//
// Created by tomfr on 09/03/2026.
//

#include "engine/resources/cpu/model/model_resource.hpp"

#include <iostream>

#include "engine/resources/cpu/cpu_resource_manager.hpp"

glm::mat4 ModelResource::to_glm_matrix(const aiMatrix4x4& m)
{
	return {
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
	};
}

void ModelResource::load_model(const std::string& path, CpuResourceManager& resource_manager)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	const std::filesystem::path model_path(path);
	this->directory = model_path.parent_path();
	this->process_node(scene->mRootNode, scene, resource_manager);
}

int ModelResource::process_node(const aiNode* node, const aiScene* scene, CpuResourceManager& resource_manager)
{
	ModelNode model_node;
	model_node.local_transform = to_glm_matrix(node->mTransformation);

	const int node_index = static_cast<int>(this->nodes.size());
	this->nodes.push_back(model_node);

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		const int mesh_index = this->process_mesh(mesh, scene, resource_manager);
		this->nodes[node_index].mesh_indexes.push_back(mesh_index);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		int child_index = this->process_node(node->mChildren[i], scene, resource_manager);
		this->nodes[node_index].children.push_back(child_index);
	}

	return node_index;
}

int ModelResource::process_mesh(aiMesh* mesh, const aiScene* scene, CpuResourceManager& resource_manager)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};

		vertex.vertice = {
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		};

		if (mesh->HasNormals())
		{
			vertex.normal = {
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
			};
		}

		if (mesh->HasTextureCoords(0))
		{
			vertex.texcoord = {
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			};
		}

		if (mesh->HasVertexColors(0))
		{
			vertex.color = {
				mesh->mColors[0][i].r,
				mesh->mColors[0][i].g,
				mesh->mColors[0][i].b,
				mesh->mColors[0][i].a
			};
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	const int mesh_index = static_cast<int>(this->meshes.size());
	const std::string mesh_cache_key = this->directory.generic_string() + "/__mesh_" + std::to_string(mesh_index);

	ResourceHandle<MeshResource> mesh_handle = resource_manager.load_mesh(mesh_cache_key, vertices, indices);
	MeshResource& mesh_resource = resource_manager.get_mesh(mesh_handle);
	SubMesh submesh{};
	submesh.index_offset = 0;
	submesh.index_count = static_cast<uint32_t>(indices.size());

	if (mesh->mMaterialIndex >= 0)
	{
		const unsigned int material_index = mesh->mMaterialIndex;
		aiMaterial* mat = scene->mMaterials[material_index];

		if (!this->material_instances_by_index.contains(material_index))
		{
			const std::string material_cache_key = this->directory.generic_string() + "/__material_" + std::to_string(material_index);
			auto diffuse_paths = load_texture_paths(mat, aiTextureType_DIFFUSE);
			auto normal_paths = load_texture_paths(mat, aiTextureType_NORMALS);
			diffuse_paths.insert(diffuse_paths.end(), normal_paths.begin(), normal_paths.end());

			const auto material_resource = resource_manager.load_material_resource(
				material_cache_key,
				"sources/shader/base.vs",
				"sources/shader/base.fs",
				diffuse_paths
			);
			const auto material_instance = resource_manager.create_material_instance(material_resource);
			this->material_instances_by_index[material_index] = material_instance;
			this->materials.push_back(material_instance);
		}

		submesh.material = this->material_instances_by_index[material_index];
	}

	mesh_resource.add_submesh(submesh);

	this->meshes.push_back(mesh_handle);
	return mesh_index;
}

std::vector<std::string> ModelResource::load_texture_paths(const aiMaterial* mat, aiTextureType type) const
{
	std::vector<std::string> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		const std::string texture_ref = str.C_Str();
		if (texture_ref.empty() || texture_ref[0] == '*')
		{
			continue;
		}

		std::filesystem::path texture_path = this->directory / texture_ref;
		textures.push_back(texture_path.generic_string());
	}
	return textures;
}

ModelResource::ModelResource(const std::string& path, CpuResourceManager& resource_manager)
{
	this->load_model(path, resource_manager);
}

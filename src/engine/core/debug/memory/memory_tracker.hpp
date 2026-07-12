#ifndef ENGINE_DEBUG_MEMORY_TRACKER_HPP
#define ENGINE_DEBUG_MEMORY_TRACKER_HPP

#include <cstddef>
#include <iostream>

#include "engine/resources/cpu/cpu_resource_manager.hpp"
#include "engine/resources/gpu/gpu_resource_manager.hpp"

struct CpuResourceCounts {
    std::size_t models = 0;
    std::size_t meshes = 0;
    std::size_t materials = 0;
    std::size_t material_instances = 0;
    std::size_t textures = 0;
    std::size_t shaders = 0;
};

struct GpuResourceCounts {
    std::size_t meshes = 0;
    std::size_t textures = 0;
    std::size_t shaders = 0;
    std::size_t ubos = 0;
};

class MemoryTracker {
    CpuResourceCounts cpu_counts{};
    GpuResourceCounts gpu_counts{};

public:
    void refresh(const CpuResourceManager& cpu_manager, const GPUResourceManager& gpu_manager) {
        this->cpu_counts = {
            cpu_manager.count_models(),
            cpu_manager.count_meshes(),
            cpu_manager.count_materials(),
            cpu_manager.count_material_instances(),
            cpu_manager.count_textures(),
            cpu_manager.count_shaders()
        };

        this->gpu_counts = {
            gpu_manager.count_meshes(),
            gpu_manager.count_textures(),
            gpu_manager.count_shaders(),
            gpu_manager.count_ubos()
        };
    }

    void print() const {
        std::cout << "CPU resources:" << std::endl;
        std::cout << "  models: " << this->cpu_counts.models
                  << "  meshes: " << this->cpu_counts.meshes
                  << "  materials: " << this->cpu_counts.materials << std::endl;
        std::cout << "  material_instances: " << this->cpu_counts.material_instances
                  << "  textures: " << this->cpu_counts.textures
                  << "  shaders: " << this->cpu_counts.shaders << std::endl;

        std::cout << "GPU resources:" << std::endl;
        std::cout << "  meshes: " << this->gpu_counts.meshes
                  << "  textures: " << this->gpu_counts.textures
                  << "  shaders: " << this->gpu_counts.shaders
                  << "  ubos: " << this->gpu_counts.ubos << std::endl;
    }

    const CpuResourceCounts& cpu() const {
        return this->cpu_counts;
    }

    const GpuResourceCounts& gpu() const {
        return this->gpu_counts;
    }
};

#endif

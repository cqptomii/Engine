/**
 * @file memory_tracker.hpp
 * @author FRAISSE Tom
 * @brief Memory tracker class and structures for the engine debug system
 * @details This class is responsible for the tracking of the memory usage of the engine debug system
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_MEMORY_TRACKER_HPP
#define ENGINE_DEBUG_MEMORY_TRACKER_HPP

#include <cstddef>
#include <iostream>

#include "engine/resources/cpu/cpu_resource_manager.hpp"
#include "engine/resources/gpu/gpu_resource_manager.hpp"

/**
 * @brief Structure to count the CPU resources currently allocated
 * 
 */
struct CpuResourceCounts {
    std::size_t models = 0;
    std::size_t meshes = 0;
    std::size_t materials = 0;
    std::size_t material_instances = 0;
    std::size_t textures = 0;
    std::size_t shaders = 0;
};

/**
 * @brief Structure to count the GPU resources currently allocated
 * 
 */
struct GpuResourceCounts {
    std::size_t meshes = 0;
    std::size_t textures = 0;
    std::size_t shaders = 0;
    std::size_t ubos = 0;
};

/**
 * @brief Memory tracker class for the engine debug system
 * 
 */
class MemoryTracker {
    
    // The CPU resource counts
    CpuResourceCounts cpu_counts{};

    // The GPU resource counts
    GpuResourceCounts gpu_counts{};

public:

    /**
     * @brief Method to refresh the memory tracker
     * 
     * @details It counts the CPU and GPU resources currently allocated
     * @param cpu_manager The CPU resource manager
     * @param gpu_manager The GPU resource manager
     * @return void
     */
    void refresh(const CpuResourceManager& cpu_manager, const GPUResourceManager& gpu_manager) {
        
        // Count the CPU resources
        this->cpu_counts = {
            cpu_manager.count_models(),
            cpu_manager.count_meshes(),
            cpu_manager.count_materials(),
            cpu_manager.count_material_instances(),
            cpu_manager.count_textures(),
            cpu_manager.count_shaders()
        };

        // Count the GPU resources
        this->gpu_counts = {
            gpu_manager.count_meshes(),
            gpu_manager.count_textures(),
            gpu_manager.count_shaders(),
            gpu_manager.count_ubos()
        };
    }

    /**
     * @brief Method to print the memory tracker
     * 
     * @details It prints in readable format the CPU and GPU resources currently allocated
     * @return void
     */
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

    /**
     * @brief Method to get the CPU resource counts
     * 
     * @return const CpuResourceCounts& 
     */
    const CpuResourceCounts& cpu() const {
        return this->cpu_counts;
    }

    /**
     * @brief Method to get the GPU resource counts
     * 
     * @return const GpuResourceCounts& 
     */
    const GpuResourceCounts& gpu() const {
        return this->gpu_counts;
    }
};

#endif

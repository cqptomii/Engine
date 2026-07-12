#pragma once

#ifdef ENGINE_DEBUG
    #define ENGINE_DEBUG_ENABLED 1

    #define ENGINE_PROFILE_SCOPE(name) \
        ScopedTimer _engine_profile_scope_##__LINE__{Instrumentation::profiler(), name}

    #define ENGINE_COUNTER_ADD(id, key, amount) \
        Instrumentation::counters().add(id, key, amount)

    #define ENGINE_LOG(level, category, message) \
        Instrumentation::logger().log(level, category, message)

    #define ENGINE_REGISTER_NAME(id, name) \
        Instrumentation::names().register_name(id, name)

    #define ENGINE_REFRESH_MEMORY(cpu_manager, gpu_manager) \
        Instrumentation::refresh_memory(cpu_manager, gpu_manager)
#else
    #define ENGINE_DEBUG_ENABLED 0
    #define ENGINE_PROFILE_SCOPE(name) ((void)0)
    #define ENGINE_COUNTER_ADD(id, key, amount) ((void)0)
    #define ENGINE_LOG(level, category, message) ((void)0)
    #define ENGINE_REGISTER_NAME(id, name) ((void)0)
    #define ENGINE_REFRESH_MEMORY(cpu_manager, gpu_manager) ((void)0)
#endif

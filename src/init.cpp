#include "debug_agent.hpp"
#include <iostream>

std::unique_ptr<agent::DebugAgent> _debug_agent;
std::shared_ptr<CoreApiTable> _intercepted_api_table;

hsa_status_t intercept_hsa_code_object_reader_create_from_memory(
    const void* code_object,
    size_t size,
    hsa_code_object_reader_t* code_object_reader)
{
    return _debug_agent->intercept_hsa_code_object_reader_create_from_memory(
        _intercepted_api_table->hsa_code_object_reader_create_from_memory_fn,
        code_object, size, code_object_reader);
}

hsa_status_t intercept_hsa_code_object_deserialize(
    void* serialized_code_object,
    size_t serialized_code_object_size,
    const char* options,
    hsa_code_object_t* code_object)
{
    return _debug_agent->intercept_hsa_code_object_deserialize(
        _intercepted_api_table->hsa_code_object_deserialize_fn,
        serialized_code_object, serialized_code_object_size, options, code_object);
}

hsa_status_t intercept_hsa_executable_load_agent_code_object(
    hsa_executable_t executable,
    hsa_agent_t agent,
    hsa_code_object_reader_t code_object_reader,
    const char* options,
    hsa_loaded_code_object_t* loaded_code_object)
{
    return _debug_agent->intercept_hsa_executable_load_agent_code_object(
        _intercepted_api_table->hsa_executable_load_agent_code_object_fn,
        executable, agent, code_object_reader, options, loaded_code_object);
}

hsa_status_t intercept_hsa_executable_load_code_object(
    hsa_executable_t executable,
    hsa_agent_t agent,
    hsa_code_object_t code_object,
    const char* options)
{
    return _debug_agent->intercept_hsa_executable_load_code_object(
        _intercepted_api_table->hsa_executable_load_code_object_fn,
        executable, agent, code_object, options);
}

hsa_status_t intercept_hsa_executable_symbol_get_info(
    hsa_executable_symbol_t executable_symbol,
    hsa_executable_symbol_info_t attribute,
    void* value)
{
    return _debug_agent->intercept_hsa_executable_symbol_get_info(
        _intercepted_api_table->hsa_executable_symbol_get_info_fn,
        executable_symbol, attribute, value);
}

extern "C" bool OnLoad(void* api_table_ptr, uint64_t rt_version, uint64_t failed_tool_cnt, const char* const* failed_tool_names)
{
    try
    {
        auto api_table = reinterpret_cast<HsaApiTable*>(api_table_ptr);
        _intercepted_api_table = std::make_shared<CoreApiTable>();
        memcpy(_intercepted_api_table.get(), static_cast<const void*>(api_table->core_), sizeof(CoreApiTable));

        api_table->core_->hsa_code_object_reader_create_from_memory_fn = intercept_hsa_code_object_reader_create_from_memory;
        api_table->core_->hsa_code_object_deserialize_fn = intercept_hsa_code_object_deserialize;
        api_table->core_->hsa_executable_load_agent_code_object_fn = intercept_hsa_executable_load_agent_code_object;
        api_table->core_->hsa_executable_load_code_object_fn = intercept_hsa_executable_load_code_object;
        api_table->core_->hsa_executable_symbol_get_info_fn = intercept_hsa_executable_symbol_get_info;

        auto config = std::make_shared<agent::Config>();
        auto logger = std::make_shared<agent::AgentLogger>(config->agent_log_file());
        auto co_logger = std::make_shared<agent::CodeObjectLogger>(config->code_object_log_file());
        auto co_loader = std::make_unique<agent::CodeObjectLoader>(_intercepted_api_table);
        _debug_agent = std::make_unique<agent::DebugAgent>(config, logger, co_logger, std::move(co_loader));
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }

    return true;
}

extern "C" void OnUnload()
{
    _debug_agent->write_debug_buffer_to_file();
    _debug_agent.reset();
    _intercepted_api_table.reset();
}

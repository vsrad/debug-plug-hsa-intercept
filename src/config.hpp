#pragma once

#include "buffer_allocation.hpp"
#include "code_object_swap.hpp"
#include <vector>

namespace agent
{
class Config
{
private:
    std::string _agent_log_file;
    uint64_t _debug_buffer_size;
    std::string _debug_buffer_dump_file;
    std::string _code_object_log_file;
    std::string _code_object_dump_dir;
    std::vector<BufferAllocation> _buffer_allocations;
    std::vector<CodeObjectSwap> _code_object_swaps;

public:
    Config();
    const std::string& agent_log_file() const { return _agent_log_file; }
    uint64_t debug_buffer_size() const { return _debug_buffer_size; }
    const std::string& debug_buffer_dump_file() const { return _debug_buffer_dump_file; }
    const std::string& code_object_log_file() const { return _code_object_log_file; }
    const std::string& code_object_dump_dir() const { return _code_object_dump_dir; }
    const std::vector<BufferAllocation>& buffer_allocations() const { return _buffer_allocations; }
    const std::vector<CodeObjectSwap>& code_object_swaps() const { return _code_object_swaps; }
};
} // namespace agent

#pragma once

#include "code_object_loader.hpp"
#include "logger/logger.hpp"
#include "buffer.hpp"
#include <atomic>
#include <vector>

namespace agent
{
class CodeObjectSwapper
{
private:
    std::shared_ptr<std::vector<CodeObjectSwap>> _swaps;
    std::shared_ptr<AgentLogger> _logger;
    std::atomic<uint64_t> _call_counter;
    std::optional<CodeObject> do_swap(const CodeObjectSwap& swap, std::shared_ptr<CodeObject> source, const std::unique_ptr<Buffer>& debug_buffer);

    struct SymbolSwap
    {
        const CodeObjectSwap& swap;
        CodeObject replacement_co;
        hsa_executable_t exec;
    };
    std::unordered_map<std::shared_ptr<CodeObject>, SymbolSwap> _symbol_swaps;

public:
    CodeObjectSwapper(std::shared_ptr<std::vector<CodeObjectSwap>> swaps, std::shared_ptr<AgentLogger> logger)
        : _swaps(swaps), _logger(logger), _call_counter(0) {}
    std::optional<CodeObject> get_swapped_code_object(std::shared_ptr<CodeObject> source, const std::unique_ptr<Buffer>& debug_buffer);
    void prepare_symbol_swap(std::shared_ptr<CodeObject> source, CodeObjectLoader& co_loader, hsa_agent_t agent);
    std::optional<hsa_executable_symbol_t> swap_symbol(std::shared_ptr<CodeObject> source, const std::string& source_symbol_name);
};
}; // namespace agent

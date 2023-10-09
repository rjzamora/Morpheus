#include "morpheus/llm/llm_engine.hpp"

#include "morpheus/llm/llm_task_handler_runner.hpp"
#include "morpheus/llm/utils.hpp"

#include <mrc/coroutines/task.hpp>

namespace morpheus::llm {

LLMEngine::LLMEngine() = default;

LLMEngine::~LLMEngine() = default;

void LLMEngine::add_task_handler(input_map_t inputs, std::shared_ptr<LLMTaskHandler> task_handler)
{
    auto input_names = task_handler->get_input_names();

    auto final_inputs = process_input_names(inputs, input_names);

    m_task_handlers.push_back(std::make_shared<LLMTaskHandlerRunner>(std::move(final_inputs), task_handler));
}

Task<std::vector<std::shared_ptr<ControlMessage>>> LLMEngine::run(std::shared_ptr<ControlMessage> input_message)
{
    if (!input_message)
    {
        throw std::runtime_error("LLMEngine::run() called with a null message");
    }

    if (!input_message->has_task("llm_engine"))
    {
        throw std::runtime_error("LLMEngine::run() called with a message that does not have the 'llm_engine' task");
    }

    std::vector<std::shared_ptr<ControlMessage>> output_messages;

    while (input_message->has_task("llm_engine"))
    {
        auto current_task = input_message->remove_task("llm_engine");

        // Temp create an instance of LLMTask for type safety
        LLMTask tmp_task(current_task["task_type"].get<std::string>(), current_task.at("task_dict"));

        // Set the name, task, control_message and inputs on the context
        auto context = std::make_shared<LLMContext>(tmp_task, input_message);

        // Call the base node
        co_await this->execute(context);

        // Pass the outputs into the task generators
        auto tasks = co_await this->handle_tasks(context);

        output_messages.insert(output_messages.end(), tasks.begin(), tasks.end());
    }

    co_return output_messages;
}

Task<std::vector<std::shared_ptr<ControlMessage>>> LLMEngine::handle_tasks(std::shared_ptr<LLMContext> context)
{
    // Wait for the base node outputs (This will yield if not already available)
    // auto outputs = context->get_outputs();

    for (auto& task_handler : m_task_handlers)
    {
        auto new_tasks = co_await task_handler->try_handle(context);

        if (new_tasks.has_value())
        {
            co_return new_tasks.value();
        }
    }

    throw std::runtime_error("No task handler was able to handle the input message and responses generated");
}

}  // namespace morpheus::llm

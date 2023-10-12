/*
 * SPDX-FileCopyrightText: Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "morpheus/export.h"
#include "morpheus/llm/input_map.hpp"
#include "morpheus/llm/llm_task.hpp"
#include "morpheus/messages/control.hpp"
#include "morpheus/utilities/json_types.hpp"

#include <mrc/types.hpp>
#include <nlohmann/json.hpp>

#include <memory>
#include <string>
#include <vector>

namespace morpheus::llm {

struct LLMContextState
{
    LLMTask task;
    std::shared_ptr<ControlMessage> message;
    nlohmann::json values;
};

class MORPHEUS_EXPORT LLMContext : public std::enable_shared_from_this<LLMContext>
{
  public:
    LLMContext();

    LLMContext(LLMTask task, std::shared_ptr<ControlMessage> message);

    LLMContext(std::shared_ptr<LLMContext> parent, std::string name, input_mappings_t inputs);

    ~LLMContext();

    std::shared_ptr<LLMContext> parent() const;

    const std::string& name() const;

    const input_mappings_t& input_map() const;

    const LLMTask& task() const;

    std::shared_ptr<ControlMessage>& message() const;

    nlohmann::json::const_reference all_outputs() const;

    std::string full_name() const;

    std::shared_ptr<LLMContext> push(std::string name, input_mappings_t inputs);

    void pop();

    nlohmann::json::const_reference get_input() const;

    nlohmann::json::const_reference get_input(const std::string& node_name) const;

    nlohmann::json get_inputs() const;

    void set_output(nlohmann::json outputs);

    void set_output(const std::string& output_name, nlohmann::json output);

    void set_output_names(std::vector<std::string> output_names);

    void outputs_complete();

    nlohmann::json::const_reference view_outputs() const;

  private:
    std::shared_ptr<LLMContext> m_parent{nullptr};
    std::string m_name;
    input_mappings_t m_inputs;
    std::vector<std::string> m_output_names;  // Names of keys to be used as the output. Empty means use all keys

    std::shared_ptr<LLMContextState> m_state;

    nlohmann::json m_outputs;

    mrc::Promise<void> m_outputs_promise;
    mrc::SharedFuture<void> m_outputs_future;
};
}  // namespace morpheus::llm

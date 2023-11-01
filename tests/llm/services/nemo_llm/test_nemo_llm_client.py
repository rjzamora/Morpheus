# SPDX-FileCopyrightText: Copyright (c) 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from unittest import mock

from morpheus.llm.services.nemo_llm_service import NeMoLLMClient


def _make_mock_nemo_llm():
    mock_nemo_llm = mock.MagicMock()
    mock_nemo_llm.return_value = mock_nemo_llm
    mock_nemo_llm.generate_multiple.return_value = ["test_output"]
    return mock_nemo_llm


def _make_mock_nemo_service():
    mock_nemo_llm = _make_mock_nemo_llm()
    mock_nemo_service = mock.MagicMock()
    mock_nemo_service.return_value = mock_nemo_service
    mock_nemo_service._conn = mock_nemo_llm
    return (mock_nemo_service, mock_nemo_llm)


def test_generate():
    (mock_nemo_service, mock_nemo_llm) = _make_mock_nemo_service()

    client = NeMoLLMClient(mock_nemo_service, "test_model", additional_arg="test_arg")
    assert client.generate({'prompt': "test_prompt"}) == "test_output"
    mock_nemo_llm.generate_multiple.assert_called_once_with(model="test_model",
                                                            prompts=["test_prompt"],
                                                            return_type="text",
                                                            additional_arg="test_arg")


def test_generate_batch():
    (mock_nemo_service, mock_nemo_llm) = _make_mock_nemo_service()
    mock_nemo_llm.generate_multiple.return_value = ["output1", "output2"]

    client = NeMoLLMClient(mock_nemo_service, "test_model", additional_arg="test_arg")
    assert client.generate_batch({'prompt': ["prompt1", "prompt2"]}) == ["output1", "output2"]
    mock_nemo_llm.generate_multiple.assert_called_once_with(model="test_model",
                                                            prompts=["prompt1", "prompt2"],
                                                            return_type="text",
                                                            additional_arg="test_arg")

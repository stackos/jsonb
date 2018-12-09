/*
* jsonb
* Copyright 2018-2019 by Stack - stackos@qq.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#pragma once

#include <json/json.h>

namespace jsonb
{
    class Document
    {
    public:
        Document();
        ~Document();
        bool Load(const std::string& json);
        bool Load(const void* binary, size_t size);
        std::string ToJson();
        const void* GetBinary() const { return m_binary; }
        size_t GetBinarySize() const { return m_binary_size; }

    private:
        Json::Value m_root;
        void* m_binary;
        size_t m_binary_size;
    };
}

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
#include <sstream>

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
        void WriteValue(std::ostringstream& os, const Json::Value& value);
        void WriteObject(std::ostringstream& os, const Json::Value& obj);
        void WriteArray(std::ostringstream& os, const Json::Value& arr);
        void WriteInt32(std::ostringstream& os, int32_t i);
        void WriteUint32(std::ostringstream& os, uint32_t i);
        void WriteFloat(std::ostringstream& os, float f);
        void WriteString(std::ostringstream& os, const Json::Value& s);
        void WriteBool(std::ostringstream& os, bool b);
        void WriteNull(std::ostringstream& os);
        template <class T>
        void Write(std::ostringstream& os, const T& t)
        {
            os.write((const char*) &t, sizeof(t));
        }

        Json::Value ReadValue(std::istringstream& is);
        Json::Value ReadObject(std::istringstream& is);
        Json::Value ReadArray(std::istringstream& is);
        Json::Value ReadString(std::istringstream& is);
        template <class T>
        T Read(std::istringstream& is)
        {
            T t;
            is.read((char*) &t, sizeof(t));
            return t;
        }

    private:
        Json::Value m_root;
        void* m_binary;
        size_t m_binary_size;
    };
}

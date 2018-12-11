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

#include <jsonb/jsonb.h>

namespace jsonb
{
    enum class ValueType
    {
        Object,
        Array,
        String,
        Uint8,
        Int8,
        Uint16,
        Int16,
        Uint32,
        Int32,
        Float,
        Bool,
        Null,
    };

    Document::Document():
        m_binary(nullptr),
        m_binary_size(0)
    {
        
    }

    Document::~Document()
    {
        if (m_binary)
        {
            free(m_binary);
            m_binary = nullptr;
        }
        m_binary_size = 0;
    }

    void Document::WriteValue(std::ostringstream& os, const Json::Value& value)
    {
        Json::ValueType type = value.type();
        switch (type)
        {
        case Json::ValueType::objectValue:
            this->WriteObject(os, value);
            break;
        case Json::ValueType::arrayValue:
            this->WriteArray(os, value);
            break;
        case Json::ValueType::stringValue:
            this->WriteString(os, value);
            break;
        case Json::ValueType::intValue:
            this->WriteInt32(os, value.asInt());
            break;
        case Json::ValueType::uintValue:
            this->WriteUint32(os, value.asUInt());
            break;
        case Json::ValueType::realValue:
            this->WriteFloat(os, value.asFloat());
            break;
        case Json::ValueType::booleanValue:
            this->WriteBool(os, value.asBool());
            break;
        case Json::ValueType::nullValue:
            this->WriteNull(os);
            break;
        }
    }

    void Document::WriteObject(std::ostringstream& os, const Json::Value& obj)
    {
        this->Write(os, (uint8_t) ValueType::Object);

        int value_count = obj.size();
        this->WriteInt32(os, value_count);

        for (auto i = obj.begin(); i != obj.end(); ++i)
        {
            Json::Value key = i.key();
            this->WriteString(os, key);

            const Json::Value& value = *i;
            this->WriteValue(os, value);
        }
    }

    void Document::WriteArray(std::ostringstream& os, const Json::Value& arr)
    {
        this->Write(os, (uint8_t) ValueType::Array);

        int value_count = arr.size();
        this->WriteInt32(os, value_count);

        for (int i = 0; i < value_count; ++i)
        {
            const Json::Value& value = arr[i];
            this->WriteValue(os, value);
        }
    }

    void Document::WriteInt32(std::ostringstream& os, int32_t i)
    {
        if (i >= -32768 && i <= 32767)
        {
            if (i >= -128 && i <= 127)
            {
                this->Write(os, (uint8_t) ValueType::Int8);
                this->Write(os, (int8_t) i);
            }
            else if (i >= 128 && i <= 255)
            {
                this->Write(os, (uint8_t) ValueType::Uint8);
                this->Write(os, (uint8_t) i);
            }
            else
            {
                this->Write(os, (uint8_t) ValueType::Int16);
                this->Write(os, (int16_t) i);
            }
        }
        else if (i >= 32768 && i <= 65535)
        {
            this->Write(os, (uint8_t) ValueType::Uint16);
            this->Write(os, (uint16_t) i);
        }
        else
        {
            this->Write(os, (uint8_t) ValueType::Int32);
            this->Write(os, i);
        }
    }

    void Document::WriteUint32(std::ostringstream& os, uint32_t i)
    {
        this->Write(os, (uint8_t) ValueType::Uint32);
        this->Write(os, i);
    }

    void Document::WriteFloat(std::ostringstream& os, float f)
    {
        this->Write(os, (uint8_t) ValueType::Float);
        this->Write(os, f);
    }

    void Document::WriteString(std::ostringstream& os, const Json::Value& s)
    {
        this->Write(os, (uint8_t) ValueType::String);

        std::string str = s.asString();
        int size = str.length();

        this->WriteInt32(os, size);
        if (size > 0)
        {
            os.write(str.c_str(), size);
        }
    }

    void Document::WriteBool(std::ostringstream& os, bool b)
    {
        this->Write(os, (uint8_t) ValueType::Bool);
        this->Write(os, (int8_t) (b ? 1 : 0));
    }

    void Document::WriteNull(std::ostringstream& os)
    {
        this->Write(os, (uint8_t) ValueType::Null);
    }

    bool Document::Load(const std::string& json)
    {
        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();
        std::string errs;
        const char* begin = json.c_str();
        const char* end = begin + json.length();
        if (reader->parse(begin, end, &m_root, &errs))
        {
            std::ostringstream os;

            // serialize root to stream
            this->WriteValue(os, m_root);

            std::string str = os.str();
            const void* binary = &str[0];
            size_t size = str.size();

            if (m_binary)
            {
                free(m_binary);
                m_binary = nullptr;
            }

            m_binary_size = size;
            if (size > 0)
            {
                m_binary = malloc(size);
                memcpy(m_binary, binary, size);
            }
            
            return true;
        }
        else
        {
            return false;
        }
    }

    Json::Value Document::ReadValue(std::istringstream& is)
    {
        Json::Value value;

        ValueType type = (ValueType) this->Read<uint8_t>(is);
        switch(type)
        {
        case ValueType::Object:
            value = this->ReadObject(is);
            break;
        case ValueType::Array:
            value = this->ReadArray(is);
            break;
        case ValueType::String:
            value = this->ReadString(is);
            break;
        case ValueType::Uint8:
            value = Json::Value((int) this->Read<uint8_t>(is));
            break;
        case ValueType::Int8:
            value = Json::Value((int) this->Read<int8_t>(is));
            break;
        case ValueType::Uint16:
            value = Json::Value((int) this->Read<uint16_t>(is));
            break;
        case ValueType::Int16:
            value = Json::Value((int) this->Read<int16_t>(is));
            break;
        case ValueType::Uint32:
            value = Json::Value(this->Read<uint32_t>(is));
            break;
        case ValueType::Int32:
            value = Json::Value(this->Read<int32_t>(is));
            break;
        case ValueType::Float:
            value = Json::Value(this->Read<float>(is));
            break;
        case ValueType::Bool:
            value = Json::Value(this->Read<int8_t>(is) == 1);
            break;
        case ValueType::Null:
            value = Json::Value(Json::ValueType::nullValue);
            break;
        }

        return value;
    }

    Json::Value Document::ReadObject(std::istringstream& is)
    {
        Json::Value value(Json::ValueType::objectValue);
        
        int value_count = this->ReadValue(is).asInt();
        for (int i = 0; i < value_count; ++i)
        {
            std::string key = this->ReadValue(is).asString();
            value[key] = this->ReadValue(is);
        }

        return value;
    }

    Json::Value Document::ReadArray(std::istringstream& is)
    {
        Json::Value value(Json::ValueType::arrayValue);

        int value_count = this->ReadValue(is).asInt();
        for (int i = 0; i < value_count; ++i)
        {
            value.append(this->ReadValue(is));
        }

        return value;
    }

    Json::Value Document::ReadString(std::istringstream& is)
    {
        std::string str;
        int size = this->ReadValue(is).asInt();
        if (size > 0)
        {
            str.resize(size);
            is.read((char*) &str[0], size);
        }
        return Json::Value(str);
    }

    bool Document::Load(const void* binary, size_t size)
    {
        if (binary == nullptr || size == 0)
        {
            return false;
        }

        if (m_binary)
        {
            free(m_binary);
            m_binary = nullptr;
        }

        m_binary_size = size;
        m_binary = malloc(size);
        memcpy(m_binary, binary, size);

        // deserialize stream to root
        std::string buffer((const char*) m_binary, m_binary_size);
        std::istringstream is(buffer);
        
        m_root = this->ReadValue(is);

        return true;
    }

    std::string Document::ToJson()
    {
        return m_root.toStyledString();
    }
}

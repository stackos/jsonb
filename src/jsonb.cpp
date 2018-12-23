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
        Uint64,
        Int64,
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
            this->WriteInt64(os, value.asLargestInt());
            break;
        case Json::ValueType::uintValue:
            this->WriteUint64(os, value.asLargestUInt());
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
        this->WriteInt64(os, value_count);

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
        this->WriteInt64(os, value_count);

        for (int i = 0; i < value_count; ++i)
        {
            const Json::Value& value = arr[i];
            this->WriteValue(os, value);
        }
    }

    void Document::WriteInt64(std::ostringstream& os, int64_t i)
    {
        if (i >= -2147483648i64 && i <= 2147483647i64)
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
                this->Write(os, (int32_t) i);
            }
        }
        else if (i >= 2147483648i64 && i <= 4294967295i64)
        {
            this->Write(os, (uint8_t) ValueType::Uint32);
            this->Write(os, (uint32_t) i);
        }
        else
        {
            this->Write(os, (uint8_t) ValueType::Int64);
            this->Write(os, (int64_t) i);
        }
    }

    void Document::WriteUint64(std::ostringstream& os, uint64_t i)
    {
        this->Write(os, (uint8_t) ValueType::Uint64);
        this->Write(os, (uint64_t) i);
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
        int size = (int) str.length();

        this->WriteInt64(os, size);
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
        bool result = reader->parse(begin, end, &m_root, &errs);
        delete reader;
        return result;
    }

    void Document::ReadValue(std::istringstream& is, Json::Value& value)
    {
        ValueType type = (ValueType) this->Read<uint8_t>(is);
        switch (type)
        {
            case ValueType::Object:
                this->ReadObject(is, value);
                break;
            case ValueType::Array:
                this->ReadArray(is, value);
                break;
            case ValueType::String:
            {
                std::string str;
                this->ReadString(is, str);
                break;
            }
            case ValueType::Uint8:
            {
                Json::Value i((int) this->Read<uint8_t>(is));
                value.swapPayload(i);
                break;
            }
            case ValueType::Int8:
            {
                Json::Value i((int) this->Read<int8_t>(is));
                value.swapPayload(i);
                break;
            }
            case ValueType::Uint16:
            {
                Json::Value i((int) this->Read<uint16_t>(is));
                value.swapPayload(i);
                break;
            }
            case ValueType::Int16:
            {
                Json::Value i((int) this->Read<int16_t>(is));
                value.swapPayload(i);
                break;
            }
            case ValueType::Uint32:
            {
                Json::Value i(this->Read<uint32_t>(is));
                value.swapPayload(i);
                break;
            }
            case ValueType::Int32:
            {
                Json::Value i(this->Read<int32_t>(is));
                value.swapPayload(i);
                break;
            }
            case ValueType::Uint64:
            {
                Json::Value i(this->Read<uint64_t>(is));
                value.swapPayload(i);
                break;
            }
            case ValueType::Int64:
            {
                Json::Value i(this->Read<int64_t>(is));
                value.swapPayload(i);
                break;
            }
            case ValueType::Float:
            {
                Json::Value f(this->Read<float>(is));
                value.swapPayload(f);
                break;
            }
            case ValueType::Bool:
            {
                Json::Value b(this->Read<int8_t>(is) == 1);
                value.swapPayload(b);
                break;
            }
            case ValueType::Null:
                break;
        }
    }

    void Document::ReadObject(std::istringstream& is, Json::Value& value)
    {
        int value_count = this->ReadAsInt(is);
        for (int i = 0; i < value_count; ++i)
        {
            std::string key;
            this->Read<uint8_t>(is);
            this->ReadString(is, key);
            this->ReadValue(is, value[key]);
        }
    }

    void Document::ReadArray(std::istringstream& is, Json::Value& value)
    {
        int value_count = this->ReadAsInt(is);
        value.resize(value_count);
        for (int i = 0; i < value_count; ++i)
        {
            this->ReadValue(is, value[i]);
        }
    }

    void Document::ReadString(std::istringstream& is, std::string& str)
    {
        int size = this->ReadAsInt(is);
        str.resize(size);
        if (size > 0)
        {
            is.read((char*) &str[0], size);
        }
    }

    int Document::ReadAsInt(std::istringstream& is)
    {
        int i = 0;
        ValueType type = (ValueType) this->Read<uint8_t>(is);
        switch (type)
        {
        case ValueType::Uint8:
            i = (int) this->Read<uint8_t>(is);
            break;
        case ValueType::Int8:
            i = (int) this->Read<int8_t>(is);
            break;
        case ValueType::Uint16:
            i = (int) this->Read<uint16_t>(is);
            break;
        case ValueType::Int16:
            i = (int) this->Read<int16_t>(is);
            break;
        case ValueType::Int32:
            i = (int) this->Read<int32_t>(is);
            break;
        }
        return i;
    }

    bool Document::Load(const void* binary, size_t size)
    {
        if (binary == nullptr || size == 0)
        {
            return false;
        }

        // deserialize stream to root
        std::string buffer((const char*) binary, size);
        std::istringstream is(buffer);
        
        this->ReadValue(is, m_root);

        return true;
    }

    void Document::ToBinary()
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
    }

    std::string Document::ToJson()
    {
        return m_root.toStyledString();
    }
}

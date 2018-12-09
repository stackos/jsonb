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
#include <sstream>

namespace jsonb
{
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

            // test write
            char buff[1024] = { };
            os.write(buff, 1024);

            std::string str = os.str();
            const void* binary = &str[0];
            size_t size = str.size();

            if (m_binary)
            {
                free(m_binary);
                m_binary = nullptr;
            }

            m_binary_size = size;
            m_binary = malloc(size);
            memcpy(m_binary, binary, size);

            return true;
        }
        else
        {
            return false;
        }
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

        return true;
    }

    std::string Document::ToJson()
    {
        return m_root.toStyledString();
    }
}

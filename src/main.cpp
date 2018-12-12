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
#include <fstream>
#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("Usage:\n");
        printf("\tjsonb.exe -b input.json output.jsonb\n");
        printf("\tjsonb.exe -t input.jsonb output.json\n");
        return 0;
    }

    std::string conv = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];

    bool to_text = false;
    if (conv == "-t")
    {
        to_text = true;
    }

    std::string input_buffer;
    std::ifstream is(input, std::ios::binary | std::ios::in);
    if (is)
    {
        is.seekg(0, std::ios::end);
        size_t size = (size_t) is.tellg();
        is.seekg(std::ios::beg);

        input_buffer.resize(size);
        is.read(&input_buffer[0], size);

        is.close();
    }
    else
    {
        return 0;
    }

#define BENCHMARK 0
#if BENCHMARK
    {
        auto t = std::chrono::system_clock::now();

        if (to_text)
        {
            jsonb::Document doc;
            for (int i = 0; i < 100; ++i)
            {
                doc.Load((const void*) &input_buffer[0], input_buffer.size());
            }
        }
        else
        {
            jsonb::Document doc;
            for (int i = 0; i < 100; ++i)
            {
                doc.Load(input_buffer);
            }
        }

        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - t).count();

        printf("load time: %d ms", (int) time);

        return 0;
    }
#endif
    
    if (to_text)
    {
        jsonb::Document doc;
        if (doc.Load((const void*) &input_buffer[0], input_buffer.size()))
        {
            std::string json = doc.ToJson();

            std::ofstream os(output, std::ios::binary | std::ios::out);
            if (os)
            {
                os.write(&json[0], json.size());
                os.close();
            }
        }
    }
    else
    {
        jsonb::Document doc;
        if (doc.Load(input_buffer))
        {
            doc.ToBinary();
            const void* bin = doc.GetBinary();
            size_t bin_size = doc.GetBinarySize();

            std::ofstream os(output, std::ios::binary | std::ios::out);
            if (os)
            {
                os.write((const char*) bin, bin_size);
                os.close();
            }
        }
    }
    
    return 0;
}

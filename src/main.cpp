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

    jsonb::Document doc;
    if (to_text)
    {
        if (doc.Load((const void*) &input_buffer[0], input_buffer.size()))
        {
            std::string json = doc.ToJson();

            printf("json string: %s\n", json.c_str());
        }
    }
    else
    {
        if (doc.Load(input_buffer))
        {
            const void* bin = doc.GetBinary();
            size_t bin_size = doc.GetBinarySize();

            printf("binary size: %d\n", (int) bin_size);

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

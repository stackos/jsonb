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

int main(int argc, char* argv[])
{
    jsonb::Document doc;
    if (doc.Load("{\"key\" : 0.123}"))
    {
        const void* bin = doc.GetBinary();
        size_t bin_size = doc.GetBinarySize();

        printf("binary size: %d\n", (int) bin_size);

        if (doc.Load(bin, bin_size))
        {
            std::string json = doc.ToJson();

            printf("json string: %s\n", json.c_str());
        }
    }
    return 0;
}

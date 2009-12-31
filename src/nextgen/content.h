#ifndef NEXTGEN_CONTENT
#define NEXTGEN_CONTENT

#include "common.h"

namespace nextgen
{
    namespace content
    {
        class file_asset
        {
            struct variables
            {
                variables() : id(0), data(null_str)
                {

                }

                uint32_t id;
                std::string data;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(file_asset, variables);
        };

        class service
        {
            public: template<typename element_type> element_type get_asset(string const& name)
            {
                auto self = *this;

                if(self->asset_list.size() > 0)
                {
                    if(self->asset_list.find(name) != self->asset_list.end())
                    {
                        return self->asset_list[name];
                    }
                }

                std::ifstream f;
                f.open(name, std::ios::in | std::ios::binary);

                if(f.is_open())
                {
                    // get length of file:
                    f.seekg(0, std::ios::end);
                    size_t length = f.tellg();
                    f.seekg(0, std::ios::beg);

                    element_type e;

                    // read data as a block:
                    char data[length];
                    f.read(data, length);

                    f.close();

                    e->data = string(data);

                    return e;
                }
                else
                {
                    exit("File not open: " + name);
                }
            }

            private: struct variables
            {
                variables()
                {

                }

                hash_map<string, file_asset> asset_list;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(service, variables);
        };
    }
}

#endif

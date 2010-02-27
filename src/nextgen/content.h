#ifndef NEXTGEN_CONTENT
#define NEXTGEN_CONTENT

#include "common.h"

namespace nextgen
{
    namespace content
    {
        std::string extension_to_mimetype(std::string const& extension)
        {
            std::string e2 = extension;

            boost::to_lower(e2);
            find_and_replace(e2, ".", "");

            if(e2 == "gif")
                return "image/gif";
            else if(e2 == "png")
                return "image/png";
            else if(e2 == "html")
                return "text/html";
            else if(e2 == "swf")
                return "application/x-shockwave-flash";
            else if(e2 == "ico")
                return "image/x-icon";
            else if(e2 == "flv")
                return "flv-application/octet-stream";
            else if(e2 == "xml")
                return "application/xml";
            else if(e2 == "json")
                return "application/json";
            else
                return null_str;
        }

        class file_asset
        {
            public: std::string get_mimetype()
            {
                auto self = *this;

                return extension_to_mimetype(boost::filesystem::extension(self->path));
            }

            private: struct variables
            {
                variables() : id(0), path(null_str), data(null_str)
                {

                }

                uint32_t id;
                std::string path;
                std::string data;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(file_asset, variables);
        };

        class service
        {
            public: /*template<typename Element> Element */ file_asset get_asset(std::string const& name)
            {
                auto self = *this;

                if(self->asset_list.find(name) != self->asset_list.end())
                    return self->asset_list[name];

                if(!boost::filesystem::exists(name) || boost::filesystem::is_directory(name))
                {
                    std::cout << "[nextgen:content:service] File doesn't exist: " << name << std::endl;

                    return null;
                }

                std::ifstream f;
                f.open(name, std::ios::in | std::ios::binary);

                if(!f.is_open())
                {
                    std::cout << "[nextgen:content:service] File isn't open: " << name << std::endl;

                    return null;
                }

                file_asset e;//Element e;

                e->data.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

                f.close();

                e->path = name;

                self->asset_list[name] = e;

                return e;
            }

            private: struct variables
            {
                variables()
                {

                }

                boost::unordered_map<std::string, file_asset> asset_list;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(service, variables);
        };
    }
}

#endif

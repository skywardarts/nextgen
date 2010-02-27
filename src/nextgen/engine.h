#ifndef NEXTGEN_ENGINE_H
#define NEXTGEN_ENGINE_H

#include "nextgen/common.h"
#include "nextgen/math.h"

namespace nextgen
{
    namespace engine
    {
        struct session_base_variables
        {
            session_base_variables(network::service network_service = null, int32_t port = null_num) :
                network_service(network_service),
                server(network_service, port)
            {

            }

            network::service network_service;
            network::http_server server;
            boost::unordered_map<int32_t, event<std::function<void(network::http_message)>>> callback_list;
        };

        template<typename Self, typename Variables = session_base_variables>
        class session_base
        {
            public: typedef session_base<Self, Variables> this_type;
            public: typedef Self self_type;
            public: typedef Variables variables_type;

            public: void accept(event<std::function<void(network::http_client)>> callback2 = null)
            {
                self_type self2 = *this;
                auto self = self2;

                auto callback = callback2;

                self->server.accept([=](network::http_client c1)
                {
                    if(callback != null)
                       callback(c1);

                    self.receive(c1);
                });
            }

            public: void send(network::http_client c1, network::http_message m1, event<std::function<void(network::http_message)>> successful = null, event<std::function<void()>> failure = null) const
            {
                self_type self2 = *this;
                auto self = self2;


                int32_t http_id = self->callback_list.size();

                self->callback_list[http_id] = successful;

                m1->header_list["HTTP_ID"] = to_string(http_id);

                c1.send(m1,
                [=]
                {

                },
                [=]
                {
                    if(failure != null)
                        failure();
                    else
                        self.on_disconnect(c1);
                });
            }

            public: void receive(network::http_client c1) const
            {
                self_type self2 = *this;
                auto self = self2;

                c1.receive(
                [=](network::http_message r1)
                {
                    /*auto http_id = to_int(r1->header_list["HTTP_ID"]);

                    if(self->callback_list.find(http_id) != self->callback_list.end())
                    {
                        if(self->callback_list[http_id] != null)
                            self->callback_list[http_id](r1);
                        else
                            self.on_request(c1, r1);

                        self->callback_list.erase(http_id);
                    }
                    else
                    {
                        self.on_request(c1, r1);
                    }

                    self.receive(c1);*/
                },
                [=]
                {
                    //self.on_disconnect(c1);
                });
            }

            public: void on_request(network::http_client c1, network::http_message r1) const
            {
                self_type self = *this;
            }

            public: void on_disconnect(network::http_client c1) const
            {
                self_type self = *this;
            }

            NEXTGEN_ATTACH_SHARED_VARIABLES(session_base, variables_type);
        };

        struct basic_service_variables
        {
            basic_service_variables()
            {

            }
        };

        template<typename VariablesType = basic_service_variables>
        class basic_service
        {
            public: typedef VariablesType variables_type;

            public: void update()
            {
                auto self = *this;

            }

            NEXTGEN_ATTACH_SHARED_VARIABLES(basic_service, variables_type);
        };

        typedef basic_service<> service;

        namespace game
        {
            struct object_base_variables
            {
                typedef std::string name_type;
                typedef int32_t vector_type;
                typedef math::vector<vector_type> position_type;
                typedef math::vector<vector_type> rotation_type;

                object_base_variables() : id(null_num), name(null_str), model_name(null_str), speed(null_num)
                {

                }

                int32_t id;
                std::string name;
                position_type position;
                rotation_type rotation;
                std::string model_name;
                int32_t speed;
            };

            template<typename Variables = object_base_variables>
            class object_base
            {
                public: typedef Variables variables_type;

                NEXTGEN_ATTACH_SHARED_VARIABLES(object_base, variables_type);
            };

            struct npc_base_variables : public object_base_variables
            {
                typedef object_base_variables base_type;

                npc_base_variables() : base_type()
                {

                }
            };

            template<typename Variables = npc_base_variables>
            class npc_base : public object_base<Variables>
            {
                public: typedef Variables variables_type;
                public: typedef object_base<variables_type> base_type;

                NEXTGEN_ATTACH_SHARED_BASE(npc_base, base_type);
            };

            template<typename Variables = npc_base_variables>
            class monster_base : public npc_base<Variables>
            {
                public: typedef Variables variables_type;
                public: typedef npc_base<variables_type> base_type;

                NEXTGEN_ATTACH_SHARED_BASE(monster_base, base_type);
            };

            struct player_base_variables : public npc_base_variables
            {
                typedef npc_base_variables base_type;

                player_base_variables(network::http_client connection) : base_type(), connection(connection), latency(null_num)
                {

                }

                network::http_client connection;
                int32_t latency;
            };

            template<typename Variables = player_base_variables>
            class player_base : public npc_base<Variables>
            {
                public: typedef Variables variables_type;
                public: typedef npc_base<variables_type> base_type;

                NEXTGEN_ATTACH_SHARED_BASE(player_base, base_type);
            };

            struct world_base_variables
            {
                typedef player_base<> player_type;
                typedef object_base<> object_type;
                typedef npc_base<> npc_type;

                typedef math::vector<int> vector_type;
                typedef vector_type position_type;
                typedef vector_type rotation_type;

                world_base_variables()//network::service network_service, content::service content_service) : network_service(network_service), content_service(content_service)
                {

                }

                //content::service content_service;
                //network::service network_service;
                std::list<player_type> player_list;
                std::list<object_type> object_list;
                std::list<npc_type> npc_list;
            };

            template<typename Variables = world_base_variables>
            class world_base
            {
                public: typedef Variables variables_type;
                public: typedef typename variables_type::player_type player_type;
                public: typedef typename variables_type::object_type object_type;
                public: typedef typename variables_type::npc_type npc_type;

                public: void update()
                {

                }

                public: void add_player(player_type& p)
                {

                }

                public: void remove_player()
                {

                }

                public: void initialize()
                {
                    auto self = *this;

                    object_type bush1;

                    bush1->id = 1;
                    bush1->name = "bush1";
                    bush1->position.x(1620);
                    bush1->position.y(-4690);
                    bush1->model_name = "data/graphics/environment/bush1.png";

                    self->object_list.push_back(bush1);
                }

                NEXTGEN_ATTACH_SHARED_VARIABLES(world_base, variables_type,
                {
                    this->initialize();
                });
            };
        }

        typedef game::object_base<> game_object;
        typedef game::npc_base<> game_npc;
        typedef game::monster_base<> game_monster;
        typedef game::player_base<> game_player;
        typedef game::world_base<> game_world;

        namespace central
        {
            class basic_user
            {
                private: struct variables
                {
                    variables(network::http_client connection, int32_t id = null_num, std::string username = null_str, std::string password = null_str, std::string email = null_str) :
                        connection(connection),
                        id(id),
                        username(username),
                        password(password),
                        email(email),
                        authorization_code(null_str)
                    {

                    }

                    network::http_client connection;
                    int32_t id;
                    std::string username;
                    std::string password;
                    std::string email;
                    int32_t latency;
                    std::string authorization_code;
                };

                NEXTGEN_ATTACH_SHARED_VARIABLES(basic_user, variables);
            };

        }

        typedef central::basic_user central_user;

        namespace chat
        {

        }

    }
}

#endif

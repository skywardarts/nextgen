#ifndef NEXTGEN_DATABASE
#define NEXTGEN_DATABASE

#include "common.h"

namespace nextgen
{
    namespace database
    {
        //typedef boost::shared_ptr<boost::unordered_map<std::string, std::string>> row;
        //typedef boost::shared_ptr<std::vector<row>> row_list;
/*
        class basic_row
        {
            public: std::string to_xml() const
            {
                auto self = *this;

                std::stringstream is;

                boost::property_tree::write_xml(is, self->tree);

                std::string is_str = is.str();

                find_and_replace(is_str, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n", "");

                return is_str;
            }

            public: template<typename ReturnType> ReturnType& get(std::string const& name) const
            {
                auto self = *this;

                return self->tree.get<ReturnType>(name);
            }

            public: template<typename ArgumentType> void set(std::string const& name, ArgumentType& value) const
            {
                auto self = *this;

                self->tree.set(name, value);
            }

            private: struct variables
            {
                variables()
                {

                }

                //boost::unorder_map<std::string, boost::any> items;
                boost::property_tree::iptree tree;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(basic_row, variables);
        };

        class basic_row_list
        {
            public: typedef boost::property_tree::iptree row_type;

            public: std::string to_xml() const
            {
                auto self = *this;

                return std::string(""); // todo(daemn)
            }

            public: size

            public: void add(row_type& row) const
            {
                auto self = *this;

                self->items.push_back(row);
            }

            public: row_type get(uint32_t index) const
            {
                auto self = *this;

                return self->items[index];
            }

            public: row_type operator[](uint32_t index) const
            {
                auto self = *this;

                return self.get(index);
            }

            private: struct variables
            {
                variables()
                {

                }

                std::vector<row_type> items;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(basic_row_list, variables);
        };*/


        class basic_result // todo(daemn)
        {
            public: typedef boost::property_tree::iptree row_type;
            public: typedef std::vector<row_type> row_list_type;

            public: uint32_t get_insert_id() const
            {
                auto self = *this;

                return self->insert_id;
            }

            public: uint32_t get_affected_rows() const
            {
                auto self = *this;

                return self->affected_rows;
            }

            public: uint32_t get_row_count() const
            {
                auto self = *this;

                return self->items.size();
            }

            public: row_type get_row() const
            {
                auto self = *this;

                if(self->items.size() == 0)
                    std::cout << "[nextgen:database:link] No row result exists." << std::endl;

                return self->items[0];
            }

            public: row_list_type get_rows() const
            {
                auto self = *this;

                return self->items;
            }

            private: struct variables
            {


                variables() : insert_id(null_num), affected_rows(null_num)
                {

                }

                row_list_type items;
                int32_t insert_id;
                int32_t affected_rows;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(basic_result, variables);
        };

        class basic_link
        {
            public: typedef basic_result::row_type row_type;
            public: typedef basic_result::row_list_type row_list_type;
            public: typedef basic_result result_type;

            public: uint32_t get_last_insert_id() const
            {
                auto self = *this;

                return self->last_insert_id;
            }

            public: uint32_t get_last_affected_rows() const
            {
                auto self = *this;

                return self->last_affected_rows;
            }

            public: result_type execute(std::string const& query) const
            {
                auto self = *this;

                result_type r;

                if(self->connected == null_str)
                {
                    std::cout << "[nextgen:database:link] Not connected." << std::endl;

                    return null;
                }

                MYSQL_RES* result;
                MYSQL_ROW row;

                int status = mysql_query(self->link, query.c_str());

                if(status)
                {
                    std::cout << "[nextgen:database:link] Could not execute statement(s)" << std::endl;
                    std::cout << mysql_error(self->link) << std::endl;

                    mysql_close(self->link);

                    return null;
                }

                MYSQL_FIELD *field;
                unsigned int num_fields;
                unsigned int i;
                unsigned long *lengths;

                std::vector<std::string> fields;

                do
                {
                    result = mysql_store_result(self->link);

                    if(result)
                    {
                        //process_result_set(mysql, result);

                        num_fields = mysql_num_fields(result);

                        for(i = 0; i < num_fields; i++)
                        {
                            field = mysql_fetch_field(result);
                            fields.push_back(field->name);
                        }

                        while((row = mysql_fetch_row(result)))
                        {
                           lengths = mysql_fetch_lengths(result);

                           row_type hash;

                           for(i = 0; i < num_fields; i++)
                           {
                               if(row[i])
                                    hash.put(fields[i], row[i]);
                               else
                                    hash.put(fields[i], "NULL");
                           }

                           r->items.push_back(hash);
                        }

                        mysql_free_result(result);
                    }
                    else
                    {
                        if(mysql_field_count(self->link) == 0)
                        {
                            uint32_t affected_rows = mysql_affected_rows(self->link);

                            self->last_affected_rows = affected_rows;
                            r->affected_rows = affected_rows;

                            uint32_t insert_id;

                            if((insert_id = mysql_insert_id(self->link)) != 0)
                            {
                                self->last_insert_id = insert_id;
                                r->insert_id = insert_id;
                            }
                        }
                        else
                        {
                            printf("Could not retrieve result set\n");

                            break;
                        }
                    }

                    if((status = mysql_next_result(self->link)) > 0)
                        printf("Could not execute statement\n");

                } while (status == 0);

                return r;
            }

            public: void connect(std::string const& host, std::string const& username, std::string const& password, std::string const& database = "") const
            {
                auto self = *this;

                if(self->connected != null_str && self->connected != database)
                    self.disconnect();

                self->link = mysql_init(NULL);

                if(!self->link)
                {
                    std::cout << "[nextgen:database:link] MySQL init error" << std::endl;

                    return;
                }

                // Connect to MySQL.
                if(mysql_real_connect(self->link, host.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, NULL, 0) == NULL)
                {
                    std::cout << "[nextgen:database:link] Error connecting to " << database.c_str() << "." << std::endl;

                    std::cout << "[mysql] " << mysql_error(NULL) << std::endl;

                    mysql_close(self->link);
                    mysql_library_end();

                    return;
                }

                self->connected = database;
            }

            public: void disconnect() const
            {
                auto self = *this;

                mysql_close(self->link);

                self->link = (MYSQL*)NULL;

                self->connected = null_str;
            }

            private: struct variables
            {
                variables() : connected(null_str), last_insert_id(null_num), last_affected_rows(null_num)
                {

                }

                MYSQL* link;
                std::string connected;
                int32_t last_insert_id;
                int32_t last_affected_rows;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(basic_link, variables);
        };

        class basic_query
        {
            public: typedef basic_link link_type;
            public: typedef basic_result result_type;

            public: result_type execute(link_type link, std::vector<std::string>&& arguments) const
            {
                auto self = *this;

                if(time(0) > self->timestamp + self->delay)
                {
                    std::string q = self->query;

                    for(uint32_t i = 0, l = arguments.size(); i < l; ++i)
                        find_and_replace(q, "$" + to_string(i+1), arguments[i]);

                    self->result = link.execute(q);

                    self->timestamp = time(0);
                }
                else
                {
                    std::cout << "[nextgen:database:query] Too soon, try again later." << std::endl;
                }

                return self->result;
            }

            private: struct variables
            {
                variables(std::string query = null_str, int32_t delay = 1) : query(query), delay(delay), timestamp(0), result(null)
                {

                }

                std::string query;
                int32_t delay;
                int32_t timestamp;
                result_type result;
            };

            NEXTGEN_ATTACH_SHARED_VARIABLES(basic_query, variables);
        };

        typedef basic_result::row_type row;
        typedef basic_result::row_list_type row_list;
        typedef basic_result result;

        typedef basic_query query;
        typedef basic_link link;
    }



}

#endif

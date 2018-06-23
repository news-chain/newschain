//
// Created by boy on 18-6-19.
//
#pragma once


#include <app/application.hpp>
#include <boost/program_options.hpp>
#include <fc/io/json.hpp>
#include <fc/io/fstream.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/variant.hpp>
#include <chainbase/chainbase.hpp>

#define NEWS_JSON_RPC_PLUGIN_NAME   ("json_rpc")


#define JSON_RPC_REGISTER_API( API_NAME )                                                       \
{                                                                                               \
   news::plugins::json_rpc::detail::register_api_method_visitor vtor( API_NAME );              \
   for_each_api( vtor );                                                                        \
}




using namespace boost::program_options;


/**
 * @brief Internal type used to bind api methods
 * to names.
 *
 * Arguments: Variant object of propert arg type
 */
typedef std::function< fc::variant(const fc::variant&) > api_method;

/**
 * @brief An API, containing APIs and Methods
 *
 * An API is composed of several calls, where each call has a
 * name defined by the API class. The api_call functions
 * are compile time bindings of names to methods.
 */
typedef std::map< std::string, api_method > api_description;



#define JSON_RPC_PARSE_ERROR        (-32700)
#define JSON_RPC_INVALID_REQUEST    (-32600)
#define JSON_RPC_METHOD_NOT_FOUND   (-32601)
#define JSON_RPC_INVALID_PARAMS     (-32602)
#define JSON_RPC_INTERNAL_ERROR     (-32603)
#define JSON_RPC_SERVER_ERROR       (-32000)
#define JSON_RPC_NO_PARAMS          (-32001)
#define JSON_RPC_PARSE_PARAMS_ERROR (-32002)
#define JSON_RPC_ERROR_DURING_CALL  (-32003)



namespace news{
    namespace plugins{
        namespace json_rpc{

            namespace detail{
                class json_rpc_plugin_impl;
            }


            struct api_method_signature
            {
                fc::variant args;
                fc::variant ret;
            };

            class json_rpc_plugin : public news::app::plugin< json_rpc_plugin >
            {
            public:
                json_rpc_plugin();
                virtual ~json_rpc_plugin();

//                APPBASE_PLUGIN_REQUIRES( (plugins::json_rpc::json_rpc_plugin) );
                NEWSAPP_PLUGIN_REQUIRES();


                static const std::string& name() { static std::string name = NEWS_JSON_RPC_PLUGIN_NAME; return name; }

                virtual void set_program_options(options_description&, options_description& cfg) override;


                void add_api_method(const std::string &api_name, const std::string &method_name, const api_method &api, const api_method_signature &sig);

                std::string call(const std::string &body);
            protected:
                virtual void plugin_initialize(const variables_map& options) override;
                virtual void plugin_startup() override;
                virtual void plugin_shutdown() override;

            private:
                std::unique_ptr< detail::json_rpc_plugin_impl > my;
            };



            namespace detail {

                class register_api_method_visitor
                {
                public:
                    register_api_method_visitor( const std::string& api_name )
                            : _api_name( api_name ),
                              _json_rpc_plugin( news::app::application::getInstance().get_plugin< news::plugins::json_rpc::json_rpc_plugin >() )
                    {}

                    template< typename Plugin, typename Method, typename Args, typename Ret >
                    void operator()(
                            Plugin& plugin,
                            const std::string& method_name,
                            Method method,
                            Args* args,
                            Ret* ret )
                    {
                        _json_rpc_plugin.add_api_method( _api_name, method_name,
                                                         [&plugin,method]( const fc::variant& args ) -> fc::variant
                                                         {
//                                                             ddump((args));
                                                             return fc::variant( (plugin.*method)( args.as< Args >(), true ) );
                                                         },
                                                         api_method_signature{ fc::variant( Args() ), fc::variant( Ret() ) } );
                    }

                private:
                    std::string _api_name;
                    news::plugins::json_rpc::json_rpc_plugin& _json_rpc_plugin;
                };

            }


        }//news::plugins::json_rpc
    }//news::plugins
}//news


FC_REFLECT( news::plugins::json_rpc::api_method_signature, (args)(ret) )
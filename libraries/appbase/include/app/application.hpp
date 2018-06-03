//
// Created by boy on 18-5-30.
//

#pragma once


#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <memory>
#include <string>
#include <iostream>
#include "plugin.hpp"



namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;


namespace news{
    namespace app{


        class application{
        public:
            ~application();

            //only one in global
            static application &getInstance();

            /*
             *  set options
             **/
            void set_program_options();

            /*
             * add options desc
             * */
            void add_program_options(const bpo::option_description &cli, const bpo::option_description cfg);

            /*
             *   init args
             * */
            template < typename... Plugins>
            bool initizlize(int argc, char **argv){
                return initialize_impl(argc, argv,  {find_plugin(Plugins::name())...} );
            }


            void start_up();



            /*
             *      add plugin
             * */
            template <typename Plugin>
            auto &register_plugin(){
                auto plug = std::make_shared<Plugin>();
                _plugins[Plugin::name()] = plug;
                plug->register_dependencies();
                return *plug;
            }

            /*
             *      find plugin
             * */
            template <typename Plugin>
            Plugin* find_plugin() const;

            /*
             *      init plugin
             * */
            void plugin_init(abstract_plugin &plugin);

            /*
             *      start plugin
             * */
            void plugin_started(abstract_plugin &plugin);


            /*
             *      get boost::asio::io_service
             * */
            boost::asio::io_service &get_io_service();


            /*
             *      get args
             * */
            const bpo::variables_map &get_args() const;

            /*
             *      io_serv stop
             * */
            void quit();

            /*
             *      exec
             * */
            void exec();

            /*
             *  shutdown plugins
             * */
            void shutdown();

        protected:
            bool initialize_impl(int argc, char **argv, std::vector< abstract_plugin* > autostart_plugins);

            abstract_plugin* find_plugin(const std::string &name) const;
            abstract_plugin* get_plugin(const std::string &name) const;

        private:
            application();
            void write_default_config(const bfs::path &path);

            std::map<std::string, std::shared_ptr< abstract_plugin > >   _plugins;
            std::vector< abstract_plugin *>                              _initialized_plugins;
            std::vector< abstract_plugin *>                              _running_plugins;


            std::shared_ptr< boost::asio::io_service> io_serv;
            std::unique_ptr< class application_impl > my;




        };




        ////////////////////////////////////////////////////////////
        /////////////////     plugin     ///////////////////////////
        ////////////////////////////////////////////////////////////

        template < typename Impl>
        class plugin : public abstract_plugin{
        public:
            virtual ~plugin() {}

            virtual state get_state() const override { return _state; }
            virtual const std::string& get_name()const override final { return Impl::name(); }

            virtual void register_dependencies()
            {
                this->plugin_for_each_dependency( [&]( abstract_plugin& plug ){} );
            }

            virtual void initialize(const variables_map& options) override final
            {
                if( _state == registered )
                {
                    _state = initialized;
                    this->plugin_for_each_dependency( [&]( abstract_plugin& plug ){ plug.initialize( options ); } );
                    this->plugin_initialize( options );
                    // std::cout << "Initializing plugin " << Impl::name() << std::endl;
                    application::getInstance().plugin_init( *this );
                }
                if (_state != initialized)
                    BOOST_THROW_EXCEPTION( std::runtime_error("Initial state was not registered, so final state cannot be initialized.") );
            }

            virtual void startup() override final
            {
                if( _state == initialized )
                {
                    _state = started;
                    this->plugin_for_each_dependency( [&]( abstract_plugin& plug ){ plug.startup(); } );
                    this->plugin_startup();
                    application::getInstance().plugin_started( *this );
                }
                if (_state != started )
                    BOOST_THROW_EXCEPTION( std::runtime_error("Initial state was not initialized, so final state cannot be started.") );
            }

            virtual void shutdown() override final
            {
                if( _state == started )
                {
                    _state = stopped;
                    //ilog( "shutting down plugin ${name}", ("name",name()) );
                    this->plugin_shutdown();
                }
            }

        protected:
            plugin() = default;

        private:
            state _state = abstract_plugin::registered;
        };


    }//namespace app
} //namespace news
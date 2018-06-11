//
// Created by boy on 18-5-30.
//

#include <app/application.hpp>
#include <fc/log/logger_config.hpp>


namespace news{
    namespace app{

        class application_impl{
        public:
            application_impl(){

            }
            bpo::options_description    _app_options;
            bpo::options_description    _cfg_options;

            bpo::variables_map          _map_args;



        private:
            boost::program_options::variables_map _options;


        };

        application::application()
                :my(new application_impl()) {
            io_serv = std::make_shared<boost::asio::io_service>();
        }


        application::~application() {

        }


        application& application::getInstance() {
            static application app;
            return app;
        }




        void application::start_up() {
            for(const auto & plugin : _initialized_plugins){
                plugin->startup();
            }

        }

//        template <typename Plugin>
//        auto& application::register_plugin() {
//            auto plug = std::make_shared<Plugin>();
//            _plugins[Plugin::name()] = plug;
//            plug->register_dependencies();
//            return *plug;
//        }



        /////////////////////////////////
        /////////////plugin//////////////
        /////////////////////////////////

        template <typename Plugin>
        Plugin* application::find_plugin() const {

        }

        void application::plugin_init(news::app::abstract_plugin &plugin) {
            _initialized_plugins.push_back(&plugin);
        }


        void application::plugin_started(news::app::abstract_plugin &plugin) {
            _running_plugins.push_back(&plugin);
        }


        boost::asio::io_service& application::get_io_service() {
            return *io_serv;
        }


        abstract_plugin* application::find_plugin(const std::string &name) const {
            auto itr = _plugins.find(name);
            if(itr == _plugins.end()){
                return nullptr;
            }
            return itr->second.get();
        }

        abstract_plugin* application::get_plugin(const std::string &name) const {
            auto itr = _plugins.find(name);
            if(itr == _plugins.end()){
                BOOST_THROW_EXCEPTION(std::runtime_error("unable to get plugin:" + name));
            }
            return itr->second.get();
        }






        void application::add_program_options(const bpo::option_description &cli, const bpo::option_description cfg) {

        }


        void application::set_program_options() {


            bpo::options_description options_desc("Application config");

            options_desc.add_options()
                    ("help,h", "print help message")
                    ("data-dir,d", bpo::value<boost::filesystem::path>()->default_value("node_data"), "database and config --directory")
//                    ("config-dir,c", bpo::value<bfs::path>()->default_value("config.ini"), "config.ini path")
//                    ("config-log", bpo::value<bfs::path>()->default_value("config_log.ini"), "config logs level")
                    ("version,v", "print version information");

            my->_app_options.add(options_desc);


            for(auto p : _plugins){
                bpo::options_description plugin_cli_option("Command line option for " + p.second->get_name());
                bpo::options_description plugin_cfg_option("Config options for" + p.second->get_name());
                p.second->set_program_options(plugin_cli_option, plugin_cfg_option);
                if(plugin_cfg_option.options().size()){
                    my->_app_options.add(plugin_cfg_option);
                    my->_cfg_options.add(plugin_cfg_option);
                }

                if(plugin_cfg_option.options().size()){
                    my->_app_options.add(plugin_cli_option);
                }
            }

        }


        bool application::initialize_impl(int argc, char **argv, std::vector< abstract_plugin* > autostart_plugins) {
//            for(auto itr : autostart_plugins){
//                std::cout << "initplugins " << itr->get_name() << std::endl;
//            }

            try {
                set_program_options();

                bpo::store(bpo::parse_command_line(argc, argv, my->_app_options), my->_map_args);

                if(my->_map_args.count("help")){
                    std::cout << my->_app_options << std::endl;
                    return false;
                }
                if(my->_map_args.count("version")){
                    std::cout << "version 0.0.0" << std::endl;
                }


                bfs::path data_dir;
                if(my->_map_args.count("data-dir")){
                    data_dir = my->_map_args["data-dir"].as<bfs::path>();
                    if(data_dir.is_relative()){
                        data_dir = bfs::current_path() / data_dir;
                    }
                }
                else{     //create
                    //TODO : WIN32 or linux
                    data_dir = bfs::current_path() / "node_data";
                }

                if(!bfs::exists(data_dir)){
                    write_default_config(data_dir / "config.ini");
                }



                fc::logging_config config = fc::logging_config::default_config();


            }catch (boost::program_options::error &e){
                std::cout << e.what() << std::endl;
                return false;
            }

            for(const auto &plugin : autostart_plugins){
                if(plugin != nullptr && plugin->get_state() == abstract_plugin::registered){
                    plugin->initialize(my->_map_args);
                }
            }




            return true;
        }



        const bpo::variables_map& application::get_args() const {
            return my->_map_args;
        }


        void application::write_default_config(const bfs::path &path) {
            if(!bfs::exists(path.parent_path())){
                bfs::create_directories(path.parent_path());
                std::cout << "create data-dir: " << path << std::endl;
            }

            std::ofstream out_cfg( bfs::path(path).make_preferred().string());
            for(const boost::shared_ptr<bpo::option_description> od : my->_cfg_options.options())
            {
                if(!od->description().empty())
                    out_cfg << "# " << od->description() << "\n";
                boost::any store;
                if(!od->semantic()->apply_default(store))
                    out_cfg << "# " << od->long_name() << " = \n";
                else
                {
                    auto example = od->format_parameter();
                    if( example.empty() )
                    {
                        // This is a boolean switch
                        out_cfg << od->long_name() << " = " << "false\n";
                    }
                    else if( example.length() <= 7 )
                    {
                        // The string is formatted "arg"
                        out_cfg << "# " << od->long_name() << " = \n";
                    }
                    else
                    {
                        // The string is formatted "arg (=<interesting part>)"
                        example.erase(0, 6);
                        example.erase(example.length()-1);
                        out_cfg << od->long_name() << " = " << example << "\n";
                    }
                }
                out_cfg << "\n";
            }
            out_cfg.close();
        }


        void application::quit() {
            io_serv->stop();
        }

        void application::exec() {
            signal(SIGPIPE, SIG_IGN);

            std::shared_ptr<boost::asio::signal_set> sigint_set(new boost::asio::signal_set(*io_serv, SIGINT));
            sigint_set->async_wait([sigint_set,this](const boost::system::error_code& err, int num) {
                quit();
                sigint_set->cancel();
            });

            std::shared_ptr<boost::asio::signal_set> sigterm_set(new boost::asio::signal_set(*io_serv, SIGTERM));
            sigterm_set->async_wait([sigterm_set,this](const boost::system::error_code& err, int num) {
                quit();
                sigterm_set->cancel();
            });
            io_serv->run();
            shutdown();



        }


        void application::shutdown() {
            for(auto itr = _running_plugins.rbegin(); itr != _running_plugins.rend(); itr++){
                (*itr)->shutdown();
            }

            for(auto itr = _running_plugins.rbegin(); itr != _running_plugins.rend(); itr++){
                _plugins.erase((*itr)->get_name());
            }

            _running_plugins.clear();
            _initialized_plugins.clear();
            _plugins.clear();
        }


    }//namespace app
}//namespace news
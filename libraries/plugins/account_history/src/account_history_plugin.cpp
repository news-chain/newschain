//
// Created by boy on 18-7-3.
//



#include <news/plugins/account_history/account_history_plugin.hpp>


namespace news{
    namespace plugins{
        namespace account_history_plugin{



            using namespace news::chain;
            namespace detail{
                class account_history_impl{
                public:
                    account_history_impl(news::chain::database &d):_db(d){}


                    void on_pre_apply_operation(const news::base::operation_notification &note);

                    news::chain::database &_db;
                };







                ////////////////////////////////////////////////////////////////////////
                ////////////////////////////////////////////////////////////////////////
                void account_history_impl::on_pre_apply_operation(const news::base::operation_notification &note) {

                    auto new_obj = _db.create<operation_object>([note, this](operation_object &obj){
                        obj.trx_id = note.trx_id;
                        obj.block = note.block;
                        obj.trx_in_block = note.trx_in_block;
                        obj.timestamp = _db.head_block_time();
                        auto size = fc::raw::pack_size(note.op);
                        obj.op_packed.resize(size);
                        fc::datastream<char*> ds(obj.op_packed.data(), size);

                        fc::raw::pack(ds, note.op);
                    });


//                    account_name  nn;
//                    const auto history_index =  _db.get_index<account_hsitory_obj_index>().indices().get<by_account>();
//                    auto itr = history_index.lower_bound(boost::make_tunlp());
//                    if(history_index != )
//                    _db.create<account_history_object>([](account_history_object &obj){
//                        //TODO
//
//                    });

                }
            }















            account_history_plugin::account_history_plugin():_my(new detail::account_history_impl(news::app::application::getInstance().get_plugin<news::plugins::chain_plugin::chain_plugin>().get_database())) {





//              how to work ?
//            _my->_db.add_pre_apply_operation_handler(boost::bind(&detail::account_history_impl::on_pre_apply_operation, _my.get()), *this);
            }

            account_history_plugin::~account_history_plugin() {

            }

            void account_history_plugin::set_program_options(options_description &, options_description &cfg) {

            }

            void account_history_plugin::plugin_initialize(const variables_map &options) {

            }

            void account_history_plugin::plugin_startup() {
                _my->_db.add_post_apply_operation_handler([this](const news::base::operation_notification &note){
                    _my->on_pre_apply_operation(note);
                }, *this);
            }

            void account_history_plugin::plugin_shutdown() {

            }
        }
    }
}
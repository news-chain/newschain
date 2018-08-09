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

                    flat_set<account_name> _names;
                    get_operation_names_visitor visitor(_names);
                    note.op.visit(visitor);

                    const auto& new_obj = _db.create<operation_object>([note, this](operation_object &obj){
                        obj.trx_id = note.trx_id;
                        obj.block = note.block;
                        obj.trx_in_block = note.trx_in_block;
                        obj.timestamp = _db.head_block_time();
                        auto size = fc::raw::pack_size(note.op);
                        obj.op_packed.resize(size);
                        fc::datastream<char*> ds(obj.op_packed.data(), size);

                        fc::raw::pack(ds, note.op);
                    });


                    const auto &history_index = _db.get_index<account_history_obj_index>().indices().get<by_account>();

                    for(const auto &name : _names){
                        uint32_t seq = 1;
                        auto itr = history_index.lower_bound(boost::make_tuple(name, UINT32_MAX));
                        if(itr != history_index.end() && itr->name == name){
                            seq = itr->sequence + 1;
                        }
                        _db.create<account_history_object>([&](account_history_object &obj){
                            obj.name = name;
                            obj.sequence = seq;
                            obj.op_id = new_obj.id;
                        });
                    }


                }
            }















            account_history_plugin::account_history_plugin():_my(new detail::account_history_impl(news::app::application::getInstance().get_plugin<news::plugins::chain_plugin::chain_plugin>().get_database())) {
            }

            account_history_plugin::~account_history_plugin() {

            }

            void account_history_plugin::set_program_options(options_description &, options_description &cfg) {

            }

            void account_history_plugin::plugin_initialize(const variables_map &options) {

            }

            void account_history_plugin::plugin_startup() {
                _my->_db.add_post_apply_operation_handler([&](const news::base::operation_notification &note){
                    _my->on_pre_apply_operation(note);
                }, *this, 0);
            }

            void account_history_plugin::plugin_shutdown() {

            }
        }
    }
}
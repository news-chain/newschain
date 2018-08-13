//
// Created by boy on 18-7-31.
//

#include <test/record.hpp>
#include <fc/io/json.hpp>


namespace test {


    std::string variable_to_json(const std::string &trx, uint64_t id) {
        auto json =
                "{\"jsonrpc\":\"2.0\",\"params\":[\"chain_api\",\"push_transaction\"," + trx +
                "],\"id\":" + std::to_string(id) + ",\"method\":\"call\"}";
        return json;
    }


    record::record() : _id(1) {

    }


    std::vector<std::string> record::add_record_data(std::vector<signed_transaction> &vec) {
        std::vector<std::string> data;

        for (auto &trx : vec) {
            auto json = variable_to_json(fc::json::to_string(trx), _id);
            data.emplace_back(std::move(json));
            _id++;
        }


        return data;
    }

    void record::add_send_data(uint64_t id, tools::get_context cxt) {
        if(id == 0 ){
            return;
        }
        if (_data.find(id) == _data.end()) {
            _data[id] = cxt;
        } else {
            elog("add_send_data id is exited.");
        }
    }

    bool record::update_data(uint64_t id, tools::get_context cxt) {
        if(id == 0){
            return true;
        }
        if (_data.find(id) != _data.end()) {
            _data[id].get_time = cxt.get_time;
            _data[id].ret = cxt.ret;
            return true;
        } else {
            elog("not find data id ${id}", ("id", id));
        }

        return false;
    }

    void record::log_data_and_move() {
        int64_t max_request_time = 0;
        int64_t min_request_time = 0;
        uint32_t send_count = _data.size();
        uint32_t success = 0;
        uint32_t failed = 0;
        uint64_t average = 0;
        uint64_t all_time = 0;
        uint32_t not_receive = 0;
        uint32_t time_out = 0;


//        auto data = std::move(_data);
        std::vector<uint64_t> remove_data;

        fc::time_point now = fc::time_point::now();
        for (auto &dd : _data) {
            auto spend = (dd.second.get_time - dd.second.send_time).count();
            if(spend > 0){
                max_request_time = max_request_time < spend ? spend : max_request_time;
                min_request_time = min_request_time > spend ? spend : min_request_time;
                all_time += spend;
            }

            if(dd.second.ret.jsonrpc.length() == 0 || dd.second.ret.id == 0){
                not_receive++;
                if(now - dd.second.send_time > fc::minutes(1)){
                    time_out++;
                    remove_data.push_back(dd.first);
                }
                continue;
            }
            else if (dd.second.ret.id != 0 && dd.second.ret.error.valid() ) {
//                if(failed % 10 == 0){
                    elog("error ${e}", ("e", dd.second.ret));
//                }
                failed++;
                if(failed * 1.0 / (send_count * 1.0) > 0.1){
//                    elog("error ${e}", ("e", dd.second.ret));
//                    std::cerr << "failed too many." << std::endl;
//                    exit(2);
                }
                remove_data.push_back(dd.first);
            } else {
                success++;
                remove_data.push_back(dd.first);
            }


        }
        send_count = not_receive + failed + success;
        for(auto &id : remove_data){
            _data.erase(id);
        }

        if(send_count == 0){
            elog("not send any message.");
            return;
        }
        average = all_time / send_count;


        std::cout << "*************************************************" << std::endl;
        std::cout << "time :            " << std::string(fc::time_point::now()) << std::endl;
        std::cout << "max_request_time: " << max_request_time / 1000 << "ms" << std::endl;
        std::cout << "min_request_time: " << min_request_time / 1000 << "ms" << std::endl;
        std::cout << "send_count:       " << send_count << std::endl;
        std::cout << "success:          " << success << std::endl;
        std::cout << "failed:           " << failed << std::endl;
        std::cout << "not_receive:      " << not_receive << std::endl;
        std::cout << "time_out:         " << time_out << std::endl;
        std::cout << "average:          " << average / 1000 << "ms" << std::endl;
        std::cout << "*************************************************" << std::endl;


        if(failed * 1.0 / (send_count * 1.0) > 0.1){
            std::cerr << "failed too many." << std::endl;
            exit(2);
        }
        if(time_out * 1.0 / (send_count * 1.0) > 0.1){
            std::cerr << "time_out too many" << std::endl;
            exit(3);
        }
    }

    void record::stop() {
        log_data_and_move();
    }
}
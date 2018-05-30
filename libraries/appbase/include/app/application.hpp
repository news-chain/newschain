//
// Created by boy on 18-5-30.
//

#pragma once

#include <memory>

namespace news{
    namespace app{

        class application_impl;



        class application{
        public:
            ~application();

            //only one in global
            static application &getInstance();






        private:
            application();

            std::unique_ptr< application_impl > my;

        };







    }//namespace app
} //namespace news
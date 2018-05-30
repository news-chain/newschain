//
// Created by boy on 18-5-30.
//

#include <app/application.hpp>



namespace news{
    namespace app{
        class application_impl{
        public:
            application_impl(){

            }
        };









        application::application():my(new application_impl) {

        }


        application::~application() {

        }


        application& application::getInstance() {
            static application app;
            return app;
        }




    }//namespace app
}//namespace news
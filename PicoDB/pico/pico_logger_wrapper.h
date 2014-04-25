//
//  pico_logger_wrapper.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/25/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_pico_logger_wrapper_h
#define PicoDB_pico_logger_wrapper_h
#include <pico/pico_utils.h>
#include <logger.h>


namespace pico
{
    class pico_logger_wrapper {
    std::shared_ptr<logger> instance;
    public:
        pico_logger_wrapper():instance(new logger("gicapods")){
        
        }

        template<typename T>
        pico_logger_wrapper& operator << (T nonstr)
        {
            string str = convertToString<T>(nonstr);
            log(str);
            return *this;
        }
        template<>
        pico_logger_wrapper& operator << (const std::string& str)
        {
            log(str);
            return *this;
        }
        
//
        void log(const std::string& str){
            
            instance->log(str);
        }
    
    };

}


#endif

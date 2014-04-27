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
    
    public:
    logger mylogger;
        pico_logger_wrapper():mylogger("gicapods"){
        
        }
        void log(const std::string& str){
            mylogger<<str;
        }
    
    };
 
}


#endif

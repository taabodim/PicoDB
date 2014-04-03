//
//  Logger.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/2/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_Logger_h
#define PicoDB_Logger_h
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

namespace pico{
    
class logger{
    
public:
    
    static std::shared_ptr<boost::mutex> log_mutex;
    
    void log(const std::string& str)
    {
        boost::interprocess::scoped_lock<boost::mutex> lock( *log_mutex);
        std::cout<<str<<std::endl;
    }
    
    logger(){
        if(log_mutex==nullptr)
        {
            std::cout<<"logger : initiliazing the mutext for the logger\n";
            std::shared_ptr<boost::mutex> temp(new boost::mutex());
            log_mutex=temp;
        }
        std::cout<<"logger instance created...\n";};
    logger operator=(logger& r){
        std::cout<<"assignment operator logger instance called..\n";
        return r;};
    private :
    
    logger(logger& l){};
    
  
    
};
}

#endif

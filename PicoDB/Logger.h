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
#include <boost/any.hpp>
using namespace std;
namespace pico{
    
class logger{
    
public:
    
    static std::shared_ptr<boost::mutex> log_mutex;
    void log(const std::string& str )
    {
        boost::interprocess::scoped_lock<boost::mutex> lock( *log_mutex);
        std::cout<<str <<"\n ";
        
    }
    void log(const std::string& str,const std::string& str1 )
    {
        boost::interprocess::scoped_lock<boost::mutex> lock( *log_mutex);
        std::cout<<str <<"  "<<str1<<endl;
        
    }
    void log(const std::string& str,const std::string& str1,const std::string& str2 )
    {
        boost::interprocess::scoped_lock<boost::mutex> lock( *log_mutex);
        std::cout<<str <<"  "<<str1<<" "<<str2<<endl;
        
    }
    
//    void log(const std::string& str)
//    {
//        boost::interprocess::scoped_lock<boost::mutex> lock( *log_mutex);
//        std::cout<<str<<std::endl;
//    }
    
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

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
#include <fstream>
#include <iostream>
#include <memory>
#include <pico_utils.h>
using namespace std;
namespace pico{
    
    class logger  {
        private :
        std::ofstream outfile;
        boost::mutex log_mutex;
       
        void append(std::string str)
        {
            outfile.write((char*) str.c_str(), str.size());
            outfile.flush();
        }
     
public:
   
        void log(std::string str )
        {
            while (true)
            {
            boost::interprocess::scoped_lock<boost::mutex> lock(log_mutex,boost::interprocess::try_to_lock);
            if(lock)
            {
                append(str);
                break;
            }
            }
            
        }
       logger(std::string filename){
        
        string path("/Users/mahmoudtaabodi/Documents/");
		std::string ext(".log");
		path.append(filename);
        path.append(ext);
       outfile.open(path,
                     std::fstream::out | std::fstream::app | std::fstream::binary);
     }
   
        logger()=delete;
    
    
};
    
    template<typename T>
    logger& operator << (logger& wrapper,T nonstr)
    {
        string str = convertToString<T>(nonstr);
       // std::cout<<str;
        wrapper.log(str);
        return wrapper;
    }
    template<>
    logger& operator << (logger& wrapper,const std::string& str)
    {
        //std::cout<<str;
        wrapper.log(str);
        return wrapper;
    }

}

#endif

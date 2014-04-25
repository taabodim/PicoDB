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
using namespace std;
namespace pico{
    
    class logger : public std::enable_shared_from_this<logger> {
        
public:
    std::ofstream outfile;
    boost::mutex log_mutex;
   

    void log(std::string str )
    {
        boost::interprocess::scoped_lock<boost::mutex> lock(log_mutex);
        str.append("\n");
        append(str);
        
    }
    void append(std::string str)
    {
        outfile.write((char*) str.c_str(), str.size());
        outfile.flush();
    }
    
      std::shared_ptr<logger> operator <<(const std::string& str)
        {
            log(str);
            return shared_from_this();
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
}

#endif

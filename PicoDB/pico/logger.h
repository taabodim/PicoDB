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
using namespace std;
namespace pico{
    
class logger{
    
public:
    std::ofstream outfile;
    boost::mutex log_mutex;
    void log(std::string str )
    {
        boost::interprocess::scoped_lock<boost::mutex> lock(log_mutex);
       // std::cout<<"logger : writing the log to file....\n";
        str.append("\n");
        append(str);
        
    }
    void append(std::string str)
    {
        outfile.write((char*) str.c_str(), str.size());
        outfile.flush();
    }
    
    logger(std::string filename){
        
        string path("/Users/mahmoudtaabodi/Documents/");
		std::string ext(".log");
		path.append(filename);
        path.append(ext);

        
        outfile.open(path,
                     std::fstream::out | std::fstream::app | std::fstream::binary);
        std::cout<<"logger instance created...\n";
    }
   
    logger()=delete;

   
  
    
};
}

#endif

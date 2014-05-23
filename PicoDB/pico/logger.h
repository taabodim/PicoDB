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
#include <pico/PicoConfig.h>

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

            boost::unique_lock<boost::mutex> lock(log_mutex);
            append(str);
            
            
        }
       logger(std::string filename){
        
           string path(PicoConfig::logDir);
		std::string ext(".log");
		path.append(filename);
        path.append(ext);
       
       outfile.open(path,
                     std::fstream::out | std::fstream::app | std::fstream::binary);

     }
   
        const static bool traceEnabled = true;
        bool isTraceEnabled()
        {
            return traceEnabled;
        }
        
        
        logger()=delete;
        
    virtual ~logger()
        {
                if(outfile.good())
                    outfile.close();
        }
    
};
    
  
    
 

}

#endif

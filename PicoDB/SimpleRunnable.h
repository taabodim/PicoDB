/*
 * SimpleRunnable.h
 *
 *  Created on: Mar 10, 2014
 *      Author: mahmoudtaabodi
 */
#pragma once
#include <Runnable.h>
#include <logger.h>
#include <pico/pico_utils.h>

namespace pico {

class SimpleRunnable : public Runnable{

public:
    static string logFileName;
    logger mylogger;
    
    SimpleRunnable(int taskId) : Runnable(taskId),mylogger(logFileName)
    {
       
    }
	void run() {
        std::string str;
        str.append("Simple Runnable is running ... by a thread  with id : ");
        str.append(convertToString<boost::thread::id>(boost::this_thread::get_id()));
        
		for (int i = 1; i < 10; i++) {
            
			mylogger.log(str);
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		}
	}
    
    virtual ~SimpleRunnable(){}

};
}


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
#include <atomic>
namespace pico {

class SimpleRunnable : public Runnable{

public:
    static string logFileName;
    logger mylogger;
     std::atomic_long numberOfoutputs;
    SimpleRunnable(int taskId) : Runnable(taskId),mylogger(logFileName)
    {
       
    }
	void run() {
        std::string str;
        str.append("Simple Runnable is running ... by a thread  with id : ");
        str.append(convertToString<boost::thread::id>(boost::this_thread::get_id()));
        
		for (int i = 1; i < 100; i++) {
            
            numberOfoutputs++;
            long  x = numberOfoutputs.load(std::memory_order_relaxed);
            str.append(" this is the num : ");
            str.append(convertToString<long>(x));
			mylogger.log(str);
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
	}
    
    virtual ~SimpleRunnable(){}

};
}


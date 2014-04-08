//
//  countdown_latch.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/8/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_countdown_latch_h
#define PicoDB_countdown_latch_h
#include <atomic>
#include <boost/thread.hpp>

using namespace std;

class countdown_latch{
    private :
    boost::mutex mutext;
//    boost::condition_variable messageClientQueueIsEmpty;
//    boost::condition_variable clientIsAllowedToWrite;
    boost::unique_lock<boost::mutex> lock;
    std::atomic_long countdown;
public:
    countdown_latch(long num):num(countdown),lock(mutex){}
    void countup(){
        countdown++;
    }
    void countdown(){
        countdown--;
        
        if(countdown==0)
        {
            std::cout<<"countdown is zero, notifying all the waiting threads"<<endl;
            lock.notify_all();
        }
    }
    void wait(){
        lock.wait();
    }
    ~virtual countdown_latch()
    {
    }
    
}



#endif

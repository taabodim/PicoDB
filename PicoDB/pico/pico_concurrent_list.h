//
//  pico_concurrent_list.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 3/28/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_pico_concurrent_list_h
#define PicoDB_pico_concurrent_list_h
#include <list>
#include <pico/pico_utils.h>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

using namespace std;
namespace pico{
   
    template <typename queueType>
    class pico_concurrent_list{
    private:
        list<queueType> underlying_list;
        boost::mutex mutex_;
        
    
    public:
        
        pico_concurrent_list()
        {
            log("pico_concurrent_list being constructed");
        }
        
       
        queueType pop()
        {
            boost::interprocess::scoped_lock<boost::mutex> lock_( mutex_);
            log("pico_concurrent_list : poping from the list..");
            queueType msg = underlying_list.front();
            underlying_list.pop_front();
            return msg;
        }
        bool empty()
        {
            return underlying_list.empty();
        }
        void push(queueType& msg)
        {
            boost::interprocess::scoped_lock<boost::mutex> lock_( mutex_);
            log("pushing pico msg to the front");
            underlying_list.push_front(msg);
            
        }
        queueType get(int index)
        {
            queueType empty;
            int i=0;
            for(typename list<queueType>::iterator iter=underlying_list.begin();iter!=underlying_list.end();++iter)
            {
                if(i==index)
                {
                    return *iter;
                }
                i++;
            }
//            log("index "+index+ " was not found in the list..concurrent list has only "+i+" elements \n");
            return empty;
        }
        
        void append(queueType t)
        {
        	underlying_list.push_back(t);
        }
        void clear()
        {
            
         while(!underlying_list.empty())
         {
            underlying_list.pop_front();
         }
            
         }
        
        
        string toString()
        {
            string str;
            while(!underlying_list.empty())
            {
                queueType t = underlying_list.front();
                 underlying_list.pop_front();
                str.append(t.getString());
            }
            log("this is the string representation of the pico_buffered_message");
            return str;
                    }
        virtual ~pico_concurrent_list()
        {
            log("pico_concurrent_list being destructed..");
        }
    };
}
#endif

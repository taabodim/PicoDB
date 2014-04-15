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
#include <logger.h>
using namespace std;
namespace pico{
    
    template<typename queueType>
    struct is_unique_ptr{
        static const bool value=false;
    };
    
  
    
    template<>
    struct is_unique_ptr<bufferTypeUnqiuePtr>{
        static const bool value=true;
    };
    
   
    
    template <typename queueType>
    class pico_concurrent_list{
    private:
        boost::mutex mutex_;
        //logger mylogger;
        
    public:
        list<queueType> underlying_list;
        
        pico_concurrent_list()
        {
            //    std::cout<<("pico_concurrent_list being constructed");
        }
       
        queueType pop()
        {
            queueType msg;
            boost::interprocess::scoped_lock<boost::mutex> lock_( mutex_);
           if(underlying_list.size()>0)
           {
            msg = underlying_list.back();
            underlying_list.pop_back();
            
            std::cout<<"pico_concurrent_list : poping from end of the list this item ..\n";
//          std::cout<<msg.toString()<<endl;
            
            return msg;
           }else{
               std::cout<<"pico_concurrent_list : returning empty message!!!"<<std::endl;
               return msg;//empty message
           }
        }
        bool empty()
        {
            return underlying_list.empty();
        }
        void push(queueType msg)
        {
            boost::interprocess::scoped_lock<boost::mutex> lock_( mutex_);//throws bad access
            //std::cout<<("pushing pico msg to the front");
            underlying_list.push_front(msg);
            
        }
        void printAll()
        {
            for (typename list<queueType>::iterator i = underlying_list.begin();
                 i != underlying_list.end(); ++i) {
                cout << "list iterator ==> " << i->toString() << endl;
            }
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
            //            std::cout<<("index "+index+ " was not found in the list..concurrent list has only "+i+" elements \n");
            return empty;
        }
        
        void append(queueType t)
        {
            
        	underlying_list.push_front(t);
        }
        void clear()
        {
            
            while(!underlying_list.empty())
            {
                underlying_list.pop_front();
            }
            
        }
        
        typename list<queueType>::iterator getLastBuffer()
        {
            return  underlying_list.begin();
        }
        
        typename list<queueType>::iterator getFirstBuffer()
        {
            return underlying_list.end();
        }
        
        string toString() 
        {
            string str;
            while(!underlying_list.empty())
            {
                queueType t = underlying_list.front();
                underlying_list.pop_front();
                std::cout<<"pico_concurrent_list : this is the string thats going to be appneded"<<t.toString()<<endl;
                str.append(t.toString());
            }
//            std::cout<<"this is the string representation of the pico_buffered_message"<<str<<endl;
            return str;
        }
        virtual ~pico_concurrent_list()
        {
            // std::cout<<("pico_concurrent_list being destructed..\n");
        }
    };
}
#endif

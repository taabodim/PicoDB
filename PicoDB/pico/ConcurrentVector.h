//
//  ConcurrentVector.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 5/23/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_ConcurrentVector_h
#define PicoDB_ConcurrentVector_h

#include <vector>
#include <pico/pico_utils.h>
#include <boost/thread/mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <pico_logger_wrapper.h>

using namespace std;
namespace pico{
    
    
    
    
    class pico_message;
    class pico_record;
    template< typename T >
    struct VectorTraits{
        static const bool is_shared_ptr = false; //for all cases its false, except the pico message
        static const bool is_pico_record=false;
    };
    
//    template<>
//    struct VectorTraits<pico_message>{
//        static const bool is_shared_ptr=true;
//        static const bool is_pico_record=false;
//        static  std::shared_ptr<pico_message> getEmptyInstance(){
//            return NULL;
//        }
//    };
//    
//    template<>
//    struct VectorTraits<pico_record>{
//        static const bool is_shared_ptr=true;
//        static const bool is_pico_record=true;
//        static pico_record getEmptyInstance()
//        {
//            pico_record r;
//            return r;
//        }
//    };
//    
//    template<> struct VectorTraits<std::shared_ptr<pico_message>>{
//        static const bool is_shared_ptr=true;
//        static const bool is_pico_record=false;
//        
//        static  std::shared_ptr<pico_message> getEmptyInstance(){
//            return NULL;
//        }
//        
//    };
    
//    class pico_messageForResponseQueue_;
//    
//    template<>
//    struct VectorTraits<pico_messageForResponseQueue_>{
//        static const bool is_shared_ptr=true;
//        static const bool is_pico_record=false;
//        static  std::shared_ptr<pico_message> getEmptyInstance(){
//            return NULL;
//        }
//    };
    
    
    
    
    
    template <typename queueType,typename traits>
    class ConcurrentVector : public pico_logger_wrapper{
    private:
        boost::mutex mutex_;
        
    public:
        vector<queueType> underlying_list;
        
        ConcurrentVector()
        {
            //     mylogger<<("\npico_concurrent_list being constructed");
        }
        
        queueType pop()//this method returns the end of queue and removes it from the end of queue
        {
            queueType msg;
            while(true)
            {
                boost::interprocess::scoped_lock<boost::mutex> lock_( mutex_,boost::interprocess::try_to_lock);
                if(lock_)
                {
                    if(underlying_list.size()>0)
                    {
                        msg = underlying_list.back();
                        underlying_list.pop_back();
                        
                        //mylogger<<"\npico_concurrent_list : poping from end of the list this item ..\n"<<msg.toString();
                        
                        return msg;
                    }else{
                        mylogger<<"pico_concurrent_list : returning empty message!!!\n";
                        return msg;//empty message
                    }
                }
            }
        }
        queueType pop(int sequenceNumber)//this method returns the end of queue and removes it from the end of queue
        {
            queueType msg;
            while(true)
            {
                boost::interprocess::scoped_lock<boost::mutex> lock_( mutex_,boost::interprocess::try_to_lock);
                if(lock_)
                {
                    if(underlying_list.size()>0)
                    {
                        
                        auto iter = underlying_list.begin();
                        //
                        for(int i=0;i<sequenceNumber;i++)
                        {
                            ++iter;
                            
                        }
                        queueType msg = *iter;
                        underlying_list.remove(*iter);
                        assert(!msg.toString().empty());
                        // mylogger<<"\npico_concurrent_list : poping from end of the list this item ..\n"<<msg.toString();
                        
                        return msg;
                    }else{
                        mylogger<<"ConcurrentVector : returning empty message!!!\n";
                        return msg;//empty message
                    }
                }
            }
        }
        void remove(queueType element)
        {
            if(underlying_list.size()>0)
            {
                underlying_list.remove(element);
            }
        }
        
        queueType peek()//this method returns the end of queue without deleting it
        
        {
            queueType msg;
            while(true)
            {
                boost::interprocess::scoped_lock<boost::mutex> lock_( mutex_,boost::interprocess::try_to_lock);
                if(lock_)
                {
                    if(underlying_list.size()>0)
                    {
                        msg = underlying_list.back();
                        //mylogger<<"\npico_concurrent_list : poping from end of the list this item ..\n"<<msg.toString();
                        
                        return msg;
                    }else{
                        if(mylogger.isTraceEnabled())
                        {
                            mylogger<<"ConcurrentVector : returning empty message!!!\n";
                        }
                        return msg;//empty message
                    }
                    break;
                }
                else{
                    if(mylogger.isTraceEnabled())
                    {
                        mylogger<<"ConcurrentVector :  trying to get the lock\n";
                    }
                    return traits::getEmptyInstance();
                }
            }
        }
        
        
        bool empty()
        {
            return underlying_list.empty();
        }
        void push_back(queueType msg)
        {
            while(true)
            {
                
                boost::interprocess::scoped_lock<boost::mutex> lock_( mutex_,boost::interprocess::try_to_lock);//throws bad access
                if(mylogger.isTraceEnabled())
                {
                    mylogger<<"\npushing pico msg to the front";
                }
                if(lock_)
                {
                    underlying_list.push_back(msg);
                    break;
                }
            }
        }
        void printAll()
        {
            mylogger << "printAll being called \n";
            for (typename list<queueType>::iterator i = underlying_list.begin();
                 i != underlying_list.end(); ++i) {
                mylogger << "list iterator ==> " << i->toString() << "\n";
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
            //            mylogger<<("index "+index+ " was not found in the list..concurrent list has only "+i+" elements \n");
            return empty;
        }
        
        void append(queueType t)
        {
            
        	underlying_list.push_front(t);
        }
        int size()
        {
            return underlying_list.size();
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
            
            
            for(typename list<queueType>::iterator iter=underlying_list.begin();iter!=underlying_list.end();++iter)
            {
                mylogger<<"ConcurrentVector : this is the string thats going to be appneded"<<iter->toString()<<"\n";
                str.append(iter->toString());
            }
            
            mylogger<<"this is the string representation of the pico_buffered_message"<<str<<"\n";
            return str;
        }
        virtual ~ConcurrentVector()
        {
            mylogger<<("\ConcurrentVector being destructed..\n");
        }
        
        void testTheConcurrentVector()
        {
            
        
        }
    };
}
#endif



//
//  PonocoDriverHelper.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/30/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_PonocoDriverHelper_h
#define PicoDB_PonocoDriverHelper_h
#include <mutex>
//the PonocoDriver Class extends this, to get the request
//and response queue..this is the class that wraps
//the mutex,locks , condition variables and queues for ponoco driver
//then by passing one instance of this class to different
//ponocoDriver instances, we can sync them....and have a producer consumer
//pattern


namespace pico {
    class PonocoDriverHelper{
        
        public:
     
        typedef pico_message queueType;
        
        PonocoDriverHelper():
        writeOneBufferLock(sessionMutex),allowedToWriteLock(allowedToWriteLockMutex),
        responseQueueIsEmptyLock(responseQueueMutex),bufferQueuePtr_(new pico_concurrent_list<bufferTypePtr>){
            
        }
        std::shared_ptr<pico_concurrent_list<bufferTypePtr>> bufferQueuePtr_;
        
        
        //this is the list that all the requests are buffered to
        // pico_concurrent_list<bufferTypePtr> bufferQueue_; //this is shared among all
        //the instances of PonocoDriver and PonocoClient, thats how they interact with each other
        //so I am going to use these two lists in the constructor and make them shared that way
        //as I am going to use one instance of it, i am going to use a std::shared_ptr<pico_concurrent_list<bufferTypePtr>> bufferQueuePtr_;
        pico_concurrent_list <queueType> responseQueue_;

        
        boost::mutex sessionMutex;   // mutex for the condition variable
        boost::mutex allowedToWriteLockMutex;
        boost::mutex queueRequestMessagesMutex;
        boost::mutex responseQueueMutex;
        boost::condition_variable bufferQueueIsEmpty;
        boost::condition_variable responseQueueIsEmpty;
        
        boost::condition_variable clientIsAllowedToWrite;
        boost::unique_lock<boost::mutex> allowedToWriteLock;
        boost::unique_lock<boost::mutex> writeOneBufferLock;
        boost::unique_lock<boost::mutex> responseQueueIsEmptyLock;
//
//        boost::mutex::scoped_lock<boost::mutex> allowedToWriteLock;
//        boost::mutex::scoped_lock<boost::mutex> writeOneBufferLock;
//        boost::mutex::scoped_lock<boost::mutex> responseQueueIsEmptyLock;
        
        
        
        
        
        
        
        
        
        
        
//        have a multithreaded app that has to read some data often, and occasionally that data is updated. Right now a mutex keeps access to that data safe, but it's expensive because I would like multiple threads to be able to read simultaneously, and only lock them out when an update is needed (the updating thread could wait for the other threads to finish).
//        
//        I think this is what boost::shared_mutex is supposed to d
//        boost shared lock example
//        boost::shared_mutex _access;
//        void reader()
//        {
//            // get shared access
//            boost::shared_lock<boost::shared_mutex> lock(_access);
//            
//            // now we have shared access
//        }
//        
//        void writer()
//        {
//            // get upgradable access
//            boost::upgrade_lock<boost::shared_mutex> lock(_access);
//            
//            // get exclusive access
//            boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
//            // now we have exclusive access
//        }
    
    };
}

#endif

//
//  ThreadPool.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/22/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_ThreadPool_h
#define PicoDB_ThreadPool_h
#include <vector>
#include <pico_concurrent_list.h>
#include <ThreadWorker.h>
#include <Runnable.h>

namespace pico {
    class ThreadPool
    {
        public :
        pico_concurrent_list<taskType>  taskQueue;
        bool stop;
        int numOfThreadWorkers;
        //as we use mutex in a ThreadWorker class and
        //it is not copyable , then the whole ThreadWorker class
        //becomes not copy constructable, so we use use ThreadWorker on the heap
        //in the STL containers
        std::vector<workerType> workers;
        ThreadPool(int num)
        {
            stop = false;
            numOfThreadWorkers = num;
            startWokers();
        }
        void startWokers()
        {
            for(int i=0;i<numOfThreadWorkers;i++)
            {
                auto worker = std::make_shared<ThreadWorker> (); //make shared gives me some error, figure it out later
//                std::shared_ptr<ThreadWorker> worker (new ThreadWorker());
                workers.push_back( worker);
                worker->start();
            }
        }
        ThreadPool& submitTask
        (taskType task)
        {
            taskQueue.push(task);
            return *this;
        }
        void start()
        {
            
            while(!stop)
            {
                
            }
        }
        virtual ~ThreadPool()
        {
            
        }
        
    };
    
    
    
}

#endif

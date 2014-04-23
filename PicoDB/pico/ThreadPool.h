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
#include <boost/enable_shared_from_this.hpp>
#include <memory>
#include <thread>         // std::thread
#include <pico/pico_utils.h>
namespace pico {
    class ThreadWorker;
    class ThreadPool;
 //   typedef std::shared_ptr<ThreadWorker> workerType;
    typedef ThreadWorker* workerType;
    
    class ThreadPool :  public std::enable_shared_from_this<ThreadPool>
    {
        public :
        std::shared_ptr<pico_concurrent_list<taskType>>  taskQueue;
        bool stop;
        int numOfThreadWorkers;
        //as we use mutex in a ThreadWorker class and
        //it is not copyable , then the whole ThreadWorker class
        //becomes not copy constructable, so we use use ThreadWorker on the heap
        //in the STL containers
        std::vector<workerType> workers;
        
        boost::mutex poolMutex;
        boost::condition_variable poolQueueIsEmpty;
        boost::condition_variable poolShutDown;
        boost::unique_lock<boost::mutex> poolLock;
        
        ThreadPool(int num) : taskQueue (new pico_concurrent_list<taskType>())
        , poolLock(poolMutex)
        {
            stop = false;
            numOfThreadWorkers = num;
            startWokers();
            }
        
        void startWokers()
        {
            for(int i=0;i<numOfThreadWorkers;i++)
            {
                
                //auto worker = std::make_shared<ThreadWorker> (taskQueue);
                ThreadWorker* worker (new ThreadWorker(taskQueue));
                
                 workers.push_back(worker);
//                worker.start();
                worker->start();
            }
        }
        void submitTask(taskType task)
        {
            //all the workers share the same queue so we put it for the first worker
            int randomIndex = random_number<int>(3)%numOfThreadWorkers;
            workers[randomIndex]->assignJobToWorker(task);
            workers[randomIndex]->notifyThisThreadQueueWasFilled();
        }
       
       
        void start()
        {
             startWokers();
            

           
        }
       void shutDownAfterWorkers()
        {
            for(int i=0;i<numOfThreadWorkers;i++)
            {
                if (workers[i]->threadHandle.joinable()) {
                    workers[i]->threadHandle.join();
                }
            }
        }
        void shutDown()
        {
            stop=true;
            
            poolShutDown.notify_all();
        }
        virtual ~ThreadPool()
        {
            
        }
        
    };
    

    void runThreadPool() {
        
//        auto pool = std::make_shared<ThreadPool>(2);
//        pool(6);
//
        ThreadPool* pool(new ThreadPool(2));
        
        
//        pool.start();
        
        for(int i=0;i<3;i++)
        {
        auto r1 =  std::make_shared<SimpleRunnable> (124);
            
            pool->submitTask(r1);
            std::cout << " i is "<<i<<endl;
        }
        pool->shutDownAfterWorkers();
        std::cout << "end of runThreadPool()" << std::endl;
        
    }
    
    void testThreadPool() {
        
        std::cout << "Running ten thread" << endl;
        std::thread t(runThreadPool);
        std::cout << "before joining the thread" << endl;
        t.join();
        
        std::cout << "Thread pool is done." << std::endl;
        
    }
    
}

#endif

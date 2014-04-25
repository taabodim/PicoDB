/*
 * ThreadWorker.h
 *
 *  Created on: Mar 10, 2014
 *      Author: mahmoudtaabodi
 */

#pragma once

#include <list>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "Runnable.h"
#include "SimpleRunnable.h"
#include <pico/pico_concurrent_list.h>

namespace pico {
    
    class ThreadWorker {
        
    private:
        
        int numberOfJobsDoneByTheWorker;
       
        std::shared_ptr<pico_concurrent_list<taskType>> queueOfTasks;//this queue must be common among the thread pool and all thread workers ,thus it should be in heap
        std::size_t WorkerQueueLimit;
        bool free;
        
        boost::mutex workerMutex;
        boost::unique_lock<boost::mutex> workerLock;
        boost::function<void()> bound_func;
    public:
        bool stopFlag_; //this should be public
        boost::condition_variable workerQueueIsEmpty;//this should be public

        boost::thread threadHandle;
        
        ThreadWorker(std::shared_ptr<pico_concurrent_list<taskType>> queueOfTasksArg) :workerLock(workerMutex),bound_func(boost::bind(&ThreadWorker::runIndefinitely, this)),threadHandle(bound_func){
           
            
            queueOfTasks = queueOfTasksArg;
            stopFlag_ = false;
            free = true;
            WorkerQueueLimit = 10;
           // threadHandle.detach(); dont do this, they should be joinable and thread pool should wait for them
       
        }
        
        bool isAvailable() {
            return free;
        }
        void setIsAvailable(bool fr) {
            free = fr;
        }
        void start() {
            //std::cout<<"thread worker starting..";
            
        }
        void assignJobToWorker(taskType task) {
            
            queueOfTasks->push(task);
            workerQueueIsEmpty.notify_all();
        }
        
        //defining  the function here because it uses the thread pool which is defined after the class
        void  runIndefinitely() {
            
            while (!stopFlag_) {
                
                try {
                    
                    while (!queueOfTasks->empty()) {
                        //   //std::cout << "queueOfTasks size is : "
                        //   << queueOfTasks->size() //<< std::endl;
                        setIsAvailable(false);
                        taskType task = queueOfTasks->pop();
                        if(task!=NULL)
                            task->run();
                        
                        // //std::cout << "task was finished by thread worker.."
                        //   //<< std::endl;
                    }
                    setIsAvailable(true);
                    // //std::cout << "queueOfTasks is empty,worker is waiting for more tasks.\n ";
                    workerQueueIsEmpty.wait(workerLock);
                    
                } catch (std::exception& e) {
                    
                    //std::cout << "thread worker threw  exception..." << e.what()
                    ////<< std::endl;
                    
                } catch (...) {
                    //std::cout << "thread worker threw unknown exception..."
                    ////<< std::endl;
                    
                }
                
                //            //std::cout << "thread worker is waiting for task" //<< std::endl;
                
            }
            
            
        }
        void notifyThisThreadQueueWasFilled()
        {
            workerQueueIsEmpty.notify_all();
        }
        virtual ~ThreadWorker() {
            stopFlag_ = true;
        }
        
    };
    
} /* namespace threadPool */


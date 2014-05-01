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
#include <pico_logger_wrapper.h>
namespace pico {
    
    class ThreadWorker : public pico_logger_wrapper{
        
    private:
        
        int numberOfJobsDoneByTheWorker;
       
        std::shared_ptr<pico_concurrent_list<taskType>> queueOfTasks;//this queue must be common among the thread pool and all thread workers ,thus it should be in heap
        std::size_t WorkerQueueLimit;
        bool free;
        
        boost::mutex workerMutex;
        boost::function<void()> bound_func;
    public:
        bool stopFlag_; //this should be public
        boost::condition_variable workerQueueIsEmpty;//this should be public

        boost::thread threadHandle;
        
        ThreadWorker(std::shared_ptr<pico_concurrent_list<taskType>> queueOfTasksArg) :bound_func(boost::bind(&ThreadWorker::runIndefinitely, this)),threadHandle(bound_func){
           
            
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
            mylogger<<"thread worker starting..\n";
            
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
                        //   mylogger << "queueOfTasks size is : "
                        //   << queueOfTasks->size() //<< std::endl;
                        setIsAvailable(false);
                        taskType task = queueOfTasks->pop();
                        if(task!=NULL)
                            task->run();
                        
                         mylogger << "task was finished by thread worker.."
                        <<"\n";
                    }
                    setIsAvailable(true);
                     mylogger << "queueOfTasks is empty,worker is waiting for more tasks.\n ";
                   
                    
                    boost::unique_lock<boost::mutex> workerLock(workerMutex);
                    if(workerLock.owns_lock())
                    {
                        workerQueueIsEmpty.wait(workerLock);
                    }
                    
                } catch (std::exception& e) {
                    
                    mylogger << "thread worker threw  exception..." << e.what()
                    << "\n";
                    
                } catch (...) {
                    mylogger << "thread worker threw unknown exception..."
                    << "\n";
                    
                }
                
                //            mylogger << "thread worker is waiting for task" //<< std::endl;
                
            }
            
            
        }
        void notifyThisThreadQueueWasFilled()
        {
            boost::unique_lock<boost::mutex> workerLock(workerMutex);
            if(workerLock.owns_lock())
            {
               workerQueueIsEmpty.notify_all();
            }

            
        }
        virtual ~ThreadWorker() {
            stopFlag_ = true;
        }
        
    };
    
} /* namespace threadPool */


//
//  DeleteTask.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/23/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_DeleteTaskRunnable_h
#define PicoDB_DeleteTaskRunnable_h
#include <Runnable.h>
#include <logger.h>
#include <pico/pico_utils.h>

#include <atomic>
namespace pico {
    class pico_collection;
    class DeleteTaskRunnable : public Runnable{
        
    public:
        static string logFileName;
        logger mylogger;
//        std::shared_ptr<pico_collection> collection;//this is the collection that delete will be called for
        //this is pointer because of cyclic header dependency, we cant have a member variable
       
        //i dont use std shared here because it will call the collection destrcutor
        //pico_collection* collection;
        std::shared_ptr<pico_collection> collection;
        pico_record  record;//this is the record that will be deleted until the next start offset
        std::atomic_long numberOfoutputs;
        DeleteTaskRunnable(std::shared_ptr<pico_collection> collectionArg,pico_record  recordArg) :collection(collectionArg) , Runnable(),mylogger(logFileName)
        {
            this->collection=collectionArg;
            this->record = recordArg;
        }
    
        void run();//definition is in pico_collection after that class is defined
       
    virtual ~DeleteTaskRunnable(){
    }
    
};
}




#endif

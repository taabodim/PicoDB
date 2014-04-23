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
        std::shared_ptr<pico_collection> collection;
        pico_record&  record;//this is the record that will be deleted until the next start offset
        std::atomic_long numberOfoutputs;
        DeleteTaskRunnable(pico_collection* collectionArg,pico_record&  recordArg) :collection(collectionArg) ,record(recordArg), Runnable(),mylogger(logFileName)
        {
            
        }
        void run();//definition is in pico_collection after that class is defined
        
        
        
       //        void deletion_function(pico_record firstRecordOfMessageToBeDeleted)//this function is the main function that deletion thread calls to delete the record
//        {
//            std::cout << "  offset in the list is  " << firstRecordOfMessageToBeDeleted.offset_of_record << endl;
//            
//            deleteOneMessage(firstRecordOfMessageToBeDeleted.offset_of_record);
//            //this should be done in a seperate thread
//            //to boost performance, and deleteRecord function should delete the node in index and queue the record
//            //for delete
//            
//        }
//        
//    }
//    
//    void deleteOneMessage(offsetType offsetOfFirstRecordOfMessage)
//    //this function deletes all the records of a message starting from the first one
//    //until the next "first record" is found
//    {
//        list<offsetType> all_offsets_for_this_message;
//        all_offsets_for_this_message.push_back(offsetOfFirstRecordOfMessage);
//        
//        offsetType nextOffset=offsetOfFirstRecordOfMessage;
//        do
//        {
//            
//            pico_record  nextRecord = retrieve(nextOffset);
//            if(pico_record::recordStartsWithConKEY(nextRecord))
//            {
//                all_offsets_for_this_message.push_back(nextOffset);
//                nextOffset +=  pico_record::max_size;
//            }
//            else{
//                break;
//            }
//        } while(nextOffset<=getEndOfFileOffset());
//        
//        
//        while(!all_offsets_for_this_message.empty())
//        {
//            deleteOneRecord(all_offsets_for_this_message.front());
//            all_offsets_for_this_message.pop_front();
//        }
//        
//        
//    }
    
    virtual ~DeleteTaskRunnable(){
    }
    
};
}




#endif

//
//  RecordInserter.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 5/11/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_RecordInserter_h
#define PicoDB_RecordInserter_h
#include <pico/pico_message.h>
#include <pico/pico_utils.h>
#include <pico/collection_manager.h>
#include <pico_logger_wrapper.h>

namespace pico{
    

class RecordInserter : public pico_logger_wrapper {
    private :
    collection_manager collectionManager;
    public :
    
    pico_message insertOneMessage(pico_message picoMsg,offsetType offset)//insert at a specific offset
    {
        int i = 0;
        std::shared_ptr<pico_collection> optionCollection =
        collectionManager.getTheCollection(picoMsg.collection);
        
        pico_buffered_message<pico_record> msg_in_buffers =
        picoMsg.getKeyValueOfMessageInRecords();
        
        pico_record firstrecord = msg_in_buffers.peek();
        
        offsetType whereToWriteThisRecord = offset;
        if (collectionManager.getTheCollection(picoMsg.collection)->ifRecordExists(
                                                                                   firstrecord)) {
            
            if (mylogger.isTraceEnabled()) {
                mylogger
                << "request_processr: record already exists,returning error! ";
            }
            
            string result("record already exists in database ");
            pico_message msg = pico_message::build_message_from_string(result,
                                                                       picoMsg.messageId);
            return msg;
            
        }
        
        
        
        do {
            pico_record record = msg_in_buffers.pop();
            
            mylogger
            << "\nrequest_processor : record that is going to be saved is this : "
            << record.toString();
            
            optionCollection->overwrite(record, whereToWriteThisRecord);
            whereToWriteThisRecord += max_database_record_size;
            
        } while (!msg_in_buffers.empty());
        
        string result("one message was added to database in ");
        result.append(convertToString(i));
        result.append(" seperate records");
        
        pico_message msg = pico_message::build_message_from_string(result,
                                                                   picoMsg.messageId);
        return msg;
        
        
    }
    
    pico_message insertOneMessage(pico_message picoMsg)
    {
        int i = 0;
        std::shared_ptr<pico_collection> optionCollection =
        collectionManager.getTheCollection(picoMsg.collection);
        
        pico_buffered_message<pico_record> msg_in_buffers =
        picoMsg.getKeyValueOfMessageInRecords();
        
        pico_record firstrecord = msg_in_buffers.peek();
        
        offsetType whereToWriteThisRecord = -1;
        if (collectionManager.getTheCollection(picoMsg.collection)->ifRecordExists(
                                                                                   firstrecord)) {
            
            if (mylogger.isTraceEnabled()) {
                mylogger
                << "request_processr: record already exists,returning error! ";
            }
            
            string result("record already exists in database ");
            pico_message msg = pico_message::build_message_from_string(result,
                                                                       picoMsg.messageId);
            return msg;
            
            
        }
        
        
        do {
            
            pico_record record = msg_in_buffers.pop();
            
            mylogger
            << "\nrequest_processor : record that is going to be saved is this : "
            << record.toString();
            
            optionCollection->append(record); //append the
            whereToWriteThisRecord += max_database_record_size;
            
        } while (!msg_in_buffers.empty());
        string result("one message was added to database in ");
        result.append(convertToString(i));
        result.append(" seperate records");
        
        pico_message msg = pico_message::build_message_from_string(result,
                                                                   picoMsg.messageId);
        return msg;
        
        
    }
};
}


#endif

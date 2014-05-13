/*
 * collection.h
 *
 *  Created on: Mar 13, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_COLLECTION_H_
#define PICO_COLLECTION_H_

#include <fstream>
#include <iostream>
#include <pico/pico_record.h>
#include <stdio.h>
#include <pico/pico_index.h>
#include <logger.h>
#include <pico/pico_concurrent_list.h>
#include <ThreadPool.h>
#include <DeleteTaskRunnable.h>
#include <pico/pico_utils.h>
#include <pico_logger_wrapper.h>
#include <OffsetManager.h>
//this is a wrapper around the file that represents the collection
namespace pico {
    struct recordInDatabase{
        char typeHolder [pico_record::max_key_type_size];
        char keyHolder[pico_record::max_key_size];
        char valueHolder[pico_record::max_value_size];
    };
    
    //these the parts of record that are saved in file
    const static int max_database_record_size = sizeof(struct recordInDatabase);
    
    class pico_collection: public std::enable_shared_from_this<pico_collection>,
    public pico_logger_wrapper {
        
        //add this to some logic that doesnt call other functions who have this
        //boost::interprocess::scoped_lock<boost::mutex> lock(collectionMutex)
        
        //this is a unique_ptr because as its static i want to have only one for all the collections
        //thus i want to compare it against NULL
        const static int numberOfDeletionThreads = 1;
        static std::unique_ptr<ThreadPool> delete_thread_pool;
        std::mutex writeMutex;
        std::shared_ptr<OffsetManager> offsetManager;
    public:
        
        boost::mutex collectionMutex;
        pico_binary_index_tree index;
        string filename;
        pico_collection() = delete;
        
        pico_collection(std::string name)  {
            

        	name = getFullCollectionName(name);
            filename = name;
            assert(!filename.empty());
            
            std::shared_ptr<OffsetManager> offsetManagerPtr(new OffsetManager (filename));
            offsetManager = offsetManagerPtr;
            // mylogger<<("pico_collection : name of the file is "<<filename<<std::endl;
            
            openFileIfItDoesntExist(filename);
            
            mylogger << "pico_collection : file being opened now.\n";
//            file.open(filename,
//                      std::fstream::in | std::fstream::out | std::fstream::binary);
            //use the fstream for both reading and writing and appending , there should be
            //only one stream open to the file, it makes the life easier and less buggier.
            //writing in out mode,
            //if i do app mode, the seekp wont work properly
            
            // test_reading_from_collection();
            
            list<pico_record> all_pico_messages = read_all_messages_records(); //write a function to get all the begining records for putting them in the tree (done)
            size_t initSize = all_pico_messages.size();
            index.build_tree(all_pico_messages);
            
            assert(index.size()==(initSize+1));
        }
        
        
        
        
        size_t getNumberOfMessages() {
            //    	list<offsetType> all_Messages_offsets = read_all_Messages_offsets();
            //		return all_Messages_offsets.size();
            return index.numberOfNodesInTree;
        }
        
        string getName() {
            return filename;
        }
        void queue_record_for_deletion(
                                       pico_record& firstRecordOfMessageToBeDeleted) {
            auto deleteTask = std::make_shared < DeleteTaskRunnable
            > (shared_from_this(), firstRecordOfMessageToBeDeleted);
            
            //mylogger<<"hello"<<"I am here"<<" "<<3<<3.23<<"\n";
            delete_thread_pool->submitTask(deleteTask);
        }
        void deleteRecord(pico_record& firstRecordOfMessageToBeDeleted,bool async) {
            //this function gets called by request processor, it finds the first record of the message
            //that has the same key and delete all the records that follows that first record until the next
            //first record in the file
            
            //delete this record all over the file , logically we should only have one record with the same key
            //when we are inserting to the file, we check if the record exists , we update it( the update is just deleting and inserting again)
            read_offset_of_this_record(firstRecordOfMessageToBeDeleted); //offset of record is found using index.search
            pico_record_node node = *index.convert_pico_record_to_index_node(
                                                                             firstRecordOfMessageToBeDeleted);
            firstRecordOfMessageToBeDeleted.offset_of_record = node.offset;
            
            if(async) {queue_record_for_deletion(firstRecordOfMessageToBeDeleted);
            }
            else {
            deletion_function(firstRecordOfMessageToBeDeleted);
            }
            index.remove(node);
            
        }
        void deletion_function(pico_record firstRecordOfMessageToBeDeleted) //this function is the main function that deletion thread calls to delete the record
        {
            
            //   read_offset_of_this_record(firstRecordOfMessageToBeDeleted); the index has already been deleted
            mylogger << "this offset is going to be deleted "
            << firstRecordOfMessageToBeDeleted.offset_of_record << "\n";
            deleteOneMessage(firstRecordOfMessageToBeDeleted.offset_of_record);
            
            //this should be done in a seperate thread
            //to boost performance, and deleteRecord function should delete the node in index and queue the record
            //for delete
            
        }
        void deleteOneMessage(offsetType offsetOfFirstRecordOfMessage)
        //this function deletes all the records of a message starting from the first one
        //until the next "first record" is found
        {
            list<offsetType> all_offsets_for_this_message;
            
            offsetType nextOffset = offsetOfFirstRecordOfMessage;
            offsetType endOffset = offsetManager->getEndOfFileOffset();
            bool ignoreFirstBEGKEYRecordOffset = true;
            do {
                
                std::shared_ptr<pico_record> nextRecord = retrieve(nextOffset);
                
                if (pico_record::recordStartsWithBEGKEY(*nextRecord) && ignoreFirstBEGKEYRecordOffset==false ) {
                   break;
                } else {
                    all_offsets_for_this_message.push_back(nextOffset);
                    nextOffset += max_database_record_size;
                    ignoreFirstBEGKEYRecordOffset = false;
                    
                }
            } while (nextOffset < endOffset);
            
            while (!all_offsets_for_this_message.empty()) {
                deleteOneRecord(all_offsets_for_this_message.front());
                all_offsets_for_this_message.pop_front();
            }
            
        }
        
        msgPtr retrieveOneMessage(offsetType offsetOfFirstRecordOfMessage,
                                        string messageIdForResponse)
        //this function retrieves all the records of a message starting from the first one
        //until the next "first record" is found
        {
            
            if (mylogger.isTraceEnabled()) {
                mylogger
                << "\npico_collection : retrieveOneMessage :  offsetOfFirstRecordOfMessage "
                << offsetOfFirstRecordOfMessage
                << " messageIdForResponse  :  " << messageIdForResponse<<" max_database_record_size: "<<max_database_record_size;
            }
            offsetType endOffset = offsetManager->getEndOfFileOffset();
            
            pico_buffered_message<pico_record> all_records_for_this_message;
            
            offsetType nextOffset = offsetOfFirstRecordOfMessage;
            assert(nextOffset>=0);
            
            std::shared_ptr<pico_record> nextRecord = retrieve(nextOffset); //get the first record of this message
            all_records_for_this_message.append(*nextRecord);
            
            nextOffset += max_database_record_size;
            do {
                
            	std::shared_ptr<pico_record> nextRecord = retrieve(nextOffset);
                assert(!nextRecord->toString().empty());
                
//                assert(pico_record::recordStartsWithBEGKEY(nextRecord) || pico_record::recordStartsWithConKEY(nextRecord));
                if (pico_record::recordStartsWithConKEY(*nextRecord)) //this for the next message
                {
                    if (mylogger.isTraceEnabled()) {
                        mylogger<< "\npico_collection : retrieveOneMessage :  this buffer starts with CONKEY  \n";
                    }
                    
                    all_records_for_this_message.append(*nextRecord);
                    if (mylogger.isTraceEnabled()) {
                        mylogger
                        << "\npico_collection : retrieveOneMessage : "
                        " this record was retrived from db and appended to list  "
                        << nextRecord->toString() << "\n";
                    }
                }
                else
                {
                    if (mylogger.isTraceEnabled()) {
                        mylogger<< "\npico_collection : retrieveOneMessage : this buffer doesnt start with CONKEY  \n"
                        << nextRecord->toString() << "\n";
                    }
                    
                    break;
                }
                
                
                nextOffset += max_database_record_size;
                
            } while (nextOffset<endOffset);
            pico_message util;
            mylogger
            << "\n pico_collection : about to convert all the buffers read from db to a nice pico_message \n ";
      
            assert(!messageIdForResponse.empty());
            std::shared_ptr<pico_message> msg = util.convert_records_to_message(
                                                               all_records_for_this_message, messageIdForResponse,LONG_MESSAGE_JUST_KEY_VALUE_WITH_BEGKEY_CONKEY);
            
            mylogger
            << "\n pico_collection : retrieveOneMessage this is the whole message retrieved "
            << msg->toString();
            return msg;
            
        }
        
        msgPtr getMessageByKey(pico_record& record,
                                     string messageIdForResponse) {
            string result;
            assert(!pico_record::isRecordEmpty(record));
            
            if(mylogger.isTraceEnabled())
            {
                mylogger << " pico_collection : getMessageByKey record.offset_of_record is "
            << record.offset_of_record << "\n" <<"record is "<<record.toString()<<"\n";
            }
            if(CollectionIsEmpty())
            {
                result.append("COLLECTIONISEMPTY");
            
            }
           
            else
                if (index.search(record) != nullptr) {
                
                assert(!messageIdForResponse.empty());
                assert(record.offset_of_record>=0);
                msgPtr foundMessage = retrieveOneMessage(
                                                               record.offset_of_record, messageIdForResponse);
                
               
                if(mylogger.isTraceEnabled())
                { mylogger
                << "\n pico_collection : getMessageByKey this is the whole message , messageId : "
                << foundMessage->messageId << "\n content : \n "
                << foundMessage->toString();
                }
                assert(!foundMessage->messageId.empty());
                return foundMessage;
            }
           else
           {
            assert(false);
            if(mylogger.isTraceEnabled())
            {
                mylogger << " getMessageByKey didnt find this record in the index ";
            }
               
            result.append("NODATAFOUND");
            
           }
            msgPtr msg = pico_message::build_message_from_string(result,
                                                                       messageIdForResponse);
            return msg;
            
        }

        
        list<offsetType> read_all_Messages_offsets() {
            //this function will read over the file and gets all the first records that are starting with  either BEGKEY or CONKEY
            list<offsetType> list_of_offsets;
            offsetType endOfFile_Offset = offsetManager->getEndOfFileOffset();
            // cout << " offset of end of file is " << endOfFile_Offset //<< std::endl;
            
            for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
                 max_database_record_size) {
                //  cout << " read_all_records_offsets : reading one record from offset "<<offset  //<< std::endl;
                
            	std::shared_ptr<pico_record> record_read_from_file = retrieve(offset);
                
                if (pico_record::recordStartsWithBEGKEY(*record_read_from_file)) {
                    list_of_offsets.push_back(offset);
                }
                //
                //            else{
                //                mylogger<<("warning : read_all_records_offsets : key is empty!");
                //            }
                //
            }
            return list_of_offsets;
        }
        
        list<pico_record> read_all_messages_records() {
            list<pico_record> list_;
            try {
                //this function will read over the file and gets all the first records that are starting with  either BEGKEY or CONKEY and return them as pico_records not offsets
                
                offsetType endOfFile_Offset = offsetManager->getEndOfFileOffset();
                mylogger << "\n offset of end of file is " << endOfFile_Offset;
                
                for (offsetType offset = 0; offset < endOfFile_Offset; offset +=
                     max_database_record_size) {
                    
                	std::shared_ptr<pico_record> record_read_from_file = retrieve(offset);
                    record_read_from_file->offset_of_record =offset;
                    
                    
                    mylogger
                    << "\n read_all_messages_records : reading one record from offset "
                    << offset << "\n the record read is "
                    << record_read_from_file->toString();
                    
                    if (pico_record::recordStartsWithBEGKEY(
                                                            *record_read_from_file)) {
                        list_.push_back(*record_read_from_file);
                    }
                    
                }
            } catch (...) {
                std::cerr << "Exception: read_all_messages_records: unknown thrown"
                << "\n";
                raise (SIGABRT);
                
            }
            
            return list_;
        }
        
        void read_offset_of_this_record(pico_record& record) {
            nodeType node = index.search(record);
            if (node == nullptr)
                record.offset_of_record = -1;
            else {
                record.offset_of_record = node->offset;
                
            }
            
        }

        bool ifRecordExists(pico_record& record) {
            //this function checks if a record exists or not
            
            if (index.search(record) == nullptr)
                return false;
            
            return true;

            //this is the old version of funciton
            //        list<offsetType> allRecords = read_all_offsets_that_match_this_record(record);
            //        if(allRecords.size()>0) return true;
            //        return false;
        }
        void deleteOneRecord(offsetType offsetOfToBeDeletedRecord) {
            if (offsetOfToBeDeletedRecord == -1)
                return;
            
            string empty("");
            pico_record empty_record;
            pico_record::setTheKeyInData(empty_record,empty);
            pico_record::setTheValueInData(empty_record,empty);
            pico_record::setTheRecordTypeInData(empty_record,empty);
            
            //            (empty,empty);
           // pico_record current_record = retrieve(offsetOfToBeDeletedRecord);
            //        nodeType node = index.createANodeBasedOnOffset(offset);
            //		index.deleteNode(node);
            
            overwrite(empty_record, offsetOfToBeDeletedRecord);
            
            // index.remove(*index.convert_pico_record_to_index_node(current_record));//passing the key of the record that was deleted
            //to calculate the right
            
        }
        offsetType get_offset_of_this_record(pico_record& record) { //this function is used in updating and replacing
            //this function should use the index to get the offset of the record
            //which is always the first record of the message
            //iterating through all the records in the file should be avoided
            // at all the times. and index should be found...
            nodeType node = index.search(record);
            if (node == nullptr)
                return -1;
            else
                return node->offset;
            
                }
        void overwrite(pico_record record, offsetType record_offset) { //this overwrites a file
            
            mylogger << "\noverwriting  one record to collection at this offset"<<record_offset<<"\n";
            int tryNum=0;
            do {
                //this while loop will take care of multi threaded delete
               
                append_a_record(record,record_offset);
                
                std::shared_ptr<pico_record> currentRecord = retrieve(record_offset);
                
                if(mylogger.isTraceEnabled()) //just to test if overwrite worked properly
                {
                	std::shared_ptr<pico_record> read_from_file  = retrieve(record_offset);
                    assert(currentRecord->areRecordsEqual(*read_from_file));
                }
                
                if (currentRecord->areRecordsEqual(record)) {
                    break;
                } else {
                    
                    mylogger << "overwrite didnt work on offset " << record_offset
                    << "\n" << " currentRecord.getKeyAsString() is "
                    << currentRecord->getKeyAsString()
                    << " vs record.getKeyAsString is "
                    << record.getKeyAsString()
                    << "currentRecord.getValueAsString() is "
                    << currentRecord->getValueAsString()
                    << " vs record.getValueAsString() is "
                    << record.getValueAsString();
                    tryNum++;
                }
            } while (tryNum<20);
        }
        void insert(pico_record& record) { //this appends to the end of file
            append(record);
        }
        void append(pico_record& record) { //this appends to the end of file
            offsetType record_offset = offsetManager->getEndOfFileOffset();
            record.offset_of_record = record_offset;
            append_a_record(record, record_offset);
            
        }
        
        std::shared_ptr<pico_record>  retrieve(offsetType offset) {
            std::shared_ptr<pico_record> read_from_file (new pico_record());
            
            
            FILE *ptr_myfile;
            struct recordInDatabase my_record;
            
            ptr_myfile=fopen(filename.c_str(),"rb");
            if (!ptr_myfile)
            {
                printf("Unable to open file!");
                
            }
            fseek ( ptr_myfile , offset , SEEK_SET );

            fread(&my_record,sizeof(struct recordInDatabase),1,ptr_myfile);
            
            
            for(int i=pico_record::beg_key_type_index;i<pico_record::end_key_type_index;i++)
            {
                
                read_from_file->data_[i]=my_record.typeHolder[i-pico_record::beg_key_type_index];
                
            }
            for(int i=pico_record::beg_of_key_index;i<pico_record::end_of_key_index;i++)
            {
                read_from_file->data_[i]=my_record.keyHolder[i-pico_record::beg_of_key_index];
                
            }
            for(int i=pico_record::beg_of_value_index;i<pico_record::end_of_value_index;i++)
            {
                read_from_file->data_[i]=my_record.valueHolder[i-pico_record::beg_of_value_index];
                
            }
            
            
            fclose(ptr_myfile);
            return read_from_file;
        }
        
        
        
        
        void  append_a_record  (pico_record& record, offsetType record_offset) {
            
            FILE *ptr_myfile;
            struct recordInDatabase my_record;
            
            std::unique_lock < std::mutex > writeLock(writeMutex);
          
            ptr_myfile=fopen(filename.c_str(),"r+b");
            if (!ptr_myfile)
            {
                mylogger<<"Unable to open file!";

            }

            assert(ptr_myfile);
            for(int i=pico_record::beg_key_type_index;i<pico_record::max_key_type_size;i++)
            {
                
                my_record.typeHolder[i-pico_record::beg_key_type_index]=record.data_[i];
                
            }
            for(int i=pico_record::beg_of_key_index;i<pico_record::end_of_key_index;i++)
            {
                my_record.keyHolder[i-pico_record::beg_of_key_index]=record.data_[i];
                
            }
            for(int i=pico_record::beg_of_value_index;i<pico_record::end_of_value_index;i++)
            {
                my_record.valueHolder[i-pico_record::beg_of_value_index]=record.data_[i];
                
            }
            
            fseek ( ptr_myfile , record_offset , SEEK_SET );
            fwrite(&my_record, sizeof(struct recordInDatabase), 1, ptr_myfile);
            fflush(ptr_myfile);
            fclose(ptr_myfile);
            
            offsetManager->setEndOfFileOffset(record_offset+max_database_record_size);
            
            if (pico_record::recordStartsWithBEGKEY(record)) {
                if(index.search(record)==nullptr)
                {
                     mylogger<<"pico_collection : Adding the BEGKEY record to the index !";
                    
                    int initSize = index.size();
                    record.offset_of_record=record_offset;
                    index.add_to_tree(record);
                    assert(index.size()==(initSize+1));
                }
                else{
                    mylogger<<"pico_collection : BEGKEY already exists in the tree !";

                }
            }
            
            if(mylogger.isTraceEnabled()) //just to test if append worked properly
            {
               std::shared_ptr<pico_record> read_from_file  = retrieve(record_offset);
               assert(record.areRecordsEqual(*read_from_file));
            }
           
        }
        bool CollectionIsEmpty()
        {
            
            return (offsetManager->getEndOfFileOffset()==0);
        }
        bool dropCollection()
        {
         
       	   int  status = remove(filename.c_str());

        	   if( status == 0 )
        	      {mylogger<<filename<<"  deleted successfully\n";
        	      return true;
                  }
        	   else
        	   {
        		   mylogger<<"Unable to delete the file"<<filename<<"\n";
        		   return false;
        	   }
        }
        
        ~pico_collection() {
           
            mylogger << " pico_collection destructor being called..\n";
           
        }
        
        const pico_record empty_record;
    private:

    };
    
    void DeleteTaskRunnable::run() {
        std::string str;
        str.append("DeleteTask Runnable is running ... by a thread  with id : ");
        str.append(
                   convertToString<boost::thread::id>(boost::this_thread::get_id()));
        
        collection->deletion_function(record);
        numberOfoutputs++;
        long x = numberOfoutputs.load(std::memory_order_relaxed);
        
        mylogger << " this is the num of deleted messages from collection : " << x;
        
    }
    offsetType OffsetManager::getInitialOffset()
    	{
    		 FILE *fp;
    		assert(!filename.empty());
    	    fp=fopen(filename.c_str(),"r+b");
    	    assert(fp!=NULL);
    		fseek(fp, 0L, SEEK_END);
    		size_t sz = ftell(fp);
    		fclose(fp);
    		return sz;
    	}
}
#endif /* COLLECTION_H_ */

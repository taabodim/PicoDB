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

#include <pico/pico_index.h>
#include <logger.h>
#include <pico/pico_concurrent_list.h>
#include <ThreadPool.h>
#include <DeleteTaskRunnable.h>
#include <pico/pico_utils.h>
#include <pico_logger_wrapper.h>
//this is a wrapper around the file that represents the collection
namespace pico {
    
    class pico_collection : public std::enable_shared_from_this<pico_collection> ,public pico_logger_wrapper {
        
        //add this to some logic that doesnt call other functions who have this
        //boost::interprocess::scoped_lock<boost::mutex> lock(collectionMutex)
        
        //this is a unique_ptr because as its static i want to have only one for all the collections
        //thus i want to compare it against NULL
        const static int numberOfDeletionThreads =1;
        static std::unique_ptr<ThreadPool> delete_thread_pool;
        std::mutex writeMutex;
        
        
    public:
        
        boost::mutex collectionMutex;
        pico_binary_index_tree index;
        
        pico_collection() = delete;
        
        
        
        pico_collection(std::string name) {
            
            string path("/Users/mahmoudtaabodi/Documents/");
            std::string ext(".dat");
            path.append(name);
            path.append(ext);
            name = path;
            filename = name;
            // mylogger<<("pico_collection : name of the file is "<<filename<<std::endl;
            //		infile.open(name, std::fstream::in | std::fstream::binary);
            //		outfile.open(name,
            //				std::fstream::out | std::fstream::app | std::fstream::binary);
            
            openFileIfItDoesntExist(filename);
            
            mylogger<<"pico_collection : file being opened now.\n";
            file.open(filename,	std::fstream::in |	std::fstream::out | std::fstream::binary);
            //use the fstream for both reading and writing and appending , there should be
            //only one stream open to the file, it makes the life easier and less buggier.
            //writing in out mode,
            //if i do app mode, the seekp wont work properly
            
            
            // test_reading_from_collection();
            
            list<pico_record> all_pico_messages = read_all_messages_records();//write a function to get all the begining records for putting them in the tree (done)
            index.build_tree(all_pico_messages);
            
        }
        
        
        //    list<offsetType> get_All_Offsets_InAFile()
        //    {
        //        list<offsetType> alloffsets;
        //        offsetType lastOffset =getEndOfFileOffset(file);
        //        mylogger<<" pico_collection : lastOffset is  "<<lastOffset<<endl;
        //        for(int i=0;i<lastOffset; i=i+ pico_record::max_size)
        //        {
        //             mylogger<<" pico_collection : one Offset is  "<<i<<endl;
        //            alloffsets.push_back(i);
        //        }
        //        return alloffsets;
        //    }
        
        
        size_t getNumberOfMessages() {
            //    	list<offsetType> all_Messages_offsets = read_all_Messages_offsets();
            //		return all_Messages_offsets.size();
            return index.numberOfNodesInTree;
        }
        
        string getName() {
            return filename;
        }
        
        //	pico_record get(int index) {
        //   		pico_record record;
        //		offsetType offset = index * record.max_size;
        //		if (offset > getEndOfFileOffset())
        //			return empty_record;
        //        record = retrieve(offset);
        //
        //		return record;
        //	}
        void queue_record_for_deletion(pico_record& firstRecordOfMessageToBeDeleted)
        {
            auto deleteTask = std::make_shared<DeleteTaskRunnable> (shared_from_this(),firstRecordOfMessageToBeDeleted);
            
            //mylogger<<"hello"<<"I am here"<<" "<<3<<3.23<<"\n";
            delete_thread_pool->submitTask(deleteTask);
        }
        void deleteRecord(pico_record& firstRecordOfMessageToBeDeleted) {
            //this function gets called by request processor, it finds the first record of the message
            //that has the same key and delete all the records that follows that first record until the next
            //first record in the file
            
            
            //delete this record all over the file , logically we should only have one record with the same key
            //when we are inserting to the file, we check if the record exists , we update it( the update is just deleting and inserting again)
            read_offset_of_this_record(firstRecordOfMessageToBeDeleted); //offset of record is found using index.search
            pico_record_node node  =  *index.convert_pico_record_to_index_node(firstRecordOfMessageToBeDeleted);
            firstRecordOfMessageToBeDeleted.offset_of_record = node.offset;
            queue_record_for_deletion(firstRecordOfMessageToBeDeleted);
            index.remove(node);
            
            
            
        }
        void deletion_function(pico_record firstRecordOfMessageToBeDeleted)//this function is the main function that deletion thread calls to delete the record
        {
            
            //   read_offset_of_this_record(firstRecordOfMessageToBeDeleted); the index has already been deleted
            mylogger<<"this offset is going to be deleted "<<firstRecordOfMessageToBeDeleted.offset_of_record<<"\n";
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
            all_offsets_for_this_message.push_back(offsetOfFirstRecordOfMessage);
            
            offsetType nextOffset=offsetOfFirstRecordOfMessage;
            offsetType endOffset = getEndOfFileOffset(file);
            do
            {
                
                pico_record  nextRecord = retrieve(nextOffset);
                if(pico_record::recordStartsWithConKEY(nextRecord))
                {
                    all_offsets_for_this_message.push_back(nextOffset);
                    nextOffset +=  pico_record::max_size;
                }
                else{
                    break;
                }
            } while(nextOffset<=endOffset);
            
            
            while(!all_offsets_for_this_message.empty())
            {
                deleteOneRecord(all_offsets_for_this_message.front());
                all_offsets_for_this_message.pop_front();
            }
            
            
        }
        
        pico_message retrieveOneMessage(offsetType offsetOfFirstRecordOfMessage)
        //this function retrieves all the records of a message starting from the first one
        //until the next "first record" is found
        {
              offsetType endOffset = getEndOfFileOffset(file);
            
            if(offsetOfFirstRecordOfMessage>endOffset)
            {
                string error("offset in tree is wrong");
                string messageId ("offsetIsWrong");
            pico_message  msg =  pico_message::build_message_from_string(error,messageId);
            mylogger<<"\n retrieveOneMessage offset is  wrong "<<msg.toString();
            return msg;
            }
            
//            std::shared_ptr<list<pico_record>> all_records_for_this_message(new list<pico_record>());
            //it should be shared ptr in the heap because list wont copy over to the next function nicely
            pico_buffered_message<pico_record> all_records_for_this_message;
            
             offsetType nextOffset=offsetOfFirstRecordOfMessage;
          
            do
            {
                
                pico_record  nextRecord = retrieve(nextOffset);
                all_records_for_this_message.append(nextRecord);
                
                nextOffset +=  pico_record::max_size;
                
            } while(nextOffset<=endOffset);
            
            
            pico_message  util;
            pico_message msg= util.convertBuffersToMessage(all_records_for_this_message);
            mylogger<<"\n retrieveOneMessage this is the whole message retrieved "<<msg.toString();
            return msg;
            
        }
        
        pico_message getMessageByKey(pico_record record)
        {
        
            if(index.search(record)!=nullptr)
            {
                mylogger<<" getMessageByKey record.offset_of_record is "<<record.offset_of_record<<"\n";
                pico_message foundMessage =  retrieveOneMessage(record.offset_of_record);
                mylogger<<"\n getMessageByKey this is the whole message found "<<foundMessage.toString();
                return foundMessage;
            }
            mylogger<<" getMessageByKey didnt find this record ";
          
            string noDataFound("NODATAFOUND");

            pico_message msg=pico_message::build_message_from_string(noDataFound,record.getMessageIdAsString());
            return msg;
            
        }
        //        list<pico_record> find(pico_record& firstRecordOfMessageToBeFound) {
        //
        //		list<pico_record> all_records;
        //		list<offsetType> list_of_offset = read_all_offsets_that_match_this_record(firstRecordOfMessageToBeFound);
        //
        //        while (!list_of_offset.empty()) {
        //
        //			offsetType offset = list_of_offset.front();
        //			cout << "  offset in the list is  " << offset << endl;
        //			list_of_offset.pop_front();
        //			pico_record record = retrieve(offset);
        //			all_records.push_back(record);
        //		}
        //		return all_records;
        //	}
        pico_record retrieve(offsetType offset) {//was debugged
            //it seems if if I use the global infile
            //and when I use it in other functions, it stops working here , it wont read the data here , so I am using the infileLocal ,
            std::ifstream infileLocal;
            infileLocal.open(filename, std::fstream::in | std::fstream::binary);
            mylogger << " pico_collection  : retrieve : offset is  " << offset;
            pico_record record_read_from_file;
            
            infileLocal.seekg(offset);
            infileLocal.read((char*) record_read_from_file.data_,
                             pico_record::max_size);
           
            record_read_from_file.offset_of_record = offset;
            mylogger << "\n read_all_records : record_read_from_file  " << record_read_from_file.toString();
            
            mylogger << "\n read_all_records : record_read_from_file.getKeyAsString() " << record_read_from_file.getKeyAsString();
            
            mylogger << "\n read_all_records : record_read_from_file.getValueAsString() " << record_read_from_file.getValueAsString();
            infileLocal.close();
            return record_read_from_file;
            
        }
        //    list<pico_record> read_all_records() { //this function was debugged!
        //
        //		list<pico_record> list_of_records;
        //		offsetType endOfFile_Offset = getEndOfFileOffset(file);
        //		cout << "read_all_records : offset of end of file is " << endOfFile_Offset //<< std::endl;
        //		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
        //             pico_record::getRecordSize()) {
        //            cout << " read_all_records : reading one record from offset "<<offset  //<< std::endl;
        //
        //			pico_record record_read_from_file = retrieve(offset);
        //				if (!record_read_from_file.getKeyAsString().empty()) {
        //
        //				list_of_records.push_back(record_read_from_file);
        //			}
        //            else{
        //                mylogger<<("warning : read_all_records : key is empty!");
        //            }
        //
        //		}
        //        return list_of_records;
        //	}
        
        
        list<offsetType> read_all_Messages_offsets() {
            //this function will read over the file and gets all the first records that are starting with  either BEGKEY or CONKEY
            list<offsetType> list_of_offsets;
            offsetType endOfFile_Offset = getEndOfFileOffset(file);
            // cout << " offset of end of file is " << endOfFile_Offset //<< std::endl;
            
            for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
                 pico_record::max_size) {
                //  cout << " read_all_records_offsets : reading one record from offset "<<offset  //<< std::endl;
                
                pico_record record_read_from_file = retrieve(offset);
                
                
                if(pico_record::recordStartsWithBEGKEY(record_read_from_file))
                {
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
            try{
                //this function will read over the file and gets all the first records that are starting with  either BEGKEY or CONKEY and return them as pico_records not offsets
                
                offsetType endOfFile_Offset = getEndOfFileOffset(file);
                mylogger << "\n offset of end of file is " << endOfFile_Offset;
                
                for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
                     pico_record::max_size) {
                    
                   
                    
                    pico_record record_read_from_file = retrieve(offset);
                     mylogger << "\n read_all_records_offsets : reading one record from offset "<<offset<<"\n the record read is "<<record_read_from_file.toString();
                    
                    if(pico_record::recordStartsWithBEGKEY(record_read_from_file))
                    {
                        list_.push_back(record_read_from_file);
                    }
                    
                }
            }catch (...) {
                std::cerr << "Exception: read_all_messages_records: unknown thrown" << "\n";
                raise(SIGABRT);
                
            }
            return list_;
        }
        
        
        
        void read_offset_of_this_record(pico_record& record)
        {
            nodeType node = index.search(record);
            if(node==nullptr) record.offset_of_record =  -1;
            else
            {
                record.offset_of_record = node->offset;
                
            }
            
        }
        
        
        //deprecate this function and replace it with one that finds the record in the index
        //	list<offsetType> read_all_offsets_that_match_this_record(pico_record& record) {//debugged
        //        //this function should use the index , and not iterate through the file
        //
        //        //this function finds all the records that are the same as this record
        //		list<offsetType> list_of_offsets;
        //		offsetType endOfFile_Offset = getEndOfFileOffset(file);
        //		cout << " offset of end of file is " << endOfFile_Offset //<< std::endl;
        //
        //		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
        //				pico_record::getRecordSize()) {
        //            pico_record record_read_from_file = retrieve(offset);
        //
        //
        //			if (!pico_record::recordIsEmpty(record_read_from_file) && record_read_from_file == record) {
        //				//add the offset to the list of offset
        //				list_of_offsets.push_back(offset);
        //				cout << " read_all_offsets_that_match_this_record :  records match" //<< std::endl;
        //                //as all the first records are unique in the collection we can break out of the loop
        //                break;
        //			}
        //
        //
        //
        //		}
        //		return list_of_offsets;
        //	}
        
        bool ifRecordExists(pico_record& record){
            //this function checks if a record exists or not
            
            if(index.search(record)==nullptr)
                return false;
            
            return true;
            
            
            //this is the old version of funciton
            //        list<offsetType> allRecords = read_all_offsets_that_match_this_record(record);
            //        if(allRecords.size()>0) return true;
            //        return false;
        }
        void deleteOneRecord(offsetType offsetOfToBeDeletedRecord) {
            if(offsetOfToBeDeletedRecord==-1) return;
            
            string empty("");
            pico_record empty_record;
//            (empty,empty);
            pico_record current_record = retrieve(offsetOfToBeDeletedRecord);
            //        nodeType node = index.createANodeBasedOnOffset(offset);
            //		index.deleteNode(node);
            
            overwrite(empty_record,offsetOfToBeDeletedRecord);
            // index.remove(*index.convert_pico_record_to_index_node(current_record));//passing the key of the record that was deleted
            //to calculate the right
            
            
        }
        offsetType get_offset_of_this_record(pico_record& record)
        {//this function is used in updating and replacing
            //this function should use the index to get the offset of the record
            //which is always the first record of the message
            //iterating through all the records in the file should be avoided
            // at all the times. and index should be found...
            nodeType node = index.search(record);
            if(node==nullptr) return -1;
            else return  node->offset;
            
            
            //old version of the function
            //        offsetType endOfFile_Offset = getEndOfFileOffset(file);//this function is expensive,
            //        //we should have a class variable to store endOfFilleOffset
            //
            //        cout << " get_offset_of_this_record : offset of end of file is " << endOfFile_Offset //<< std::endl;
            //
            //		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
            //             pico_record::getRecordSize()) {
            //            pico_record record_read_from_file = retrieve(offset);
            //
            //
            //			if (!pico_record::recordIsEmpty(record_read_from_file) && record_read_from_file == this_record) {
            //				cout << " get_offset_of_this_record :  records match" //<< std::endl;
            //                return offset;
            //
            //			}
            //        }
            //        return -1;
            
        }
        void overwrite(pico_record record,offsetType record_offset) { //this overwrites a file
            
            mylogger << "\noverwriting  one record to collection at this offset\n";

            std::unique_lock<std::mutex> writeLock(writeMutex);
            do
            {
                //this while loop will take care of multi threaded delete
                file.seekp(record_offset);
                file.write((char*) record.data_, pico_record::max_size);
                file.flush();
                pico_record  currentRecord =retrieve(record_offset);
                if(currentRecord.areRecordsEqual(record))
                {
                    break;
                }
                else{
                    
                    mylogger<<"overwrite didnt work on offset "<<record_offset<<"\n"<<" currentRecord.getKeyAsString() is "<<currentRecord.getKeyAsString()<<" vs record.getKeyAsString is "<<record.getKeyAsString() << "currentRecord.getValueAsString() is "<<currentRecord.getValueAsString()<<" vs record.getValueAsString() is "<<record.getValueAsString() ;
                }
            }while(true);
        }
        void insert(pico_record& record) { //this appends to the end of file
            append(record);
        }
        void append(pico_record& record) { //this appends to the end of file
            offsetType record_offset = getEndOfFileOffset(file);
            record.offset_of_record = record_offset;
            append_a_record(record,record_offset);
            
        }
        void append_a_record(pico_record& record,offsetType record_offset)
        {
            
            mylogger << "\nappending  one record to collection at this offset record_offset : "<<record_offset<<" \n";
            mylogger << "appending  one record key is :  "<<record.getKeyAsString()<<" \n";
            mylogger << "appending one record value is :  "<<record.getValueAsString()<<" \n";
            
            if(record_offset==-1) record_offset=0;
            
            std::unique_lock<std::mutex> writeLock(writeMutex);
            file.seekp(record_offset,ios_base::beg);
            file.write((char*) record.data_, pico_record::max_size);
            file.flush();
            if(pico_record::recordStartsWithBEGKEY(record))
            {
                index.add_to_tree(record);
            }
            
        }
        
        ~pico_collection() {
            //		outfile.flush();
            //		infile.close();
            //		outfile.close();
            cout<<" pico_collection destructor being called..\n";
            if (file.good()) { file.close();}
        }
        
        const pico_record empty_record;
    private:
        
        //	std::ifstream infile;
        //	std::ifstream outfile;
        std::fstream file;
        
        std::string filename;
    };
    
    void DeleteTaskRunnable::run() {
        std::string str;
        str.append("DeleteTask Runnable is running ... by a thread  with id : ");
        str.append(convertToString<boost::thread::id>(boost::this_thread::get_id()));
        
        
        collection->deletion_function(record);
        numberOfoutputs++;
        long  x = numberOfoutputs.load(std::memory_order_relaxed);

        mylogger<<" this is the num of deleted messages from collection : "<<x;
        
    }
    
}
#endif /* COLLECTION_H_ */

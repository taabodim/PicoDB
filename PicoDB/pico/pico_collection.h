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
#include <pico/pico_buffer.h>
#include <pico/pico_index.h>
#include <logger.h>
//this is a wrapper around the file that represents the collection
namespace pico {
    
class pico_collection {
    //add this to some logic that doesnt call other functions who have this
    //boost::interprocess::scoped_lock<boost::mutex> lock(collectionMutex)
public:
    //logger mylogger;
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
       // std::cout<<("pico_collection : name of the file is "<<filename<<std::endl;
//		infile.open(name, std::fstream::in | std::fstream::binary);
//		outfile.open(name,
//				std::fstream::out | std::fstream::app | std::fstream::binary);
        file.open(name,	std::fstream::in |	std::fstream::out | std::fstream::binary);
        //use the fstream for both reading and writing and appending , there should be
        //only one stream open to the file, it makes the life easier and less buggier.
        //writing in out mode,
        //if i do app mode, the seekp wont work properly
        

       // test_reading_from_collection();
        
		list<pico_record> all_pico_messages = read_all_messages_records();//write a function to get all the begining records for putting them in the tree
		index.build_tree(all_pico_messages);

	}
   
//    list<offsetType> get_All_Offsets_InAFile()
//    {
//        list<offsetType> alloffsets;
//        offsetType lastOffset =getEndOfFileOffset();
//        std::cout<<" pico_collection : lastOffset is  "<<lastOffset<<endl;
//        for(int i=0;i<lastOffset; i=i+ pico_record::max_size)
//        {
//             std::cout<<" pico_collection : one Offset is  "<<i<<endl;
//            alloffsets.push_back(i);
//        }
//        return alloffsets;
//    }
	bool collectionExists(const std::string& name) {
		std::fstream f;
		f.open(name, std::fstream::in);
		if (f.good()) {
			f.close();
			return true;
		} else {
			f.close();
			return false;
		}
	}

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
    
    }
	void deleteRecord(pico_record& firstRecordOfMessageToBeDeleted) {
        //this function gets called by request processor, it finds the first record of the message
        //that has the same key and delete all the records that follows that first record until the next
        //first record in the file
        
		
		//delete this record all over the file , logically we should only have one record with the same key
        //when we are inserting to the file, we check if the record exists , we update it( the update is just deleting and inserting again)
	
        index.remove(*index.convert_pico_record_to_index_node(firstRecordOfMessageToBeDeleted));
        queue_record_for_deletion(firstRecordOfMessageToBeDeleted);
        
      

	}
    void deletion_function(pico_record firstRecordOfMessageToBeDeleted)//this function is the main function that deletion thread calls to delete the record
    {
    
        list<offsetType> list_of_offset = read_all_offsets_that_match_this_record(firstRecordOfMessageToBeDeleted);
		while (!list_of_offset.empty()) {
            
			long offsetOfFirstRecordOfMessage = list_of_offset.front();
            std::cout << "  offset in the list is  " << offsetOfFirstRecordOfMessage << endl;
			list_of_offset.pop_front();
            
			deleteOneMessage(offsetOfFirstRecordOfMessage);
            //this should be done in a seperate thread
            //to boost performance, and deleteRecord function should delete the node in index and queue the record
            //for delete
            
		}
    
    }
    void deleteOneMessage(offsetType offsetOfFirstRecordOfMessage)
    //this function deletes all the records of a message starting from the first one
    //until the next "first record" is found
    {
        list<offsetType> all_offsets_for_this_message;
        all_offsets_for_this_message.push_back(offsetOfFirstRecordOfMessage);
        
        offsetType nextOffset=offsetOfFirstRecordOfMessage;
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
        } while(nextOffset<=getEndOfFileOffset());
        
            
        while(!all_offsets_for_this_message.empty())
        {
            deleteOneRecord(all_offsets_for_this_message.front());
            all_offsets_for_this_message.pop_front();
        }
    
    
    }
  
        list<pico_record> find(pico_record& firstRecordOfMessageToBeFound) {
        
		list<pico_record> all_records;
		list<offsetType> list_of_offset = read_all_offsets_that_match_this_record(firstRecordOfMessageToBeFound);
		
        while (!list_of_offset.empty()) {

			offsetType offset = list_of_offset.front();
			cout << "  offset in the list is  " << offset << endl;
			list_of_offset.pop_front();
			pico_record record = retrieve(offset);
			all_records.push_back(record);
		}
		return all_records;
	}
	pico_record retrieve(offsetType offset) {//was debugged
        //it seems if if I use the global infile
        //and when I use it in other functions, it stops working here , it wont read the data here , so I am using the infileLocal ,
        std::ifstream infileLocal;
        infileLocal.open(filename, std::fstream::in | std::fstream::binary);
        cout << " pico_collection  : retrieve : offset is  " << offset<< std::endl;
		pico_record record_read_from_file;
        
        infileLocal.seekg(offset);
        infileLocal.read((char*) record_read_from_file.key_,
                    pico_record::max_key_size);
        infileLocal.read((char*) record_read_from_file.value_,
                    pico_record::max_value_size);
        cout << " read_all_records : record_read_from_file.getKeyAsString() " << record_read_from_file.getKeyAsString()<< std::endl;
        infileLocal.close();
		return record_read_from_file;
        

		return record_read_from_file;
	}
//    list<pico_record> read_all_records() { //this function was debugged!
//        
//		list<pico_record> list_of_records;
//		offsetType endOfFile_Offset = getEndOfFileOffset();
//		cout << "read_all_records : offset of end of file is " << endOfFile_Offset << std::endl;
//		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
//             pico_record::getRecordSize()) {
//            cout << " read_all_records : reading one record from offset "<<offset  << std::endl;
//            
//			pico_record record_read_from_file = retrieve(offset);
//				if (!record_read_from_file.getKeyAsString().empty()) {
//                
//				list_of_records.push_back(record_read_from_file);
//			}
//            else{
//                std::cout<<("warning : read_all_records : key is empty!");
//            }
//            
//		}
//        return list_of_records;
//	}

    
	list<offsetType> read_all_Messages_offsets() {
        //this function will read over the file and gets all the first records that are starting with  either BEGKEY or CONKEY
		list<offsetType> list_of_offsets;
		offsetType endOfFile_Offset = getEndOfFileOffset();
		cout << " offset of end of file is " << endOfFile_Offset << std::endl;
		
        for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
				pico_record::max_size) {
            cout << " read_all_records_offsets : reading one record from offset "<<offset  << std::endl;
            
            pico_record record_read_from_file = retrieve(offset);

            
            if(pico_record::recordStartsWithBEGKEY(record_read_from_file))
            {
				list_of_offsets.push_back(offset);
			}
//            
//            else{
//                std::cout<<("warning : read_all_records_offsets : key is empty!");
//            }
//                
		}
        return list_of_offsets;
	}

	list<pico_record> read_all_messages_records() {
        //this function will read over the file and gets all the first records that are starting with  either BEGKEY or CONKEY and return them as pico_records not offsets
		list<pico_record> list_;
		offsetType endOfFile_Offset = getEndOfFileOffset();
		cout << " offset of end of file is " << endOfFile_Offset << std::endl;
		
        for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
             pico_record::max_size) {
            cout << " read_all_records_offsets : reading one record from offset "<<offset  << std::endl;
            
            pico_record record_read_from_file = retrieve(offset);
            
            
            if(pico_record::recordStartsWithBEGKEY(record_read_from_file))
            {
				list_.push_back(record_read_from_file);
			}
            
		}
        return list_;
	}
    

    
    offsetType read_offset_of_this_record(pico_record& record)
    {
        return index.search(record)->offset;
        
    }
    
    
    //deprecate this function and replace it with one that finds the record in the index
	list<offsetType> read_all_offsets_that_match_this_record(pico_record& record) {//debugged
        //this function should use the index , and not iterate through the file
        
        //this function finds all the records that are the same as this record
		list<offsetType> list_of_offsets;
		offsetType endOfFile_Offset = getEndOfFileOffset();
		cout << " offset of end of file is " << endOfFile_Offset << std::endl;

		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
				pico_record::getRecordSize()) {
            pico_record record_read_from_file = retrieve(offset);


			if (!pico_record::recordIsEmpty(record_read_from_file) && record_read_from_file == record) {
				//add the offset to the list of offset
				list_of_offsets.push_back(offset);
				cout << " read_all_offsets_that_match_this_record :  records match" << std::endl;
                //as all the first records are unique in the collection we can break out of the loop
                break;
			}
            
            

		}
		return list_of_offsets;
	}

    bool ifRecordExists(pico_record& record){
    //this function checks if a record exists or not
        list<offsetType> allRecords = read_all_offsets_that_match_this_record(record);
        if(allRecords.size()>0) return true;
        return false;
    }
	void deleteOneRecord(offsetType offsetOfToBeDeletedRecord) {
        
		string empty("");
        pico_record empty_record(empty,empty);
        pico_record current_record = retrieve(offsetOfToBeDeletedRecord);
        //        std::shared_ptr<pico_record_node> node = index.createANodeBasedOnOffset(offset);
//		index.deleteNode(node);
		overwrite(empty_record,offsetOfToBeDeletedRecord);
        index.remove(*index.convert_pico_record_to_index_node(current_record));//passing the key of the record that was deleted
        //to calculate the right 
        

	}
    offsetType get_offset_of_this_record(pico_record& this_record)
    {//this function is used in updating and replacing
        //this function should use the index to get the offset of the record
        //which is always the first record of the message
        //iterating through all the records in the file should be avoided
        // at all the times. and index should be found...
    
        offsetType endOfFile_Offset = getEndOfFileOffset();//this function is expensive,
        //we should have a class variable to store endOfFilleOffset
        
        cout << " get_offset_of_this_record : offset of end of file is " << endOfFile_Offset << std::endl;
        
		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
             pico_record::getRecordSize()) {
            pico_record record_read_from_file = retrieve(offset);
            
            
			if (!pico_record::recordIsEmpty(record_read_from_file) && record_read_from_file == this_record) {
				cout << " get_offset_of_this_record :  records match" << std::endl;
                return offset;
				
			}
        }
        return -1;
        
    }
    void overwrite(pico_record& record,offsetType record_offset) { //this overwrites a file
        
        cout << "overwriting  one record to collection at this offset record_offset : "<<record_offset<<" \n";
        file.seekp(record_offset);
        file.write((char*) record.getkey(), record.max_key_size);
		file.write((char*) record.getValue(), record.max_value_size);
        file.flush();
	}
    void insert(pico_record& record) { //this appends to the end of file
        append(record);
    }
	void append(pico_record& record) { //this appends to the end of file
    	offsetType record_offset = getEndOfFileOffset();
        record.offset_of_record = record_offset;
		append_a_record(record,record_offset);
       
   }
    void append_a_record(pico_record& record,offsetType record_offset)
    {
        cout << "appending  one record to collection at this offset record_offset : "<<record_offset<<" \n";
        file.seekp(record_offset,ios_base::beg);
        file.write((char*) record.getkey(), record.max_key_size);
		file.write((char*) record.getValue(), record.max_value_size);
        file.flush();
        index.add_to_tree(record);
        
    }

	offsetType getEndOfFileOffset() {//was debugged
        
   		file.seekg(0, std::ifstream::end);
        std::cout<<(" getEndOfFileOffset : filename is ");
        std::cout<<filename<<std::endl;
        std::cout<<" getEndOfFileOffset : file.tellg() :  "<<file.tellg()<<std::endl;
        
        return file.tellg();
        
	}
   
 
    
	~pico_collection() {
//		outfile.flush();
//		infile.close();
//		outfile.close();
        file.close();
	}

	const pico_record empty_record;
private:

//	std::ifstream infile;
//	std::ifstream outfile;
	std::fstream file;
   
	std::string filename;
};
}
#endif /* COLLECTION_H_ */

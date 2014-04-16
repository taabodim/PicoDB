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
	pico_binary_index_tree index_of_collection;
	pico_collection() = delete;
	pico_collection(std::string name) {

//		if (collectionExists(name)) {
//			std::cout << "collection exists\n";
//			outfile.open(name, fstream::app );
//		} else {
//			std::cout << "collection doesnt exist\n";
//			outfile.open(name, fstream::app);
//		}
        string path("/Users/mahmoudtaabodi/Documents/");
		std::string ext(".txt");
		path.append(name);
        path.append(ext);
        name = path;
		filename = name;
       // std::cout<<("pico_collection : name of the file is "<<filename<<std::endl;
		infile.open(name, std::fstream::in | std::fstream::binary);
		outfile.open(name,
				std::fstream::out | std::fstream::app | std::fstream::binary);
       // test_reading_from_collection();
        
		list<pico_record> all_pico_messages = read_all_Messages_offsets();//write a function to get all the begining records for putting them in the tree
		index_of_collection.build_tree(all_pico_messages);

	}
    void test_reading_from_collection()
    {
        std::ifstream is (filename, std::ifstream::binary);
        if (is) {
            // get length of file:
            is.seekg (0, is.end);
            size_t length = is.tellg();
            is.seekg (0, is.beg);
            
            char * buffer = new char [length];
            
            std::cout << "Reading " << length << " characters... ";
            // read data as a block:
            is.read (buffer,length);
            
            if (is)
                std::cout << "all characters read successfully.";
            else
                std::cout << "error: only " << is.gcount() << " could be read";
            is.close();
            
            // ...buffer contains the entire file...
            
            delete[] buffer;
        }
    }
	offsetType getEndOfFileOffset() {//was debugged
       outfile.flush();
		
        std::fstream file;
		file.open(filename, std::fstream::in | std::fstream::binary);
		file.seekg(0, std::ifstream::end);
//        std::cout<<(" getEndOfFileOffset : filename is ");
//        std::cout<<(<<filename<<std::endl;
//        std::cout<<(" getEndOfFileOffset : file.tellg() :  "<<file.tellg()<<std::endl;
        
        return file.tellg();

	}
    list<offsetType> get_All_Offsets_InAFile()
    {
        list<offsetType> alloffsets;
        offsetType lastOffset =getEndOfFileOffset();
        std::cout<<" pico_collection : lastOffset is  "<<lastOffset<<endl;
        for(int i=0;i<lastOffset; i=i+ pico_record::max_size)
        {
             std::cout<<" pico_collection : one Offset is  "<<i<<endl;
            alloffsets.push_back(i);
        }
        return alloffsets;
    }
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
    	list<offsetType> all_Messages_offsets = read_all_Messages_offsets();
		return all_Messages_offsets.size();
	}

	string getName() {
		return filename;
	}
	offsetType getEndOfFileOffsett() {
 		infile.seekg(0, std::ifstream::end);
		return infile.tellg();
	}
	pico_record get(int index) {
   		pico_record record;
		offsetType offset = index * record.max_size;
		if (offset > getEndOfFileOffsett())
			return empty_record;
		infile.seekg(offset);
		infile.read((char*) record.key_, record.max_key_size);
		infile.read((char*) record.value_, record.max_value_size);
		return record;
	}
	void deleteRecord(pico_record& firstRecordOfMessageToBeDeleted) {
        
		
		//delete this record all over the file
		list<offsetType> list_of_offset = read_all_offsets_that_match_this_record(firstRecordOfMessageToBeDeleted);
		while (!list_of_offset.empty()) {

			long offsetOfFirstRecordOfMessage = list_of_offset.front();
            std::cout << "  offset in the list is  " << offsetOfFirstRecordOfMessage << endl;
			list_of_offset.pop_front();
            
			deleteOneMessage(offsetOfFirstRecordOfMessage);
		}

	}
    void deleteOneMessage(offsetType offsetOfFirstRecordOfMessage)
    
    {
        list<offsetType> all_offsets_for_this_message;
        all_offsets_for_this_message.push_back(offsetOfFirstRecordOfMessage);
        
        offsetType nextOffset=-1;
       do
        {
         nextOffset = offsetOfFirstRecordOfMessage+ pico_record::max_size;
        pico_record  nextRecord = retrieve(nextOffset);
        if(recordStartsWithConKEY(nextRecord))
            {
                all_offsets_for_this_message.push_back(nextOffset);
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
	pico_record retrieve(offsetType offset) {
        

		pico_record record_read_from_file;
		infile.seekg(offset);
		infile.read((char*) record_read_from_file.key_,
				record_read_from_file.max_key_size);
		infile.read((char*) record_read_from_file.value_,
				record_read_from_file.max_value_size);
		return record_read_from_file;
	}
    list<pico_record> read_all_records() { //this function was debugged!
        
		list<pico_record> list_of_records;
		offsetType endOfFile_Offset = getEndOfFileOffset();
		cout << "read_all_records : offset of end of file is " << endOfFile_Offset << std::endl;
		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
             pico_record::getRecordSize()) {
            cout << " read_all_records : reading one record from offset "<<offset  << std::endl;
            
			pico_record record_read_from_file;
			infile.seekg(offset);
			infile.read((char*) record_read_from_file.key_,
                        pico_record::max_key_size);
			infile.read((char*) record_read_from_file.value_,
                        pico_record::max_value_size);
            cout << " read_all_records : record_read_from_file.getKeyAsString() " << record_read_from_file.getKeyAsString()<< std::endl;
            
            
			if (!record_read_from_file.getKeyAsString().empty()) {
                
				list_of_records.push_back(record_read_from_file);
			}
            else{
                std::cout<<("warning : read_all_records : key is empty!");
            }
            
		}
        return list_of_records;
	}

    
	list<offsetType> read_all_Messages_offsets() {

		list<offsetType> list_of_offsets;
		offsetType endOfFile_Offset = getEndOfFileOffset();
		cout << " offset of end of file is " << endOfFile_Offset << std::endl;
		
        for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
				pico_record::max_size) {
            cout << " read_all_records_offsets : reading one record from offset "<<offset  << std::endl;
            
			pico_record record_read_from_file;
			infile.seekg(offset);
			infile.read((char*) record_read_from_file.key_,
                        pico_record::max_key_size);
            
//            
//			infile.read((char*) record_read_from_file.value_,
//                        pico_record::max_value_size);
//            cout << " read_all_records_offsets : record_read_from_file.getKeyAsString() " << record_read_from_file.getKeyAsString()<< std::endl;
            
            if(recordStartsWithBEGKEY(record_read_from_file))
            {
				list_of_offsets.push_back(offset);
			}
            else{
                std::cout<<("warning : read_all_records_offsets : key is empty!");
            }
                
		}
        return list_of_offsets;
	}

    bool recordStartsWithBEGKEY(pico_record& currentRecord)
    {
    if(currentRecord.key_[0]=='B' &&
       currentRecord.key_[1]=='E' &&
       currentRecord.key_[2]=='G' &&
       currentRecord.key_[3]=='K' &&
       currentRecord.key_[4]=='E' &&
       currentRecord.key_[5]=='Y')
        return true;
        
        return false;
    }
    
    bool recordStartsWithConKEY(pico_record& currentRecord)
    {
        if(currentRecord.key_[0]=='C' &&
           currentRecord.key_[1]=='O' &&
           currentRecord.key_[2]=='N' &&
           currentRecord.key_[3]=='K' &&
           currentRecord.key_[4]=='E' &&
           currentRecord.key_[5]=='Y')
            return true;
        
        return false;
    }
	list<offsetType> read_all_offsets_that_match_this_record(pico_record& record) {

		list<offsetType> list_of_offsets;
		offsetType endOfFile_Offset = getEndOfFileOffset();
		cout << " offset of end of file is " << endOfFile_Offset << std::endl;

		for (offsetType offset = 0; offset <= endOfFile_Offset; offset +=
				pico_record::getRecordSize()) {
			pico_record record_read_from_file;
			infile.seekg(offset);
			infile.read((char*) record_read_from_file.key_,
					record_read_from_file.max_key_size);
//			infile.read((char*) record_read_from_file.value_,
//					record_read_from_file.max_value_size);
			cout << " before comparing the records" << std::endl;

			if (record_read_from_file == record) {
				//add the offset to the list of offset
				list_of_offsets.push_back(offset);
				cout << " records match" << std::endl;

			}

		}
		return list_of_offsets;
	}
	void update(pico_record& old_record, pico_record& new_record) {
        
		list<offsetType> list_of_offset = read_all_offsets_that_match_this_record(old_record);
		while (!list_of_offset.empty()) {

			offsetType offset = list_of_offset.front();
			cout << "  offset in the list is  " << offset << " size of list is "
					<< list_of_offset.size() << endl;
			list_of_offset.pop_front();
			updateOneRecord(offset, new_record);
		}
	}
	size_t update(pico_record& record) {
        
		list<offsetType> list_of_offset = read_all_offsets_that_match_this_record(record);
		size_t num = 0;
		while (!list_of_offset.empty()) {

			offsetType offset = list_of_offset.front();
			cout << "  offset in the list is  " << offset << " size of list is "
					<< list_of_offset.size() << endl;
			list_of_offset.pop_front();
			updateOneRecord(offset, record);
			num++;
		}

		return num;
	}
	void updateOneRecord(offsetType record_offset, pico_record& new_record) {
        
		cout << "updating a record at offset " << record_offset << endl;
		outfile.seekp(record_offset);
		new_record.offset_of_record = record_offset;
		outfile.write((char*) new_record.getkey(), new_record.max_key_size);
		outfile.write((char*) new_record.getValue(), new_record.max_value_size);
		outfile.flush();
	}
  
	void deleteOneRecord(offsetType offsetOfFirstRecordOfMessage) {
        
		long firstOffset = outfile.tellp();
		outfile.seekp(offsetOfFirstRecordOfMessage, ios::beg);
		long offsetToWrite = outfile.tellp();
		std::cout << " one record was deleted offset is " << firstOffset
				<< " tellp is " << offsetToWrite << " firstOffset is "
				<< firstOffset << "\n";
		pico_record empty_record;
//        std::shared_ptr<pico_record_node> node = index_of_collection.createANodeBasedOnOffset(offset);
//		index_of_collection.deleteNode(node);
		insert(empty_record);

	}

	void insert(pico_record& record) {
        
		cout << "inserting to collection\n";
		size_t record_offset = outfile.tellp();
		outfile.seekp(record_offset);
		record.offset_of_record = record_offset;
		outfile.write((char*) record.getkey(), record.max_key_size);
		outfile.write((char*) record.getValue(), record.max_value_size);
       
		outfile.flush();
         index_of_collection.add_to_tree(record);
	}
	void insert(pico_record& record, int index) {
        
		size_t record_offset = index * record.max_size;
		outfile.seekp(record_offset);
		record.offset_of_record = record_offset;
		outfile.write((char*) record.getkey(), record.max_key_size);
		outfile.write((char*) record.getValue(), record.max_value_size);
		outfile.flush();
         index_of_collection.add_to_tree(record);
	}

	~pico_collection() {
		outfile.flush();
		infile.close();
		outfile.close();
	}

	const pico_record empty_record;
private:

	std::ifstream infile;
	std::ofstream outfile;
	std::string filename;
};
}
#endif /* COLLECTION_H_ */

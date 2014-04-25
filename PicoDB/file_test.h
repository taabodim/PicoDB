//
//  file_test.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/18/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_file_test_h
#define PicoDB_file_test_h
#include <boost/filesystem/fstream.hpp>
#include <fstream>
#include <iostream>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;
using namespace pico;
using namespace std;

class file_test{
    
public:
    void writeToFileExample() {
        boost::filesystem::path p("text.txt");
        boost::filesystem::ofstream ofs(p);
        ofs << "hello mahmoud taabodi" << endl;
        
    }
    
    void getInfoOnDirectory() {
        boost::filesystem::path p("/");
        
    }
    
    void getSizeOfFile() {
        
        //std::cout << "gmon.out  size is " << " " << file_size("gmon.out") << '\n';
    }
    
    
    void writeBinaryExample() {
        //	 streampos size;
        //	  char * memblock;
        //
        //	  ifstream file ("example.bin", ios::in|ios::binary|ios::ate);
        //	  if (file.is_open())
        //	  {
        //	    size = file.tellg();
        //	    memblock = new char [size];
        //	    file.seekg (0, ios::beg);
        //	    file.read (memblock, size);
        //	    file.close();
        //
        //	    cout << "the entire file content is in memory";
        //
        //	    delete[] memblock;
        //	  }
        //	  else cout << "Unable to open file";
    }
    
    void fileExamples() {
        string p = "gmon.out";
        if (exists(p))    // does p actually exist?
        {
            if (is_regular_file(p))        // is p a regular file?
                cout << p << " size is " << file_size(p) << '\n';
            
            else if (is_directory(p))      // is p a directory?
                cout << p << "is a directory\n";
            
            else
                cout << p
                << "exists, but is neither a regular file nor a directory\n";
        } else
            cout << p << "does not exist\n";
        
    }
    void binaryFileExample() {
        writeBinaryExample();
    }
    
    void write10Records(){
        
         std::ofstream overwriter;
        overwriter.open("/Users/mahmoudtaabodi/Documents/test-writing-file.dat", ios::out| std::fstream::binary);
      	overwriter.seekp(25);
        size_t tellP = overwriter.tellp();
        cout << "write10Records : overwriting going to writer at this tellp "<<tellP<<" \n";
		
		
        string key("this is the first key0");
        string value("this is the first value0");
        
  		overwriter.write((char*) key.c_str(), 50);
		overwriter.write((char*) value.c_str(), 50);
        
        
        
		overwriter.flush();
        overwriter.close();
    }
    void updateSomeRecords(){
    }
    void writeInSpecificOffsets(){
    }
    void deleteSomeFile(){
    }
    void deleteSomeRecords(){
    
    }
    
    void deadCode()
    
    {
        
        //		size_t record_offset = offsetOfFirstRecordOfMessage;
        //        std::ofstream overwriter;
        //        overwriter.open(filename, ios::out | std::fstream::binary);
        //        cout << "overwriting one record to collection at this offset record_offset : "<<record_offset<<" \n";
        //		overwriter.seekp(record_offset);
        //        size_t tellP = overwriter.tellp();
        //        cout << "overwriting going to writer at this tellp "<<tellP<<" \n";
        //
        //		record.offset_of_record = record_offset;
        //  		overwriter.write((char*) record.getkey(), record.max_key_size);
        //		overwriter.write((char*) record.getValue(), record.max_value_size);
        //
        //		//overwriter.flush();
        //        overwriter.close();
        //        //         index_of_collection.add_to_tree(record); //fix this part it has a bug
        //        
        //        
        
    
    }
};

#endif

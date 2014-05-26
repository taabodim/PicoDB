/*
 * pico_record.h
 *
 *  Created on: Mar 13, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_RECORD_H_
#define PICO_RECORD_H_

#include <memory>
#include <logger.h>
#include <pico_logger_wrapper.h>
#include <stdio.h>
#include <string.h>
using namespace std;
namespace pico {
    class pico_record: public pico_logger_wrapper {
        //use n=memcmp ( buffer1, buffer2, sizeof(buffer1) ); use this to compare if records are equal
    public:
        
        char* data_;
        
        offsetType offset_of_record = -1; //this is the offset in the file and the index
        
        pico_record() {
            
            offset_of_record = -1;
        }
        
        pico_record(string json_form_of_message)
        {
            data_ = new char[json_form_of_message.size()];
            const char* msgPtr = json_form_of_message.c_str();
            for(int i=0;i<json_form_of_message.size();i++)
                
            {
                data_[i] = *(msgPtr+i);
            }
            this->dataSize =json_form_of_message.size();
            offset_of_record = -1;
            
        }
        pico_record(const pico_record& record)
        {
            memcpy(this->data_, record.data_, sizeof(record.data_)); //get a fresh copy of data to make sure its not touched
            this->offset_of_record =  record.offset_of_record;
            this->dataSize =record.dataSize;
            
            
        }
        std::string getDataAsString() //to write to other side
        {
            assert(sizeof(data_)>0);
            char data_copy[sizeof(data_)];
            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
            //by copying into the other string or assigning to other
            std::string data(data_copy, sizeof(data_));
            assert(!data.empty());
            
            return data;
        }
        std::string toJson() {
            
            assert(sizeof(data_)>0);
            char data_copy[sizeof(data_)];
            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
            //by copying into the other string or assigning to other
            std::string data(data_copy, sizeof(data_));
            
            string str;
            std::string keyStr("{ ");
            std::string dataStr("data : ");
            std::string dataValue(data_copy, sizeof(data_));
            
            std::string end(" }");
            
            str.append(keyStr);
            str.append(dataStr);
            str.append(dataValue);
            str.append(end);
            return str;
        }
        std::string str;
        char* dataCopy_ ;
        std::string toString() {
            assert(sizeof(data_)>0);
            
            
            //get a fresh copy of data to make sure its not touched
            //by copying into the other string or assigning to other
            str.clear();
            int arraySize = dataSize;
            dataCopy_= new char[arraySize];
            memcpy(dataCopy_, data_, arraySize);
            //
            for(int i=0;i<dataSize;i++)
            {
                str.push_back(*dataCopy_);
                ++dataCopy_;
            }
            assert(!str.empty());
            assert(str.size()==dataSize);
            return str;
        }
        
        string dataString;
        string getDataForWrite() { //data is loaded to write
            if(dataString.empty())
            {
                string temp(data_);
                dataString.append(temp);
            }
            assert(dataString.size()>2);
            return dataString;
        }
        int getDataSizeForWrite() { //data size that is going to be written
            
            return dataSize;
        }
        
        int dataSize;
        
        char* getDataForRead(int dataSizeToRead) { //data is going to be loaded
            dataSize =dataSizeToRead;
            data_ = new char[dataSizeToRead];
            return data_;
        }
        
        size_t getSize() { //it is used !
            return dataSize;
        }
        typedef std::shared_ptr<pico_record> recordTypePtr;
        
        
        static pico_record* emptyInstance() //for template class
        {
            pico_record* emptyRecordPtr (new pico_record());//fix the leak later
            return emptyRecordPtr;
            //resource leak
        }
        static bool IsThisRecordASizeInfo(std::shared_ptr<pico_record> record)
        {
            for(int i=0;i<record->getSize();i++)
            {
                if(record->data_[i]=='#')
                {
                    if(i+4<record->getSize())
                    {
                        if(record->data_[i+1]=='#'
                           && record->data_[i+2]=='#'
                           && record->data_[i+3]=='#'
                           )
                        {
                            return true;
                        }
                    }
                }
            }
            
            return false;
        }
        char getChar(int i)
        {
            return *(data_+i);
        }
    private:
        
    }
    ;
}
#endif /* PICO_RECORD_H_ */

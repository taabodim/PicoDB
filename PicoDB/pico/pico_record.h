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
using namespace std;
namespace pico {
    class pico_record : public pico_logger_wrapper{
       
       
    public:
        
        const static int max_value_size = 256;//should be more than 168 as the smallest message is that
        const static int max_key_size = 32;
        const static int max_size = max_key_size + max_value_size;
        long previous_record_offset; //if its -1, it means that there is no parent, if its more than -1, it indicates that it is add on to previous record
        
        char data_[max_size];

        
        
                std::string parentMessageId; //this is the id of the parent that
        //        //this buffer belongs to
                long parentSequenceNumber; //this is the number ,that defines
        offsetType offset_of_record; //this is the offset in the file and the index
        offsetType previousRecordOffset;
        //logger mylogger;
        // Construct from a std::string.
        void setValue(const std::string  realData) {
            char value[max_value_size];
            string data = realData.substr();
            std::size_t length = data.copy(value,data.size(),0);
            // data[length]='\0';
            
            //		strncpy(value_, data.c_str(), data.size());
            value[data.size()] = '\0';
            
            for (size_t i = 0; i < max_value_size; i++) {
                data_[i+max_key_size] = value[i];
            }
            
        }
//        
        void setKeyValue(std::string  key, std::string  value) {
            setKey(key);
            setValue(value);
        }
//
//        void setData(std::string  key,std::string  value) {
//            
//            for (int i = 0; i < max_key_size; i++) {
//                this->data_[i] = this->key_[i];
//            }
//            for (int i = max_key_size; i < max_value_size+max_key_size; i++) {
//                this->data_[i] = this->value_[i] ;
//            }
//            
//        }
        void setKey(const std::string  realData) {
            char key[max_key_size];

            string data = realData.substr();
            data.copy(key,data.size(),0);
            //  data[length]='\0';
            //strncpy(key_, data.c_str(), data.size());
            key[data.size()] = '\0';
            
            for (size_t i = 0; i < max_key_size; i++) {
                data_[i] = key[i];
            }
            
        }
        
        explicit pico_record(std::string key, std::string value) {
            setKey(key);
            setValue(value);
            
        }

        pico_record() {
            
            for (int i = 0; i < max_size; i++) {
                data_[i] = '\0';
            }
        }
        
        pico_record(const pico_record&  buffer) {
            
            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
                      std::begin(this->data_));
            
            this->offset_of_record = buffer.offset_of_record;
            
        }
        
        
        pico_record operator=(pico_record&  buffer) {
            
            
            
            mylogger << "pico_record copy assigned\n";
            
            
            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
                      std::begin(this->data_));
            
             this->offset_of_record = buffer.offset_of_record;
            
            return *this;
        }
        pico_record operator=(pico_record&&   buffer) { //move assignment
            mylogger << "pico_record move assigned\n";
            
            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
                      std::begin(this->data_));
            
            this->offset_of_record = buffer.offset_of_record;
            
            return *this;
        }
        
        bool areValuesEqual(pico_record&  buffer) {
            for (int i = max_key_size; i < max_size; i++) {
                if(data_[i] !=buffer.data_[i])
                {
                    return false;
                }
            }
            
            return true;
            
        }
        bool areKeysEqual(pico_record&  buffer) {
            for (int i = 0; i < max_key_size; i++) {
                if(data_[i] !=buffer.data_[i])
                {
                    return false;
                }
            }
            
            return true;
            

        }
        void clear() {
            
            for (int i = 0; i < max_size; i++) {
                this->data_[i] = 0;
            }
        }
        
        
        char value_[max_value_size];//just for function below
        char* getValue() {
            for (int i = max_key_size; i < max_size; i++) {
                this->value_[i] = this->data_[i-max_key_size] ;
            }
            return value_;
        }
        
        char key_[max_key_size];//just for function below
        char* getkey() {
            for (int i = 0; i < max_key_size; i++) {
                this->key_[i] = this->data_[i] ;
            }
            
            return key_;
        }
        //this will mess up the end of value_ data !! ,, dangerous!!
        std::string getKeyAsString() {
            getkey();
            std::string key(key_,max_key_size);
            return key;
        }
        std::string getValueAsString() {
            //uses the constructor that takes char[]  and gives us
            //the string properly
            getValue();
            std::string val(value_,max_value_size);
            return val;
        }
        std::string getDataAsString()//to write to other side
        {
            std::string data(data_,max_size);
            return data;
        }
        std::string toJson()
        {
            string str;
            std::string keyStr("{ ");
            std::string data("data : ");
            std::string dataValue(data_,max_size);
            
            std::string end(" }");
            
            str.append(keyStr);
            str.append(data);
            str.append(dataValue);
            str.append(end);
            return str;
        }
        std::string toString() const {
            std::string str(data_,max_size);
            return str;
        }
        size_t getValueSize() {
            return max_value_size;
        }
        size_t getKeySize() {
            return max_key_size;
        }
        static size_t getRecordSize() {
            return max_size;
        }
        
        static bool recordStartsWithBEGKEY(pico_record& currentRecord)//was debugged
        {
            if(currentRecord.data_[0]=='B' &&
               currentRecord.data_[1]=='E' &&
               currentRecord.data_[2]=='G' &&
               currentRecord.data_[3]=='K' &&
               currentRecord.data_[4]=='E' &&
               currentRecord.data_[5]=='Y')
                return true;
            
            return false;
        }
        
        static bool recordStartsWithConKEY(pico_record& currentRecord)//debugged
        {
            if(currentRecord.data_[0]=='C' &&
               currentRecord.data_[1]=='O' &&
               currentRecord.data_[2]=='N' &&
               currentRecord.data_[3]=='K' &&
               currentRecord.data_[4]=='E' &&
               currentRecord.data_[5]=='Y')
                return true;
            
            return false;
        }
        
        static bool recordIsEmpty(pico_record& currentRecord)//debugged
        {
            if(currentRecord.data_[0]=='\0' &&
               currentRecord.data_[1]=='\0' &&
               currentRecord.data_[2]=='\0' &&
               currentRecord.data_[3]=='\0' &&
               currentRecord.data_[4]=='\0' &&
               currentRecord.data_[5]=='\0')
                return true;
            
            return false;
        }
        
        static void replicateTheFirstRecordKeyToOtherRecords(pico_record& firstRecord,pico_record&  currentRecord)
        {
            //except the first six key that should be CONKEY
            currentRecord.data_[0]='C';
            currentRecord.data_[1]='O';
            currentRecord.data_[2]='N';
            currentRecord.data_[3]='K';
            currentRecord.data_[4]='E';
            currentRecord.data_[5]='Y';
            
            for(int i=6;i<pico_record::max_key_size;i++)
            {
                currentRecord.data_[i]= firstRecord.data_[i];
            }
        }
        static void addKeyMarkerToFirstRecord(pico_record& firstRecord) //this argument has to be passed by ref
        {
            string keyMarker("BEGKEY"); //its the key that marks the key of the first record
            const char* keyArray = keyMarker.c_str();
            int i=0;
            while (*keyArray != 0) {
                firstRecord.data_[i]=*keyArray;
                ++i;
                ++keyArray;
            }//the key marker is put to first 6 letters of the first record
        }
        char* getDataForWrite() { //data is loaded to write
            return data_;
        }
        char* getDataForRead() { //data is going to be loaded
            return data_;
        }
        
               
        size_t getSize() {
            return max_size;
       }
       
              typedef std::shared_ptr<pico_record> recordTypePtr;
    private:
        
    };
}
#endif /* PICO_RECORD_H_ */

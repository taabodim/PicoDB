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
#include <pico/pico_buffer.h>
using namespace std;
namespace pico {
    class pico_record {
    public:
        
        offsetType offset_of_record; //this is the offset in the file and the index 
        offsetType previousRecordOffset;
        //logger mylogger;
        // Construct from a std::string.
        void setValue(const std::string  realData) {
            string data = realData.substr();
            std::size_t length = data.copy(value_,data.size(),0);
            // data[length]='\0';
            
            //		strncpy(value_, data.c_str(), data.size());
            value_[data.size()] = '\0';
            
            for (size_t i = data.size(); i < max_value_size; i++) {
                value_[i] = '\0';
            }
            
        }
        
        void setKeyValue(std::string  key, std::string  value) {
            setKey(key);
            setValue(value);
        }
        void setKey(const std::string  realData) {
            string data = realData.substr();
            std::size_t length = data.copy(key_,data.size(),0);
            //  data[length]='\0';
            //strncpy(key_, data.c_str(), data.size());
            key_[data.size()] = '\0';
            
            for (size_t i = data.size(); i < max_key_size; i++) {
                key_[i] = '\0';
            }
            
        }
        
        explicit pico_record(std::string key, std::string value) {
            setKey(key);
            setValue(value);
            
        }
        //	explicit pico_record(const char* key, const char* value) {
        //		std::string keystr(key);
        //		std::string valuestr(value);
        //		setKey(keystr);
        //		setValue(valuestr);
        //
        //	}
        pico_record() {
            
            for (int i = 0; i < max_value_size; i++) {
                value_[i] = '\0';
            }
            for (int i = 0; i < max_key_size; i++) {
                key_[i] = '\0';
            }
        }
        
        pico_record(const pico_record&  buffer) {
            
            std::copy(std::begin(buffer.key_), std::end(buffer.key_),
                      std::begin(this->key_));
            
            std::copy(std::begin(buffer.value_), std::end(buffer.value_),
                      std::begin(this->value_));
            
            this->offset_of_record = buffer.offset_of_record;
            
        }
        
        
        pico_record operator=(pico_record&  buffer) {
            
            
            
            //std::cout << "pico_record copy assigned\n";
            std::copy(std::begin(buffer.key_), std::end(buffer.key_),
                      std::begin(this->key_));
            
            std::copy(std::begin(buffer.value_), std::end(buffer.value_),
                      std::begin(this->value_));
            
             this->offset_of_record = buffer.offset_of_record;
            
            return *this;
        }
        pico_record operator=(pico_record&&   buffer) { //move assignment
            //std::cout << "pico_record move assigned\n";
            std::copy(std::begin(buffer.key_), std::end(buffer.key_),
                      std::begin(this->key_));
            
            std::copy(std::begin(buffer.value_), std::end(buffer.value_),
                      std::begin(this->value_));
            
             this->offset_of_record = buffer.offset_of_record;
            
            return *this;
        }
        bool operator==(pico_record&  buffer) {
            if(buffer.getKeyAsString().compare(this->getKeyAsString())==0)
                return true;
            
            return false;
            
        }
        void clear() {
            
            for (int i = 0; i < max_key_size; i++) {
                this->key_[i] = 0;
            }
            for (int i = 0; i < max_value_size; i++) {
                this->value_[i] = 0;
            }
        }
        char* getValue() {
            return value_;
        }
        char* getkey() {
            return key_;
        }
        std::string getKeyAsString() {
            std::string key(key_);
            return key;
        }
        std::string getValueAsString() {
            std::string val(value_);
            return val;
        }
        std::string toString() const {
            string str;
            std::string keyStr("{ key : ");
            std::string keyStr1(key_);
            std::string valueStr(" , value : ");
            std::string valueStr1(value_);
            std::string valueStr2(" }");
            str.append(keyStr);
            str.append(keyStr1);
            str.append(valueStr);
            str.append(valueStr1);
            str.append(valueStr2);
            
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
            if(currentRecord.key_[0]=='B' &&
               currentRecord.key_[1]=='E' &&
               currentRecord.key_[2]=='G' &&
               currentRecord.key_[3]=='K' &&
               currentRecord.key_[4]=='E' &&
               currentRecord.key_[5]=='Y')
                return true;
            
            return false;
        }
        
        static bool recordStartsWithConKEY(pico_record& currentRecord)//debugged
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
        
        static bool recordIsEmpty(pico_record& currentRecord)//debugged
        {
            if(currentRecord.key_[0]=='\0' &&
               currentRecord.key_[1]=='\0' &&
               currentRecord.key_[2]=='\0' &&
               currentRecord.key_[3]=='\0' &&
               currentRecord.key_[4]=='\0' &&
               currentRecord.key_[5]=='\0')
                return true;
            
            return false;
        }
        
        static void replicateTheFirstRecordKeyToOtherRecords(pico_record& firstRecord,pico_record&  currentRecord)
        {
            //except the first six key that should be CONKEY
            currentRecord.key_[0]='C';
            currentRecord.key_[1]='O';
            currentRecord.key_[2]='N';
            currentRecord.key_[3]='K';
            currentRecord.key_[4]='E';
            currentRecord.key_[5]='Y';
            
            for(int i=6;i<pico_record::max_key_size;i++)
            {
                currentRecord.key_[i]= firstRecord.key_[i];
            }
        }
        static void addKeyMarkerToFirstRecord(pico_record& firstRecord) //this argument has to be passed by ref
        {
            string keyMarker("BEGKEY"); //its the key that marks the key of the first record
            const char* keyArray = keyMarker.c_str();
            int i=0;
            while (*keyArray != 0) {
                firstRecord.key_[i]=*keyArray;
                ++i;
                ++keyArray;
            }//the key marker is put to first 6 letters of the first record
        }
        
        const static int max_start_of_record = 16;
        const static int max_end_of_record = 16;
        const static int max_value_size = 128;
        const static int max_key_size = 32;
        const static int max_size = max_key_size + max_value_size;
        char startOfRecord[max_start_of_record];
        long previous_record_offset; //if its -1, it means that there is no parent, if its more than -1, it indicates that it is add on to previous record
        char key_[max_key_size];
        char value_[max_value_size];
        char end_of_record[max_end_of_record];
    private:
        
    };
}
#endif /* PICO_RECORD_H_ */

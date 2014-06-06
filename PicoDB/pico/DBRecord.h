//
//  DBRecord.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 6/4/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

/*each record in database should have
 1. ${max_key_size} chars that make up the key , default for max_key_size is 32 for now
 
 2. ${offset_size} chars that is the starting offset of the value part data in ALL_VALUE file.
 defaulf for offset_size is 24 for now
 
 3. ${max_value_part_size} which is the size of the value after the starting offset
 so the end of value data is  No1 + No2
 default value for max_value_part_size is 24
 */


#ifndef PicoDB_DBRecord_h
#define PicoDB_DBRecord_h
#include <pico/pico_logger_wrapper.h>

namespace pico  {
    class DBRecord : pico_logger_wrapper
    {
        
        private :
        
        public :
        
        
        const static int begOfKeyIndex = 0;
        const static int maxKeySize = 32;
        const static int endOfKeyIndex = maxKeySize + begOfKeyIndex;
        
        const static int begOfStartingOffsetOfValueIndex = endOfKeyIndex;
        const static int maxSizeOfStartingOffsetOfValue = 24;
        const static int endOfStartingOffsetOfValueIndex = maxSizeOfStartingOffsetOfValue + begOfStartingOffsetOfValueIndex;
        
        const static int begOfEndingOffsetOfValueIndex = endOfStartingOffsetOfValueIndex;
        const static int maxSizeOfEndingOffsetOfValue = 24;
        const static int endOfEndingOffsetOfValueIndex = begOfEndingOffsetOfValueIndex + maxSizeOfEndingOffsetOfValue;
        
        const static int max_size = endOfEndingOffsetOfValueIndex+1;
        
        
        char data_[max_size];
        char data_copy[max_size];
        
        offsetType offset_of_record = -1; //this is the offset in the file and the index
        
        DBRecord() {
            for (int i = 0; i < max_size; i++) {            //initializng
                data_[i] = '\0';
            }
            
            offset_of_record = -1;
        }
        
        DBRecord(const DBRecord& buffer) {
            copyOver(buffer);
        }
        static bool isRecordEmpty(DBRecord& record) {
            
            for (int i = begOfKeyIndex; i < endOfEndingOffsetOfValueIndex; i++) {
                if (record.data_[i] != '\0') {
                    return false;
                }
                
            }
            
            return true;
        }
        void copyOver(const DBRecord& buffer) {
            
            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
                      std::begin(this->data_));
            
            this->offset_of_record = buffer.offset_of_record;
        }
        bool operator==(const DBRecord& buffer) {
            if(areRecordsEqual(buffer)) return true;
            return false;
        }
        
        DBRecord operator=(const DBRecord& buffer) {
            
            copyOver(buffer);
            
            return *this;
        }
        DBRecord operator=(DBRecord&& buffer) { //move assignment
            mylogger << "DBRecord move assigned\n";
            
            copyOver(buffer);
            
            return *this;
        }
        bool areRecordsEqual(const DBRecord& buffer) {
            
            if (areValuesEqual(buffer) && areKeysEqual(buffer))
                return true;
            
            return false;
        }
        bool areValuesEqual(const DBRecord& buffer) {
            for (int i = beg_of_value_index; i < end_of_value_index; i++) {
                if (data_[i] != buffer.data_[i]) {
                    return false;
                }
            }
            
            return true;
            
        }
        bool areKeysEqual(const DBRecord& buffer) {
            for (int i = beg_of_key_index; i < end_of_key_index; i++) {
                if (data_[i] != buffer.data_[i]) {
                    return false;
                }
            }
            
            return true;
            
        }
        void clear() {
            
            for (int i = 0; i < max_size; i++) {
                this->data_[i] = '\0';
            }
            
        }
        string keyType; //the string form of keyType
        string getKeyTypeAsString() {
            if (keyType.empty()) {
                getTheKeyTypeStringFromData();
            }
            assert(!keyType.empty());
            return keyType;
        }
        void getTheKeyTypeStringFromData()
        {
            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
            //by copying into the other string or assigning to other
            for (int i = beg_key_type_index; i < end_key_type_index; i++) {
                if (data_copy[i] != '\0') {
                    
                    keyType.push_back(data_copy[i]);
                } else {
                    break;
                }
            }
        }
        string key; //the string form of key
        string getKeyAsString() {
            if (key.empty()) {
                getTheKeyStringFromData();
            }
            assert(!key.empty());
            
            return key;
        }
        
        static string unpadTheKey(char* keyWithSeperator) {
            string key;
            int i = 0;
            while (keyWithSeperator != 0) {
                
                if (keyWithSeperator[i] != '\0') {
                    
                    key.push_back(keyWithSeperator[i]);
                    
                } else {
                    break;
                }
                ++i;
                ++keyWithSeperator;
            }
            
            return key;
            
        }
        string value;           //the string form of value
        
        string getValueAsString() {
            
            if (value.empty()) {
                getTheValueStringFromData();
            }
            assert(!value.empty());
            return value;
        }
        
        std::string getDataAsString() //to write to other side
        {
            
            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
            //by copying into the other string or assigning to other
            std::string data(data_copy, max_size);
            assert(!data.empty());
            
            return data;
        }
        std::string toJson() {
            
            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
            //by copying into the other string or assigning to other
            std::string data(data_copy, max_size);
            
            string str;
            std::string keyStr("{ ");
            std::string dataStr("data : ");
            std::string dataValue(data_copy, max_size);
            
            std::string end(" }");
            
            str.append(keyStr);
            str.append(dataStr);
            str.append(dataValue);
            str.append(end);
            return str;
        }
        std::string toString() {
            
            memcpy(data_copy, data_, sizeof(data_));
            
            //get a fresh copy of data to make sure its not touched
            //by copying into the other string or assigning to other
            
            std::string str(data_copy, max_size);
            assert(!str.empty());
            
            return str;
        }
        
        static bool recordStartsWithBEGKEY(DBRecord& currentRecord) //was debugged
        {
            if (currentRecord.data_[beg_key_type_index] == 'B'
                && currentRecord.data_[beg_key_type_index + 1] == 'E'
                && currentRecord.data_[beg_key_type_index + 2] == 'G'
                && currentRecord.data_[beg_key_type_index + 3] == 'K'
                && currentRecord.data_[beg_key_type_index + 4] == 'E'
                && currentRecord.data_[beg_key_type_index + 5] == 'Y')
                return true;
            
            return false;
        }
        
        static bool recordStartsWithConKEY(DBRecord& currentRecord) //debugged
        {
            if (currentRecord.data_[beg_key_type_index] == 'C'
                && currentRecord.data_[beg_key_type_index + 1] == 'O'
                && currentRecord.data_[beg_key_type_index + 2] == 'N'
                && currentRecord.data_[beg_key_type_index + 3] == 'K'
                && currentRecord.data_[beg_key_type_index + 4] == 'E'
                && currentRecord.data_[beg_key_type_index + 5] == 'Y')
                return true;
            
            return false;
        }
        static void addAppendMarkerToTheEnd(DBRecord& currentBuffer)
        
        {
            currentBuffer.data_[beg_of_appendMarker_index] = '9';
            currentBuffer.data_[beg_of_appendMarker_index + 1] = '9';
            currentBuffer.data_[beg_of_appendMarker_index + 2] = '9';
            currentBuffer.data_[beg_of_appendMarker_index + 3] = '9';
            currentBuffer.data_[beg_of_appendMarker_index + 4] = '9';
            currentBuffer.data_[beg_of_appendMarker_index + 5] = '9';
            
        }
        static bool IsThisRecordAnAddOn(DBRecord& currentBuffer) {
            
            if (currentBuffer.data_[beg_of_appendMarker_index] != '9'
                || currentBuffer.data_[beg_of_appendMarker_index + 1] != '9'
                || currentBuffer.data_[beg_of_appendMarker_index + 2] != '9'
                || currentBuffer.data_[beg_of_appendMarker_index + 3] != '9'
                || currentBuffer.data_[beg_of_appendMarker_index + 4] != '9'
                || currentBuffer.data_[beg_of_appendMarker_index + 5] != '9')
                return false;
            
            return true;
            
        }
        void setTheKeyString(string key) {
            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
            //by copying into the other string or assigning to other
            
            for (int i = beg_of_key_index; i < end_of_key_index; i++) {
                if (data_copy[i] != '\0') {
                    
                    key.push_back(data_copy[i]);
                    
                } else {
                    break;
                }
            }
            
        }
        
        void getTheKeyStringFromData() {
            for (int i = DBRecord::beg_of_key_index;
                 i < DBRecord::end_of_key_index; i++) {
                key.push_back(data_[i]);
            }
        }
        
        static void setTheRecordTypeInData(DBRecord& currentBuffer,
                                           string type) {
            
            const char* temp_buffer_message = type.c_str();
            for (int i = DBRecord::beg_key_type_index;
                 i < DBRecord::beg_key_type_index; i++) {
                currentBuffer.data_[i] = *temp_buffer_message;
                ++temp_buffer_message;
            }
            
        }
        
        static void setTheKeyInData(DBRecord& currentBuffer, string key) {
            int lastCharIndex = 0;
            const char* temp_buffer_message = key.c_str();
            for (int i = DBRecord::beg_of_key_index;
                 i < DBRecord::end_of_key_index; i++) {
                //putting everything in the value part of data_ in DBRecord , so retrieving it is easier
                
                if (*temp_buffer_message != 0) {
                    currentBuffer.data_[i] = *temp_buffer_message;
                } else {
                    break;
                }
                ++temp_buffer_message;
                lastCharIndex++;
            }
            
            for (int i = DBRecord::beg_of_key_index + lastCharIndex;
                 i < DBRecord::end_of_key_index; i++) {
                currentBuffer.data_[i] = '\0';
            }
            currentBuffer.setTheKeyString(key);
            
        }
        
        
        
        void getTheValueStringFromData()
        {
            for (int i = beg_of_value_index; i < end_of_value_index; i++) {
                value.push_back(data_[i]);
            }
            
        }
        
    };
    
}

#endif

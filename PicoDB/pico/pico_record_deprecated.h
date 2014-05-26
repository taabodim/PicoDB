////
////  pico_record_deprecated.h
////  PicoDB
////
////  Created by Mahmoud Taabodi on 5/24/14.
////  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
////
//
//#ifndef PicoDB_pico_record_deprecated_h
//#define PicoDB_pico_record_deprecated_h
//
//
//#include <memory>
//#include <logger.h>
//#include <pico_logger_wrapper.h>
//#include <stdio.h>
//#include <string.h>
//using namespace std;
//namespace pico {
//    class pico_record: public pico_logger_wrapper {
//        //use n=memcmp ( buffer1, buffer2, sizeof(buffer1) ); use this to compare if records are equal
//    public:
//        static string BEGKEY;
//        static string CONKEY;
//        const static int start_record_index = 0;
//        const static int beg_key_type_index = start_record_index;
//        const static int max_key_type_size = 6; //it could be BEGKEY or CONKEY
//        const static int end_key_type_index = max_key_type_size
//        + beg_key_type_index;
//        
//        const static int beg_of_key_index = end_key_type_index;
//        const static int max_key_size = 32;
//        const static int end_of_key_index = max_key_size + beg_of_key_index;
//        
//        const static int beg_of_value_index = end_of_key_index;
//        const static int max_value_size = 432 + 512; //to ke the whole record 512 bytes ,
//        const static int end_of_value_index = max_value_size + beg_of_value_index;
//        
//        const static int beg_of_messageId_index = end_of_value_index;
//        const static int messageId_size = 32;
//        const static int end_of_messageId_index = beg_of_messageId_index
//        + messageId_size;
//        
//        const static int beg_of_sequenceInMessage_index = end_of_messageId_index;
//        const static int sequenceInMessage = 4;
//        //this is the place of record in the array that makes the message
//        const static int end_of_sequenceInMessage_index =
//        beg_of_sequenceInMessage_index + sequenceInMessage;
//        
//        const static int beg_of_appendMarker_index = end_of_sequenceInMessage_index;
//        const static int appendMarker = 6; //it could be append or anything else
//        const static int end_of_appendMarker_index = beg_of_appendMarker_index
//        + appendMarker;
//        
//        
//        const static int  end_record_index = end_of_appendMarker_index;
//        const static int max_size = end_of_appendMarker_index+1;
//        
//        long previous_record_offset = -1; //if its -1, it means that there is no parent, if its more than -1, it indicates that it is add on to previous record
//        
//        char data_[max_size];
//        char data_copy[max_size];
//        
//        offsetType offset_of_record = -1; //this is the offset in the file and the index
//        
//        pico_record() {
//            for (int i = 0; i < max_size; i++) {		//initializng
//                data_[i] = '\0';
//            }
//            
//            offset_of_record = -1;
//        }
//        
//        pico_record(const pico_record& buffer) {
//            copyOver(buffer);
//        }
//        static bool isRecordEmpty(pico_record& record) {
//            for (int i = start_record_index; i < end_record_index; i++) {
//                if (record.data_[i] != '\0') {
//                    return false;
//                }
//                
//            }
//            
//            return true;
//        }
//        static bool ifTheRecordIsSendMeTheRestOfData(pico_record& b) {
//            if (b.data_[beg_of_value_index] == 's'
//				&& b.data_[beg_of_value_index + 1] == 'e'
//				&& b.data_[beg_of_value_index + 2] == 'n'
//				&& b.data_[beg_of_value_index + 3] == 'd'
//				&& b.data_[beg_of_value_index + 4] == 'm'
//				&& b.data_[beg_of_value_index + 5] == 'e'
//				&& b.data_[beg_of_value_index + 6] == 't'
//				&& b.data_[beg_of_value_index + 7] == 'h')
//                return true;
//            else
//                return false;
//            
//        }
//        void copyOver(const pico_record& buffer) {
//            
//            std::copy(std::begin(buffer.data_), std::end(buffer.data_),
//                      std::begin(this->data_));
//            
//            this->offset_of_record = buffer.offset_of_record;
//        }
//        bool operator==(const pico_record& buffer) {
//            
//            if(areRecordsEqual(buffer)) return true;
//            
//            return false;
//        }
//        
//        pico_record operator=(const pico_record& buffer) {
//            
//            copyOver(buffer);
//            
//            return *this;
//        }
//        pico_record operator=(pico_record&& buffer) { //move assignment
//            mylogger << "pico_record move assigned\n";
//            
//            copyOver(buffer);
//            
//            return *this;
//        }
//        bool areRecordsEqual(const pico_record& buffer) {
//            
//            if (areValuesEqual(buffer) && areKeysEqual(buffer))
//                return true;
//            
//            return false;
//        }
//        bool areValuesEqual(const pico_record& buffer) {
//            for (int i = beg_of_value_index; i < end_of_value_index; i++) {
//                if (data_[i] != buffer.data_[i]) {
//                    return false;
//                }
//            }
//            
//            return true;
//            
//        }
//        bool areKeysEqual(const pico_record& buffer) {
//            for (int i = beg_of_key_index; i < end_of_key_index; i++) {
//                if (data_[i] != buffer.data_[i]) {
//                    return false;
//                }
//            }
//            
//            return true;
//            
//        }
//        void clear() {
//            
//            for (int i = 0; i < max_size; i++) {
//                this->data_[i] = '\0';
//            }
//            
//        }
//        string keyType; //the string form of keyType
//        string getKeyTypeAsString() {
//            if (keyType.empty()) {
//                getTheKeyTypeStringFromData();
//            }
//            assert(!keyType.empty());
//            return keyType;
//        }
//        void getTheKeyTypeStringFromData()
//        {
//            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
//            //by copying into the other string or assigning to other
//            
//            for (int i = beg_key_type_index; i < end_key_type_index; i++) {
//                if (data_copy[i] != '\0') {
//                    
//                    keyType.push_back(data_copy[i]);
//                    
//                } else {
//                    break;
//                }
//            }
//        }
//        string key; //the string form of key
//        string getKeyAsString() {
//            if (key.empty()) {
//                getTheKeyStringFromData();
//            }
//            assert(!key.empty());
//            
//            return key;
//        }
//        
//        static string unpadTheKey(char* keyWithSeperator) {
//            string key;
//            int i = 0;
//            while (keyWithSeperator != 0) {
//                
//                if (keyWithSeperator[i] != '\0') {
//                    
//                    key.push_back(keyWithSeperator[i]);
//                    
//                } else {
//                    break;
//                }
//                ++i;
//                ++keyWithSeperator;
//            }
//            
//            return key;
//            
//        }
//        string value;		//the string form of value
//        
//        string getValueAsString() {
//            
//            if (value.empty()) {
//                getTheValueStringFromData();
//            }
//            assert(!value.empty());
//            return value;
//        }
//        string messageId;		//the string form of messageId
//        
//        string getMessageIdAsString() {
//            if (messageId.empty()) {
//                getTheMessageIdStringFromData();
//            }
//            assert(!messageId.empty());
//            
//            return messageId;
//        }
//        
//        std::string getDataAsString() //to write to other side
//        {
//            
//            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
//            //by copying into the other string or assigning to other
//            std::string data(data_copy, max_size);
//            assert(!data.empty());
//            
//            return data;
//        }
//        std::string toJson() {
//            
//            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
//            //by copying into the other string or assigning to other
//            std::string data(data_copy, max_size);
//            
//            string str;
//            std::string keyStr("{ ");
//            std::string dataStr("data : ");
//            std::string dataValue(data_copy, max_size);
//            
//            std::string end(" }");
//            
//            str.append(keyStr);
//            str.append(dataStr);
//            str.append(dataValue);
//            str.append(end);
//            return str;
//        }
//        std::string toString() {
//            
//            memcpy(data_copy, data_, sizeof(data_));
//            
//            //get a fresh copy of data to make sure its not touched
//            //by copying into the other string or assigning to other
//            
//            std::string str(data_copy, max_size);
//            assert(!str.empty());
//            
//            return str;
//        }
//        
//        static bool recordStartsWithBEGKEY(pico_record& currentRecord) //was debugged
//        {
//            if (currentRecord.data_[beg_key_type_index] == 'B'
//				&& currentRecord.data_[beg_key_type_index + 1] == 'E'
//				&& currentRecord.data_[beg_key_type_index + 2] == 'G'
//				&& currentRecord.data_[beg_key_type_index + 3] == 'K'
//				&& currentRecord.data_[beg_key_type_index + 4] == 'E'
//				&& currentRecord.data_[beg_key_type_index + 5] == 'Y')
//                return true;
//            
//            return false;
//        }
//        
//        static bool recordStartsWithConKEY(pico_record& currentRecord) //debugged
//        {
//            if (currentRecord.data_[beg_key_type_index] == 'C'
//				&& currentRecord.data_[beg_key_type_index + 1] == 'O'
//				&& currentRecord.data_[beg_key_type_index + 2] == 'N'
//				&& currentRecord.data_[beg_key_type_index + 3] == 'K'
//				&& currentRecord.data_[beg_key_type_index + 4] == 'E'
//				&& currentRecord.data_[beg_key_type_index + 5] == 'Y')
//                return true;
//            
//            return false;
//        }
//        static void addAppendMarkerToTheEnd(pico_record& currentBuffer)
//        
//        {
//            currentBuffer.data_[beg_of_appendMarker_index] = '9';
//            currentBuffer.data_[beg_of_appendMarker_index + 1] = '9';
//            currentBuffer.data_[beg_of_appendMarker_index + 2] = '9';
//            currentBuffer.data_[beg_of_appendMarker_index + 3] = '9';
//            currentBuffer.data_[beg_of_appendMarker_index + 4] = '9';
//            currentBuffer.data_[beg_of_appendMarker_index + 5] = '9';
//            
//        }
//        static bool IsThisRecordAnAddOn(pico_record& currentBuffer) {
//            
//            if (currentBuffer.data_[beg_of_appendMarker_index] != '9'
//				|| currentBuffer.data_[beg_of_appendMarker_index + 1] != '9'
//				|| currentBuffer.data_[beg_of_appendMarker_index + 2] != '9'
//				|| currentBuffer.data_[beg_of_appendMarker_index + 3] != '9'
//				|| currentBuffer.data_[beg_of_appendMarker_index + 4] != '9'
//				|| currentBuffer.data_[beg_of_appendMarker_index + 5] != '9')
//                return false;
//            
//            return true;
//            
//        }
//        void setTheKeyString(string key) {
//            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
//            //by copying into the other string or assigning to other
//            
//            for (int i = beg_of_key_index; i < end_of_key_index; i++) {
//                if (data_copy[i] != '\0') {
//                    
//                    key.push_back(data_copy[i]);
//                    
//                } else {
//                    break;
//                }
//            }
//            
//        }
//        
//        void getTheKeyStringFromData() {
//            for (int i = pico_record::beg_of_key_index;
//                 i < pico_record::end_of_key_index; i++) {
//                key.push_back(data_[i]);
//            }
//        }
//        
//        static void setTheRecordTypeInData(pico_record& currentBuffer,
//                                           string type) {
//            
//            const char* temp_buffer_message = type.c_str();
//            for (int i = pico_record::beg_key_type_index;
//                 i < pico_record::beg_key_type_index; i++) {
//                currentBuffer.data_[i] = *temp_buffer_message;
//                ++temp_buffer_message;
//            }
//            
//        }
//        
//        static void setTheKeyInData(pico_record& currentBuffer, string key) {
//            int lastCharIndex = 0;
//            const char* temp_buffer_message = key.c_str();
//            for (int i = pico_record::beg_of_key_index;
//                 i < pico_record::end_of_key_index; i++) {
//                //putting everything in the value part of data_ in pico_record , so retrieving it is easier
//                
//                if (*temp_buffer_message != 0) {
//                    currentBuffer.data_[i] = *temp_buffer_message;
//                } else {
//                    break;
//                }
//                ++temp_buffer_message;
//                lastCharIndex++;
//            }
//            
//            for (int i = pico_record::beg_of_key_index + lastCharIndex;
//                 i < pico_record::end_of_key_index; i++) {
//                currentBuffer.data_[i] = '\0';
//            }
//            currentBuffer.setTheKeyString(key);
//            
//        }
//        
//        static void setTheSequenceNumberInData(pico_record& currentBuffer, int sequenceNumber) {
//            int lastCharIndex = 0;
//            string sequenceNumberStr  = convertToString(sequenceNumber);
//            const char* temp_buffer_message = sequenceNumberStr.c_str();
//            for (int i = pico_record::beg_of_sequenceInMessage_index;
//                 i < pico_record::end_of_sequenceInMessage_index; i++) {
//                
//                
//                if (*temp_buffer_message != 0) {
//                    currentBuffer.data_[i] = *temp_buffer_message;
//                } else {
//                    break;
//                }
//                ++temp_buffer_message;
//                lastCharIndex++;
//            }
//            
//            for (int i = pico_record::beg_of_sequenceInMessage_index + lastCharIndex;
//                 i < pico_record::end_of_sequenceInMessage_index; i++) {
//                currentBuffer.data_[i] = '\0';
//            }
//            currentBuffer.setTheSequenceNumberString(sequenceNumberStr);
//            
//        }
//        
//        string sequenceNumber;
//        void setTheSequenceNumberString(string sequenceNumberStr)
//        {
//            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
//            //by copying into the other string or assigning to other
//            
//            for (int i = beg_of_sequenceInMessage_index; i < end_of_sequenceInMessage_index; i++) {
//                if (data_copy[i] != '\0') {
//                    sequenceNumber.push_back(data_copy[i]);
//                } else {
//                    break;
//                }
//            }
//        }
//        void setTheMessageIdString(string messageId) {
//            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
//            //by copying into the other string or assigning to other
//            
//            for (int i = beg_of_messageId_index; i < end_of_messageId_index; i++) {
//                if (data_copy[i] != '\0') {
//                    messageId.push_back(data_copy[i]);
//                } else {
//                    break;
//                }
//            }
//            
//        }
//        
//        void setTheValueString(string value) {
//            memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
//            //by copying into the other string or assigning to other
//            
//            for (int i = beg_of_value_index; i < end_of_value_index; i++) {
//                if (data_copy[i] != '\0') {
//                    value.push_back(data_copy[i]);
//                } else {
//                    break;
//                }
//            }
//            
//        }
//        void getTheValueStringFromData() {
//            for (int i = beg_of_value_index; i < end_of_value_index; i++) {
//                value.push_back(data_[i]);
//            }
//            
//        }
//        static void setTheValueInData(pico_record& currentBuffer, string values) {
//            int lastCharIndex = 0;
//            const char* temp_buffer_message = values.c_str();
//            for (int i = pico_record::beg_of_value_index;
//                 i < pico_record::end_of_value_index; i++) {
//                //putting everything in the value part of data_ in pico_record , so retrieving it is easier
//                
//                if (*temp_buffer_message != 0) {
//                    currentBuffer.data_[i] = *temp_buffer_message;
//                } else {
//                    break;
//                }
//                ++temp_buffer_message;
//                lastCharIndex++;
//                
//            }
//            for (int i = pico_record::beg_of_value_index + lastCharIndex;
//                 i < pico_record::end_of_value_index; i++) {
//                currentBuffer.data_[i] = '\0';
//            }
//            
//            currentBuffer.setTheValueString(values);
//        }
//        
//        void getTheMessageIdStringFromData() {
//            for (int i = pico_record::beg_of_messageId_index;
//                 i < pico_record::end_of_messageId_index; i++) {
//                messageId.push_back(data_[i]);
//            }
//            
//        }
//        static void setTheMessageIdInData(pico_record& currentBuffer,
//                                          string messageId) {
//            (*myloggerPtr)
//            << "\nsetTheMessageIdInData going to set this messageId : "
//            << messageId;
//            const char* messageIdInChars = messageId.c_str();
//            int lastCharIndex = 0;
//            for (int i = pico_record::beg_of_messageId_index;
//                 i < pico_record::end_of_messageId_index; i++) {
//                
//                if (*messageIdInChars != 0) {
//                    currentBuffer.data_[i] = *messageIdInChars;
//                } else {
//                    break;
//                }
//                ++messageIdInChars;
//                lastCharIndex++;
//            }
//            
//            for (int i = pico_record::beg_of_messageId_index + lastCharIndex;
//                 i < pico_record::end_of_messageId_index; i++) {
//                currentBuffer.data_[i] = '\0';
//            }
//            
//            currentBuffer.setTheMessageIdString(messageId);
//        }
//        static void removeTheAppendMarker(
//                                          std::shared_ptr<pico_record> currentBuffer) {
//            for (int i = beg_of_appendMarker_index; i < end_of_appendMarker_index;
//                 i++) {
//                currentBuffer->data_[i] = '\0';
//                
//            }
//            
//        }
//        
//        static void removeTheAppendMarker(
//                                          vector<pico_record>::iterator currentBuffer) {
//            for (int i = beg_of_appendMarker_index; i < end_of_appendMarker_index;
//                 i++) {
//                currentBuffer->data_[i] = '\0';
//                
//            }
//        }
//        static void removeTheAppendMarkerNoPtr(pico_record& currentBuffer) {
//            for (int i = beg_of_appendMarker_index; i < end_of_appendMarker_index;
//                 i++) {
//                currentBuffer.data_[i] = '\0';
//                
//            }
//        }
//        
//        static void removeTheKeyPart(pico_record& currentRecord) {
//            for (int i = beg_of_key_index; i < max_key_size; i++) {
//                currentRecord.data_[i] = '\0';
//            }
//        }
//        static void removeTheKeyMarkers1aadsadsd(pico_record& currentRecord) //debugged
//        {
//            currentRecord.data_[beg_of_appendMarker_index] = '\0';
//            currentRecord.data_[beg_of_appendMarker_index + 1] = '\0';
//            currentRecord.data_[beg_of_appendMarker_index + 2] = '\0';
//            currentRecord.data_[beg_of_appendMarker_index + 3] = '\0';
//            currentRecord.data_[beg_of_appendMarker_index + 4] = '\0';
//            currentRecord.data_[beg_of_appendMarker_index + 5] = '\0';
//        }
//        static void replicateTheFirstRecordKeyToOtherRecords(
//                                                             pico_record& firstRecord, pico_record& currentRecord) {
//            //except the first six key that should be CONKEY
//            currentRecord.data_[beg_key_type_index] = 'C';
//            currentRecord.data_[beg_key_type_index + 1] = 'O';
//            currentRecord.data_[beg_key_type_index + 2] = 'N';
//            currentRecord.data_[beg_key_type_index + 3] = 'K';
//            currentRecord.data_[beg_key_type_index + 4] = 'E';
//            currentRecord.data_[beg_key_type_index + 5] = 'Y';
//            
//            for (int i = beg_of_key_index; i < end_of_key_index; i++) {
//                currentRecord.data_[i] = firstRecord.data_[i];
//            }
//        }
//        
//        static void addKeyMarkerToRecord(pico_record& firstRecord, string keyMarker) //this argument has to be passed by ref
//        {
//            //its the key that marks the key of the first record
//            const char* keyArray = keyMarker.c_str();
//            int i = beg_key_type_index;
//            while (*keyArray != 0) {
//                firstRecord.data_[i] = *keyArray;
//                ++i;
//                ++keyArray;
//            } //the key marker is put to first 6 letters of the first record
//        }
//        
//        static void addConMarkerToFirstRecord(pico_record& continuingRecord) //this argument has to be passed by ref
//        {
//            
//            string keyMarker("CONKEY"); //its the key that marks the key of the continuing records
//            const char* keyArray = keyMarker.c_str();
//            int i = 0;
//            while (*keyArray != 0) {
//                continuingRecord.data_[i] = *keyArray;
//                ++i;
//                ++keyArray;
//            } //the key marker is put to first 6 letters of the first record
//            
//        }
//        
//        char* getDataForWrite() { //data is loaded to write
//            return data_;
//        }
//        char* getDataForRead(int dataSizeToRead) { //data is going to be loaded
//            return data_;
//        }
//        
//        size_t getSize() { //it is used !
//            return max_size;
//        }
//        typedef std::shared_ptr<pico_record> recordTypePtr;
//        
//        
//        static pico_record emptyInstance() //for template class
//        {
//            pico_record* emptyRecordPtr (new pico_record());//fix the leak later
//            return *emptyRecordPtr;
//            //resource leak
//        }
//        
//    private:
//        
//    }
//    ;
//}
//
//#endif

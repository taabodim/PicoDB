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

public:
	const static int appendMarkerSize = 6; //this is the size of the last few characters that determine if a record is the end of message
	//or message is going to be continued


	const static int beg_of_key_index = 0;
	const static int max_key_size = 32;
	const static int end_of_key_index = max_key_size;


	const static int beg_of_value_index = max_key_size;
	const static int max_value_size = 256; //should be more than 168 as the smallest message is that
	const static int end_of_value_index = max_value_size+beg_of_value_index;

    
	const static int beg_of_messageId_index = end_of_value_index;
	const static int messageId_size = 32;
	const static int end_of_messageId_index = beg_of_messageId_index+messageId_size;

    
    const static int beg_of_sequenceInMessage_index = end_of_messageId_index;
	const static int sequenceInMessage = 4;
	const static int end_of_sequenceInMessage_index = beg_of_sequenceInMessage_index+sequenceInMessage;

    
    const static int beg_of_appendMarker_index = end_of_sequenceInMessage_index;
	const static int appendMarker = 10;
	const static int end_of_appendMarker_index = beg_of_appendMarker_index+appendMarker;
    
    
    
	const static int max_size = end_of_appendMarker_index;






	long previous_record_offset; //if its -1, it means that there is no parent, if its more than -1, it indicates that it is add on to previous record

	char messageId_[messageId_size];
	char data_[max_size];
	char key_[max_size]; //these are the same size as data because we want to
	//do a safe strcpy(array2, array1);
	char value_[max_size];        //just for function below

	
	offsetType offset_of_record; //this is the offset in the file and the index
	offsetType previousRecordOffset;

	// Construct from a std::string.
//        void setValue(const std::string  realData) {
//            char value[max_value_size];
//            string data = realData.substr();
//            std::size_t length = data.copy(value,data.size(),0);
//            // data[length]='\0';
//            
//            //		strncpy(value_, data.c_str(), data.size());
//            value[data.size()] = '\0';
//            
//            for (size_t i = 0; i < max_value_size; i++) {
//                data_[i+max_key_size] = value[i];
//            }
//            for (size_t i = 0; i < max_value_size; i++) {
//                value_[i] = value[i];
//            }
//            
//            
//        }
////        
//        void setKeyValue(std::string  key, std::string  value) {
//            setKey(key);
//            setValue(value);
//        }
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
	void setKey(const std::string realData) {
		char key[max_key_size];

		string data = realData.substr();
		data.copy(key, data.size(), 0);
		//  data[length]='\0';
		//strncpy(key_, data.c_str(), data.size());
		key[data.size()] = '\0';

		for (size_t i = 0; i < max_key_size; i++) {
			data_[i] = key[i];
		}
		for (size_t i = 0; i < max_key_size; i++) {
			key_[i] = key[i];
		}
	}
//
//        explicit pico_record(std::string key, std::string value) {
//            setKey(key);
//            setValue(value);
//            
//        }

	pico_record() {

		for (int i = 0; i < max_size; i++) {            //initializng
			key_[i] = '\0';
		}
		for (int i = 0; i < max_size; i++) {            //initializng
			value_[i] = '\0';
		}
		for (int i = 0; i < max_size; i++) {		//initializng
			data_[i] = '\0';
		}
		for (int i = 0; i < messageId_size; i++) { //initializng
			messageId_[i] = '\0';
		}

	}

	pico_record(const pico_record& buffer) {
		copyOver(buffer);
	}

	static bool startWithSendMeTheRestOfData(pico_record& b) {
		if (b.data_[0] == 's' && b.data_[1] == 'e' && b.data_[2] == 'n'
				&& b.data_[3] == 'd' && b.data_[4] == 'm' && b.data_[5] == 'e'
				&& b.data_[6] == 't' && b.data_[7] == 'h')
			return true;
		else
			return false;

	}
	void copyOver(const pico_record& buffer) {

		std::copy(std::begin(buffer.data_), std::end(buffer.data_),
				std::begin(this->data_));

		std::copy(std::begin(buffer.key_), std::end(buffer.key_),
				std::begin(this->key_));
		std::copy(std::begin(buffer.value_), std::end(buffer.value_),
				std::begin(this->value_));

		std::copy(std::begin(buffer.messageId_), std::end(buffer.messageId_),
				std::begin(this->messageId_));

		this->offset_of_record = buffer.offset_of_record;
	}

	pico_record operator=(const pico_record& buffer) {

		copyOver(buffer);

		return *this;
	}
	pico_record operator=(pico_record&& buffer) { //move assignment
		mylogger << "pico_record move assigned\n";

		copyOver(buffer);

		return *this;
	}
	bool areRecordsEqual(pico_record& buffer) {

		if (areValuesEqual(buffer) && areKeysEqual(buffer))
			return true;

		return false;
	}
	bool areValuesEqual(pico_record& buffer) {
		for (int i = max_key_size; i < max_size; i++) {
			if (data_[i] != buffer.data_[i]) {
				return false;
			}
		}

		return true;

	}
	bool areKeysEqual(pico_record& buffer) {
		for (int i = 0; i < max_key_size; i++) {
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
		for (int i = 0; i < max_size; i++) {
			this->key_[i] = '\0';
		}
		for (int i = 0; i < max_size; i++) {
			this->value_[i] = '\0';
		}
		for (int i = 0; i < messageId_size; i++) {
			this->messageId_[i] = '\0';
		}
	}

	char* getValue() {
		for (int i = max_key_size; i < max_size; i++) {
			this->value_[i - max_key_size] = this->data_[i];
		}
		return value_;
	}

	char* getkey() {
		for (int i = 0; i < max_key_size; i++) {
			this->key_[i] = this->data_[i];
		}

		return key_;
	}

	string key;
	string getKeyAsString() {
		memcpy(key_, data_, sizeof(data_));

		for (int i = 0; i < max_key_size; i++) {
			if (key_[i] != '\0') {
				key.push_back(key_[i]);
			} else {
				break;
			}
		}
		return key;
	}
	string value;
	string getValueAsString() {

		memcpy(value_, data_, sizeof(data_));

		for (int i = 0; i < max_key_size; i++) {
			this->value_[i] = '\0';
		}

		for (int i = max_key_size; i < max_size; i++) {
			if (value_[i] != '\0') {
				value.push_back(value_[i]);
			} else {
				break;
			}

		}
		return value;
	}
	string messageId;
	string getMessageIdAsString() {

		for (int i = max_size - messageId_size; i < max_size; i++) {
			if (data_[i] != '\0') {
				messageId.push_back(data_[i]);
			} else {
				break;
			}
		}
		mylogger << " \n pico_record : getMessageIdAsString  messageId is "
				<< messageId;
		return messageId;
	}
	std::string getDataAsString() //to write to other side
	{
		std::string data(data_, max_size);
		return data;
	}
	std::string toJson() {
		string str;
		std::string keyStr("{ ");
		std::string data("data : ");
		std::string dataValue(data_, max_size);

		std::string end(" }");

		str.append(keyStr);
		str.append(data);
		str.append(dataValue);
		str.append(end);
		return str;
	}
	std::string toString() const {
		std::string str(data_, max_size);
		return str;
	}

	static bool recordStartsWithBEGKEY(pico_record& currentRecord) //was debugged
			{
		if (currentRecord.data_[0] == 'B' && currentRecord.data_[1] == 'E'
				&& currentRecord.data_[2] == 'G'
				&& currentRecord.data_[3] == 'K'
				&& currentRecord.data_[4] == 'E'
				&& currentRecord.data_[5] == 'Y')
			return true;

		return false;
	}

	static bool recordStartsWithConKEY(pico_record& currentRecord) //debugged
			{
		if (currentRecord.data_[0] == 'C' && currentRecord.data_[1] == 'O'
				&& currentRecord.data_[2] == 'N'
				&& currentRecord.data_[3] == 'K'
				&& currentRecord.data_[4] == 'E'
				&& currentRecord.data_[5] == 'Y')
			return true;

		return false;
	}
	static void addAppendMarkerToTheEnd(pico_record& currentBuffer)

	{
		int pos = pico_record::max_size - 1;
		currentBuffer.data_[pos] = '9';
		currentBuffer.data_[--pos] = '9';
		currentBuffer.data_[--pos] = '9';
		currentBuffer.data_[--pos] = '9';
		currentBuffer.data_[--pos] = '9';
		currentBuffer.data_[--pos] = '9';

	}
	static bool find_last_of_string(
			std::shared_ptr<pico_record> currentBuffer) {

		int pos = pico_record::max_size - 1;
		if (currentBuffer->data_[pos] != '9'
				|| currentBuffer->data_[--pos] != '9'
				|| currentBuffer->data_[--pos] != '9'
				|| currentBuffer->data_[--pos] != '9'
				|| currentBuffer->data_[--pos] != '9'
				|| currentBuffer->data_[--pos] != '9')
			return false;

		return true;

	}

	static void removeTheAppendMarker(
			std::shared_ptr<pico_record> currentBuffer) {
		int pos = pico_record::max_size - 1;
		currentBuffer->data_[pos] = '\0';
		currentBuffer->data_[--pos] = '\0';
		currentBuffer->data_[--pos] = '\0';
		currentBuffer->data_[--pos] = '\0';
		currentBuffer->data_[--pos] = '\0';
		currentBuffer->data_[--pos] = '\0';

	}
    void dissectTheBufferToItsComponents(pico_record& buffer)
    {//this message is supposed to get all the components, keys, values, messageId and etc  from the buffer, after calling this method, all the arrays that
        //hold the data are filled..and you can get them back as string
        
        
        
    
    }
	static void removeTheAppendMarker(
			list<pico_record>::iterator currentBuffer) {
		int pos = pico_record::max_size - 1;
		currentBuffer->data_[pos] = '\0';
		currentBuffer->data_[--pos] = '\0';
		currentBuffer->data_[--pos] = '\0';
		currentBuffer->data_[--pos] = '\0';
		currentBuffer->data_[--pos] = '\0';
		currentBuffer->data_[--pos] = '\0';

	}
	static void removeTheAppendMarkerNoPtr(pico_record& currentBuffer) {
		int pos = pico_record::max_size - 1;
		currentBuffer.data_[pos] = '\0';
		currentBuffer.data_[--pos] = '\0';
		currentBuffer.data_[--pos] = '\0';
		currentBuffer.data_[--pos] = '\0';
		currentBuffer.data_[--pos] = '\0';
		currentBuffer.data_[--pos] = '\0';

	}

	static bool recordIsEmpty(pico_record& currentRecord) //debugged
			{
		if (currentRecord.data_[0] == '\0' && currentRecord.data_[1] == '\0'
				&& currentRecord.data_[2] == '\0'
				&& currentRecord.data_[3] == '\0'
				&& currentRecord.data_[4] == '\0'
				&& currentRecord.data_[5] == '\0')
			return true;

		return false;
	}
	static void removeTheKeyPart(pico_record& currentRecord) {
		for (int i = 0; i < max_key_size; i++) {
			currentRecord.data_[i] = '\0';
		}
	}
	static void removeTheKeyMarkers1aadsadsd(pico_record& currentRecord) //debugged
			{
		currentRecord.data_[0] = '\0';
		currentRecord.data_[1] = '\0';
		currentRecord.data_[2] = '\0';
		currentRecord.data_[3] = '\0';
		currentRecord.data_[4] = '\0';
		currentRecord.data_[5] = '\0';
	}
	static void replicateTheFirstRecordKeyToOtherRecords(
			pico_record& firstRecord, pico_record& currentRecord) {
		//except the first six key that should be CONKEY
		currentRecord.data_[0] = 'C';
		currentRecord.data_[1] = 'O';
		currentRecord.data_[2] = 'N';
		currentRecord.data_[3] = 'K';
		currentRecord.data_[4] = 'E';
		currentRecord.data_[5] = 'Y';

		for (int i = 6; i < pico_record::max_key_size; i++) {
			currentRecord.data_[i] = firstRecord.data_[i];
		}
	}
	static void setTheMessageIdInData(pico_record& currentBuffer,
			string messageId) {
		const char* messageIdInChars = messageId.c_str();
		while (*messageIdInChars != 0) {

			for (int i = pico_record::messageId_size; i < pico_record::max_size;
					i++) {
				if (*messageIdInChars != 0) {
					currentBuffer.data_[i] = *messageIdInChars;
				}
			}
			++messageIdInChars;
		}

	}

	static void addKeyMarkerToFirstRecord(pico_record& firstRecord) //this argument has to be passed by ref
			{
		string keyMarker("BEGKEY"); //its the key that marks the key of the first record
		const char* keyArray = keyMarker.c_str();
		int i = 0;
		while (*keyArray != 0) {
			firstRecord.data_[i] = *keyArray;
			++i;
			++keyArray;
		} //the key marker is put to first 6 letters of the first record
	}
	char* getDataForWrite() { //data is loaded to write
		return data_;
	}
	char* getDataForRead() { //data is going to be loaded
		return data_;
	}

	size_t getSize() { //it is used !
		return max_size;
	}
	typedef std::shared_ptr<pico_record> recordTypePtr;
private:

};
}
#endif /* PICO_RECORD_H_ */

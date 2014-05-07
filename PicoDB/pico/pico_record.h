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

	const static int beg_key_type_index = 0;
	const static int max_key_type_index = 6; //it could be BEGKEY or CONKEY
	const static int end_key_type_index = max_key_type_index
			+ beg_key_type_index;

	const static int beg_of_key_index = end_key_type_index;
	const static int max_key_size = 32;
	const static int end_of_key_index = max_key_size + beg_of_key_index;

	const static int beg_of_value_index = end_of_key_index;
	const static int max_value_size = 256; //should be more than 168 as the smallest message is that
	const static int end_of_value_index = max_value_size + beg_of_value_index;

	const static int beg_of_messageId_index = end_of_value_index;
	const static int messageId_size = 32;
	const static int end_of_messageId_index = beg_of_messageId_index
			+ messageId_size;

	const static int beg_of_sequenceInMessage_index = end_of_messageId_index;
	const static int sequenceInMessage = 4;
	const static int end_of_sequenceInMessage_index =
			beg_of_sequenceInMessage_index + sequenceInMessage;

	const static int beg_of_appendMarker_index = end_of_sequenceInMessage_index;
	const static int appendMarker = 6; //it could be append or anything else
	const static int end_of_appendMarker_index = beg_of_appendMarker_index
			+ appendMarker;

	const static int max_size = end_of_appendMarker_index;

	long previous_record_offset; //if its -1, it means that there is no parent, if its more than -1, it indicates that it is add on to previous record

	char data_[max_size];
	char data_copy[max_size];

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
//	void setKey(const std::string realData) {
//		char key[max_key_size];
//
//		string data = realData.substr();
//		data.copy(key, data.size(), 0);
//		//  data[length]='\0';
//		//strncpy(key_, data.c_str(), data.size());
//		key[data.size()] = '\0';
//
//		for (size_t i = 0; i < max_key_size; i++) {
//			data_[i] = key[i];
//		}
//		for (size_t i = 0; i < max_key_size; i++) {
//			key_[i] = key[i];
//		}
//	}
//
//        explicit pico_record(std::string key, std::string value) {
//            setKey(key);
//            setValue(value);
//            
//        }

	pico_record() {
		for (int i = 0; i < max_size; i++) {		//initializng
			data_[i] = '\0';
		}

//		for (int i = 0; i < max_size; i++) {            //initializng
//			key_[i] = '\0';
//		}
//		for (int i = 0; i < max_size; i++) {            //initializng
//			value_[i] = '\0';
//		}
//		
//		for (int i = 0; i < messageId_size; i++) { //initializng
//			messageId_[i] = '\0';
//		}

	}

	pico_record(const pico_record& buffer) {
		copyOver(buffer);
	}

	static bool ifTheRecordIsSendMeTheRestOfData(pico_record& b) {
		if (b.data_[beg_of_value_index] == 's'
				&& b.data_[beg_of_value_index + 1] == 'e'
				&& b.data_[beg_of_value_index + 2] == 'n'
				&& b.data_[beg_of_value_index + 3] == 'd'
				&& b.data_[beg_of_value_index + 4] == 'm'
				&& b.data_[beg_of_value_index + 5] == 'e'
				&& b.data_[beg_of_value_index + 6] == 't'
				&& b.data_[beg_of_value_index + 7] == 'h')
			return true;
		else
			return false;

	}
	void copyOver(const pico_record& buffer) {

		std::copy(std::begin(buffer.data_), std::end(buffer.data_),
				std::begin(this->data_));

//		std::copy(std::begin(buffer.key_), std::end(buffer.key_),
//				std::begin(this->key_));
//		std::copy(std::begin(buffer.value_), std::end(buffer.value_),
//				std::begin(this->value_));
//
//		std::copy(std::begin(buffer.messageId_), std::end(buffer.messageId_),
//				std::begin(this->messageId_));

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
		for (int i = beg_of_value_index; i < end_of_value_index; i++) {
			if (data_[i] != buffer.data_[i]) {
				return false;
			}
		}

		return true;

	}
	bool areKeysEqual(pico_record& buffer) {
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
//		for (int i = 0; i < max_size; i++) {
//			this->key_[i] = '\0';
//		}
//		for (int i = 0; i < max_size; i++) {
//			this->value_[i] = '\0';
//		}
//		for (int i = 0; i < messageId_size; i++) {
//			this->messageId_[i] = '\0';
//		}
	}

//	char* getValue() {
//		for (int i = beg_of_value_index; i < end_of_value_index; i++) {
//			this->value_[i - max_key_size] = this->data_[i];
//		}
//		return value_;
//	}
//
//	char* getkey() {
//		for (int i = beg_of_key_index; i < end_of_key_index; i++) {
//			this->key_[i] = this->data_[i];
//		}
//
//		return key_;
//	}

	string key; //the string form of key
	string getKeyAsString() {
		memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
		//by copying into the other string or assigning to other

		for (int i = beg_of_key_index; i < end_of_key_index; i++) {
			if (data_copy[i] != '\0') {
				key.push_back(data_copy[i]);
			} else {
				break;
			}
		}
		return key;
	}
	string value;		//the string form of value
	string getValueAsString() {

		memcpy(data_copy, data_, sizeof(data_));//get a fresh copy of data to make sure its not touched
		//by copying into the other string or assigning to other

		for (int i = beg_of_value_index; i < end_of_value_index; i++) {
			if (data_copy[i] != '\0') {
				value.push_back(data_copy[i]);
			} else {
				break;
			}
		}
		return value;
	}
	string messageId;		//the string form of messageId
	string getMessageIdAsString() {
		memcpy(data_copy, data_, sizeof(data_));//get a fresh copy of data to make sure its not touched
		//by copying into the other string or assigning to other

		for (int i = beg_of_messageId_index; i < end_of_messageId_index; i++) {
			if (data_copy[i] != '\0') {
				messageId.push_back(data_copy[i]);
			} else {
				break;
			}
		}

		return messageId;
	}

	std::string getDataAsString() //to write to other side
	{
		memcpy(data_copy, data_, sizeof(data_)); //get a fresh copy of data to make sure its not touched
		//by copying into the other string or assigning to other
		std::string data(data_copy, max_size);

		return data;
	}
	std::string toJson() {

		memcpy(data_copy, data_, sizeof(data_));//get a fresh copy of data to make sure its not touched
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

		return str;
	}

	static bool recordStartsWithBEGKEY(pico_record& currentRecord) //was debugged
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

	static bool recordStartsWithConKEY(pico_record& currentRecord) //debugged
			{
		if (currentRecord.data_[beg_key_type_index] == 'C'
				&& currentRecord.data_[beg_key_type_index+1] == 'O'
				&& currentRecord.data_[beg_key_type_index + 2] == 'N'
				&& currentRecord.data_[beg_key_type_index + 3] == 'K'
				&& currentRecord.data_[beg_key_type_index + 4] == 'E'
				&& currentRecord.data_[beg_key_type_index + 5] == 'Y')
			return true;

		return false;
	}
	static void addAppendMarkerToTheEnd(pico_record& currentBuffer)

	{
		currentBuffer.data_[beg_of_appendMarker_index] = '9';
		currentBuffer.data_[beg_of_appendMarker_index + 1] = '9';
		currentBuffer.data_[beg_of_appendMarker_index + 2] = '9';
		currentBuffer.data_[beg_of_appendMarker_index + 3] = '9';
		currentBuffer.data_[beg_of_appendMarker_index + 4] = '9';
		currentBuffer.data_[beg_of_appendMarker_index + 5] = '9';

	}
	static bool find_last_of_string(
			std::shared_ptr<pico_record> currentBuffer) {

		if (currentBuffer->data_[beg_of_appendMarker_index] != '9'
				|| currentBuffer->data_[beg_of_appendMarker_index + 1] != '9'
				|| currentBuffer->data_[beg_of_appendMarker_index + 2] != '9'
				|| currentBuffer->data_[beg_of_appendMarker_index + 3] != '9'
				|| currentBuffer->data_[beg_of_appendMarker_index + 4] != '9'
				|| currentBuffer->data_[beg_of_appendMarker_index + 5] != '9')
			return false;

		return true;

	}
	static void setTheValueInData(pico_record& currentBuffer, string values) {

		const char* temp_buffer_message = values.c_str();
		for (int i = pico_record::beg_of_value_index;
				i < pico_record::end_of_value_index; i++) {
			//putting everything in the value part of data_ in pico_record , so retrieving it is easier

			if (*temp_buffer_message != 0) {
				currentBuffer.data_[i] = *temp_buffer_message;
			} else {
				break;
			}
			++temp_buffer_message;
		}

	}

	static void setTheMessageIdInData(pico_record& currentBuffer,
			string messageId) {
		(*myloggerPtr)
				<< "\nsetTheMessageIdInData going to set this messageId : "
				<< messageId;
		const char* messageIdInChars = messageId.c_str();

		for (int i = pico_record::beg_of_messageId_index;
				i < pico_record::end_of_messageId_index; i++) {

			if (*messageIdInChars != 0) {
				currentBuffer.data_[i] = *messageIdInChars;
			} else {
				break;
			}
			++messageIdInChars;
		}

	}

	static void removeTheAppendMarker(
			std::shared_ptr<pico_record> currentBuffer) {
		for (int i = beg_of_appendMarker_index; i < end_of_appendMarker_index;
				i++) {
			currentBuffer->data_[i] = '\0';

		}

	}

	static void removeTheAppendMarker(
			list<pico_record>::iterator currentBuffer) {
		for (int i = beg_of_appendMarker_index; i < end_of_appendMarker_index;
				i++) {
			currentBuffer->data_[i] = '\0';

		}
	}
	static void removeTheAppendMarkerNoPtr(pico_record& currentBuffer) {
		for (int i = beg_of_appendMarker_index; i < end_of_appendMarker_index;
				i++) {
			currentBuffer.data_[i] = '\0';

		}
	}


	static void removeTheKeyPart(pico_record& currentRecord) {
		for (int i = beg_of_key_index; i < max_key_size; i++) {
			currentRecord.data_[i] = '\0';
		}
	}
	static void removeTheKeyMarkers1aadsadsd(pico_record& currentRecord) //debugged
			{
		currentRecord.data_[beg_of_appendMarker_index] = '\0';
		currentRecord.data_[beg_of_appendMarker_index+1] = '\0';
		currentRecord.data_[beg_of_appendMarker_index+2] = '\0';
		currentRecord.data_[beg_of_appendMarker_index+3] = '\0';
		currentRecord.data_[beg_of_appendMarker_index+4] = '\0';
		currentRecord.data_[beg_of_appendMarker_index+5] = '\0';
	}
	static void replicateTheFirstRecordKeyToOtherRecords(
			pico_record& firstRecord, pico_record& currentRecord) {
		//except the first six key that should be CONKEY
		currentRecord.data_[beg_key_type_index] = 'C';
		currentRecord.data_[beg_key_type_index + 1] = 'O';
		currentRecord.data_[beg_key_type_index + 2] = 'N';
		currentRecord.data_[beg_key_type_index + 3] = 'K';
		currentRecord.data_[beg_key_type_index + 4] = 'E';
		currentRecord.data_[beg_key_type_index + 5] = 'Y';

		for (int i = beg_of_key_index; i < end_of_key_index; i++) {
			currentRecord.data_[i] = firstRecord.data_[i];
		}
	}

	static void addKeyMarkerToFirstRecord(pico_record& firstRecord) //this argument has to be passed by ref
			{
		string keyMarker("BEGKEY"); //its the key that marks the key of the first record
		const char* keyArray = keyMarker.c_str();
		int i = beg_key_type_index;
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

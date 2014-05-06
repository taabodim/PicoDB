/*
 * pico_message.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_MESSAGE_H_
#define PICO_MESSAGE_H_
//change
#include "pico/pico_record.h"
#include <third_party/json_cpp/json/json.h>
#include <pico/pico_concurrent_list.h>
#include <logger.h>
#include <pico_logger_wrapper.h>
#include <pico/pico_utils.h>
#include <pico/pico_buffered_message.h>
#include <list>
namespace pico {
class pico_message: public pico_logger_wrapper {
private:

public:
	std::string key;
	std::string value;
	std::string oldvalue; //for update messages
	std::string user;
	std::string db;
	std::string command;
	std::string messageId; //each command that passes must have an Id
	//then when server sends the response, the server sends the same id back to
	//client then the client knows , this reply message is for which request

	std::string collection;
	std::string json_form_of_message;
	std::string json_key_value_pair;
	std::string hashCodeOfMessage;

	pico_buffered_message<pico_record> key_value_buffered_message;
	////key value pair that came with this message in record list to
	//be saved in db

	pico_buffered_message<pico_record> recorded_message; //a container for all the records that make up the

	long messageSize;

	pico_message() {
	}
	pico_message(const pico_message& msg) {
		mylogger << "\npico_message copy constructor being called.\n";
		this->user = msg.user;
		this->db = msg.db;
		this->command = msg.command;
		this->messageId = msg.messageId;
		this->collection = msg.collection;
		this->messageSize = msg.messageSize;
		this->key = msg.key;
		this->value = msg.value;
		this->oldvalue = msg.oldvalue;
		this->json_form_of_message = msg.json_form_of_message;
		this->json_key_value_pair = msg.json_key_value_pair;

		this->hashCodeOfMessage = msg.hashCodeOfMessage;
		this->recorded_message = msg.recorded_message;
		this->key_value_buffered_message = msg.key_value_buffered_message;

	}
	pico_message(const std::string message_from_client) {
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;

		bool parsingSuccessful = reader.parse(message_from_client, root);
		if (!parsingSuccessful) {
			// report to the user the failure and their locations in the document.
			//                mylogger << "Failed to parse message :"<<message_from_client<<"\n"
			//              << reader.getFormattedErrorMessages();
		}
		this->json_form_of_message = message_from_client;
		this->command = root.get("command", "unknown").asString();

		this->messageId = root.get("messageId", "couldntparseit").asString();
		this->collection = root.get("collection", "unknown").asString();
		this->db = root.get("db", "unknown").asString();
		this->user = root.get("user", "unknown").asString();
		this->key = root.get("key", "unknown").asString();
		this->value = root.get("value", "unknown").asString();
		this->oldvalue = root.get("oldvalue", "unknown").asString();

		this->json_key_value_pair = createTheKeyValuePair();
		this->set_hash_code();
		this->convert_to_buffered_message();
		this->convert_key_value_buffered_message();
	}

	pico_message(const std::string json_message_from_client,
			bool simpleMessage) {
		//this is for processing shell commands

		this->json_form_of_message = json_message_from_client;
		//this->json_key_value_pair = createTheKeyValuePair(); this line should always be commented because when the msg is simple
		//it will mess up the parser
		if (!json_message_from_client.empty()) {
			this->set_hash_code();
			this->convert_to_buffered_message();
			this->convert_key_value_buffered_message();
		}
	}
	std::string createTheKeyValuePair() {

		Json::Value root;   // will contains the root value after parsing.
		root["key"] = this->key;
		root["value"] = this->value;
		Json::StyledWriter writer;
		// Make a new JSON document for the configuration. Preserve original comments.
		std::string output = writer.write(root);
		return output;
	}
	static pico_message build_message_from_string(const string value,
			string messageId) {

		pico_message msg(value, true);
		msg.messageId = messageId;
		return msg;
	}

	//	static pico_message build_complete_message_from_string(string value,
	//			string messageId) {
	//		Json::Value root;   // will contains the root value after parsing.
	//		root["key"] = "simpleMessage";
	//		root["value"] = value;
	//		root["oldvalue"] = "unknown";
	//		root["db"] = "unknown";
	//		root["user"] = "unknown";
	//		root["collection"] = "unknown";
	//		root["command"] = "unknown";
	//
	//		if (messageId.comapre("unknown") == 0) {
	//			root["messageId"] = convertToString(calc_request_id());
	//		} else {
	//			root["messageId"] = messageId;
	//		}
	//		root["hashCode"] = "unknown";
	//
	//		Json::StyledWriter writer;
	//		// Make a new JSON document for the configuration. Preserve original comments.
	//		std::string output = writer.write(root);
	//		pico_message msg(output);
	//		return msg;
	//	}
	static pico_message build_complete_message_from_key_value_pair(string key,
			string value) {
		Json::Value root;   // will contains the root value after parsing.
		root["key"] = key;
		root["value"] = value;
		root["oldvalue"] = "unknown";
		root["db"] = "unknown";
		root["user"] = "unknown";
		root["collection"] = "unknown";
		root["command"] = "unknown";
		root["messageId"] = convertToString(calc_request_id());
		root["hashCode"] = "unknown";

		Json::StyledWriter writer;
		// Make a new JSON document for the configuration. Preserve original comments.
		std::string output = writer.write(root);
		pico_message msg(output);
		return msg;
	}

	pico_message operator=(pico_message& msg) {
		mylogger << ("pico_message copy operator being called.\n");
		this->user = msg.user;
		this->db = msg.db;
		this->command = msg.command;
		this->messageId = msg.messageId;
		this->collection = msg.collection;
		this->messageSize = msg.messageSize;
		this->key = msg.key;
		this->value = msg.value;
		this->oldvalue = msg.oldvalue;
		this->json_form_of_message = msg.json_form_of_message;
		this->json_key_value_pair = msg.json_key_value_pair;
		this->hashCodeOfMessage = msg.hashCodeOfMessage;
		this->recorded_message = msg.recorded_message;
		this->key_value_buffered_message = msg.key_value_buffered_message;
		return *this;
	}
	std::string convert_message_to_json() {

		Json::Value root;   // will contains the root value after parsing.
		root["key"] = key;
		root["value"] = value;
		root["oldvalue"] = oldvalue;
		root["db"] = db;
		root["user"] = user;
		root["collection"] = collection;
		root["command"] = command;
		root["messageId"] = convertToString(messageId);
		root["hashCode"] = hashCodeOfMessage;
		Json::StyledWriter writer;
		// Make a new JSON document for the configuration. Preserve original comments.
		std::string output = writer.write(root);

		return output;
	}
	pico_message(std::string newKey, std::string newValue, std::string com,
			std::string database, std::string us, std::string col) {

		command = com;
		collection = col;
		db = database;
		user = us;
		key = newKey;
		value = newValue;
		messageId = calc_request_id();
		json_form_of_message = convert_message_to_json();
		messageSize = json_form_of_message.size();
		json_key_value_pair = createTheKeyValuePair();
		set_hash_code();
		convert_to_buffered_message();
		convert_key_value_buffered_message();
	}
	pico_message(std::string newKey, std::string old_value_arg,
			std::string newValue, std::string com, std::string database,
			std::string us, std::string col) {
		command = com;
		collection = col;
		db = database;
		user = us;
		key = newKey;
		oldvalue = old_value_arg;
		value = newValue;
		messageId = calc_request_id();
		json_form_of_message = convert_message_to_json();
		messageSize = json_form_of_message.size();
		json_key_value_pair = createTheKeyValuePair();
		set_hash_code();
		convert_to_buffered_message();
		convert_key_value_buffered_message();
	}
	pico_message(std::string keyFromDB, std::string valueFromDB) {
		//this is used when we want to create a nice pico message
		//out of array of buffers
		command = "unknown";
		collection = "unknown";
		db = "unknown";
		user = "unknown";
		key = keyFromDB;
		value = valueFromDB;
		messageId = calc_request_id();
		json_form_of_message = convert_message_to_json();
		messageSize = json_form_of_message.size();
		json_key_value_pair = createTheKeyValuePair();
		set_hash_code();
		convert_to_buffered_message();
		convert_key_value_buffered_message();
	}

	void set_hash_code() {
		std::size_t h1 = std::hash<std::string>()(user);
		std::size_t h2 = std::hash<std::string>()(db);
		std::size_t h3 = std::hash<std::string>()(command);
		std::size_t h4 = std::hash<std::string>()(collection);
		std::size_t h5 = std::hash<std::string>()(key); //find a solution to hash long strings
		std::size_t h6 = std::hash<std::string>()(value); //find a solution to hash long
		std::size_t h7 = std::hash<std::string>()(oldvalue);

		size_t hash_code = (h1 ^ (h2 << 1) ^ h3 ^ h4 ^ h5 ^ h6 ^ h7);
		this->hashCodeOfMessage = boost::lexical_cast < string > (hash_code);

		// cout << "unique message id is " << this->hashCodeOfMessage << endl;
	}

	std::string toString() const {
		return json_form_of_message;
	}
	std::string toKeyValuePairString() const {
		return json_key_value_pair;
	}
	void addConMarkerToFirstRecord(pico_record& continuingRecord) //this argument has to be passed by ref
			{

		string keyMarker("CONKEY"); //its the key that marks the key of the continuing records
		const char* keyArray = keyMarker.c_str();
		int i = 0;
		while (*keyArray != 0) {
			continuingRecord.data_[i] = *keyArray;
			++i;
			++keyArray;
		} //the key marker is put to first 6 letters of the first record

	}
	void convert_key_value_buffered_message() {
		//this function is used to save the key value in db
		//and not the transfer over network
		mylogger << "pico_message : convert_to_list_of_records \n";

		pico_record firstRecord;

		pico_record::addKeyMarkerToFirstRecord(firstRecord);
		string copyOfKey = key.substr(0);

		const char* keyArray = copyOfKey.c_str();
		int i = 6;
		while (*keyArray != 0) {
			firstRecord.data_[i] = *keyArray;
			++i;
			++keyArray;
		}            //the key is put to first record
		for (int k = i; k < pico_record::max_key_size; k++) {
			firstRecord.data_[i] = 0;
		}            //put 0 after the key and before value

		string copyOfValue = value.substr(0);
		const char* valueArray = copyOfValue.c_str();

		bool dataEnded = false;
		for (int j = pico_record::max_key_size; j < pico_record::max_size - 6;
				j++) {
			if (*valueArray != 0) {
				firstRecord.data_[j] = *valueArray;
				++valueArray;
			} else {
				dataEnded = true;
				break;

			}
		}            //the value of the first record is populated with values
		if (!dataEnded) {            //there is more data to be appended
			pico_record::addAppendMarkerToTheEnd(firstRecord);

		}

		bool moreThanOneRecord = false;
		mylogger
				<< "\n pico_message : this is the record that is put in  : key_value_buffered_message buffer : "
				<< firstRecord.toString();
		key_value_buffered_message.append(firstRecord);
		while (*valueArray != 0) {
			pico_record currentRecord;

			pico_record::replicateTheFirstRecordKeyToOtherRecords(firstRecord,
					currentRecord);

			for (int i = 6; i < pico_record::max_size - 6; i++) {

				if (*valueArray != 0) {
					currentRecord.data_[i] = *valueArray;
				} else {
					dataEnded = true;
					break;
				} //adding the rest of values to the second and third and etc records
				++valueArray;
			}

			pico_record::addAppendMarkerToTheEnd(currentRecord);
			moreThanOneRecord = true;
			mylogger
					<< "\n pico_message : this is the record that is put in  : key_value_buffered_message currentRecord buffer : "
					<< currentRecord.toString();
			key_value_buffered_message.append(currentRecord);
		}

		if (moreThanOneRecord) {
			pico_record::removeTheAppendMarker(
					key_value_buffered_message.getLastBuffer());
		}

	}

	//this function converts all the message to a list of pico_records , and
	//puts the all the message in the value part of records only
	//and set the messageId and append marker in the related parts in the data_ array in the records
	//the whole message that passes between client and server
	void convert_to_buffered_message() {

		size_t sizeOfMessage = json_form_of_message.length();
		int numOfBuffersNeededForThisMessage = (sizeOfMessage
				/ pico_record::max_value_size) + 1;
		mylogger
				<< " number of records needed to contain the key value of this message is "
				<< numOfBuffersNeededForThisMessage << "\n";

		for (int i = 0, indexInJsonMessage = 0;
				i < numOfBuffersNeededForThisMessage; i++) {

			string valueForThisBuffer;
			if ((indexInJsonMessage + pico_record::max_value_size) < sizeOfMessage) {
				valueForThisBuffer.append(
						json_form_of_message.substr(indexInJsonMessage,
								indexInJsonMessage + pico_record::max_value_size));
			} else {
				valueForThisBuffer.append(
						json_form_of_message.substr(indexInJsonMessage));

			}
			indexInJsonMessage += pico_record::max_value_size;

			mylogger
					<< "\n convert_to_buffered_message : valueForThisBuffer is "
					<< valueForThisBuffer;

			pico_record currentBuffer;
			pico_record::setTheMessageIdInData(currentBuffer, messageId);
			pico_record::setTheValueInData(currentBuffer, valueForThisBuffer);
			pico_record::addAppendMarkerToTheEnd(currentBuffer); //add append
						recorded_message.append(currentBuffer);

		}
        

		pico_record::removeTheAppendMarker(recorded_message.getLastBuffer()); //removes the append marker from the last record

	
		key_value_buffered_message.print();
	}

	//        std::shared_ptr<pico_concurrent_list<type>> msg_in_buffers
	//this method is very important!! should be debugged and improved throughly
	pico_message convertBuffersToMessage(pico_buffered_message<pico_record> all_buffers) {
         
		string allMessage;
		string key;
		string value;
		bool typeOfmessageIsRecord = false;


		mylogger<<"\n this is the begning og the convertBuffersToMessage function \n";
		all_buffers.print();

		while (!all_buffers.msg_in_buffers->empty()) {
			//get rid of all buffers that are not for this messageId
			pico_record buf = all_buffers.msg_in_buffers->pop();
			//if buffer is begingin key

			mylogger << " \n buffer popped is \n " << buf.toString();

			string temp;
			if (pico_record::ifTheRecordIsSendMeTheRestOfData(buf)) {
				string msgType(buf.getValueAsString());
				allMessage.append(msgType);
				mylogger
						<< "\n pico_message : convertBuffersToMessage : allMessage is "
						<< allMessage << "\n";
				pico_message pico_msg(allMessage);
				return pico_msg;

			}
			if (pico_record::recordStartsWithBEGKEY(buf)) {

				string key = buf.getKeyAsString();
				mylogger << "\n key extracted from key record as string is "
						<< key;

				//string tempWithoutKeyPart =temp.substr(pico_record::max_key_size);
				string tempWithoutKeyPart = buf.getValueAsString();

				mylogger << "\n the value part of key record as string is "
						<< tempWithoutKeyPart;
				value.append(tempWithoutKeyPart);
				typeOfmessageIsRecord = true;
			}
			//if buffer is continuing key
			else if (pico_record::recordStartsWithConKEY(buf)) {
				//remove the marker

				string valueIncomplete = buf.getValueAsString();

				mylogger
						<< "\n the incomplete value part of contining record as string is "
						<< valueIncomplete;
				typeOfmessageIsRecord = true;
				value.append(valueIncomplete);
			} else //its a message that passes between client and server and
				   //is not in db
			{

				string msgType(buf.getValueAsString());
				temp = msgType;
				if (!temp.empty()) {
					mylogger << " \n the msgType record as string is " << temp;

					typeOfmessageIsRecord = false;
					allMessage.append(temp);
				} else {
					mylogger << "\n the buffer is empty so we ignore it ";

				}
			}

		}                    //for

		if (typeOfmessageIsRecord) {
			mylogger
					<< "pico_message : convertBuffersToMessage : extracted key is "
					<< key << "\n value : " << value << "\n";

			pico_message pico_msg(key, value);
			return pico_msg;

		} else {
			mylogger
					<< "pico_message : convertBuffersToMessage : allMessage is "
					<< allMessage << "\n";

			pico_message pico_msg(allMessage);
			return pico_msg;

		}

	}
	//        void clear() {
	//            mylogger << ("pico_message : clearing the message ");
	//            hashCodeOfMessage = "";
	//            user = "";
	//            db = "";
	//            command = "";
	//            collection = "";
	//            key = "";
	//            value = "";
	//            oldvalue = "";
	//            json_form_of_message = "";
	//            messageSize = 0;
	//
	//        }
	~pico_message() {
		mylogger << ("pico_message being destroyed now.\n");
	}

};
}

#endif /* PICO_MESSAGE_H_ */

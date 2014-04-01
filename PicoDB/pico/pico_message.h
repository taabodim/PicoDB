/*
 * pico_message.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef PICO_MESSAGE_H_
#define PICO_MESSAGE_H_

#include "pico/pico_record.h"
#include <third_party/json_cpp/json/json.h>
#include <pico/pico_concurrent_list.h>
namespace pico {
class pico_message {
private:
	string uniqueMessageId;
public:
	std::string user;
	std::string db;
	std::string command;
	pico_record data;
	std::string collection;
	string raw_message;
	long messageSize;
	pico_message(std::string message_from_client) {//this is for processing shell commands
		raw_message = message_from_client;
		messageSize = sizeof(raw_message);
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;

		bool parsingSuccessful = reader.parse(raw_message, root);
		if (!parsingSuccessful) {
			// report to the user the failure and their locations in the document.
			std::cout << "Failed to parse message\n"
					<< reader.getFormattedErrorMessages();

			throw new pico_exception("failed to parse message from client");
		}

		command = root.get("command", "unknown").asString();
		collection = root.get("collection", "unknown").asString();
		db = root.get("db", "unknown").asString();
		user = root.get("user", "unknown").asString();

		std::string key = root.get("key", "unknown").asString();

		std::string value = root.get("value", "unknown").asString();
		data.setKeyValue(key, value);
		set_hash_code();
	}
    
    pico_message(std::string key,std::string value,std::string com,std::string database,std::string us
                 ,std::string col ) {
		//fix this part , you might need to convert a json doc to pico_message
        raw_message.append(key);
        raw_message.append(value);
        
        messageSize = sizeof(raw_message);
		
		command = command;
		collection = col;
		db = database;
		user = us;
        
        data.setKeyValue(key, value);
		set_hash_code();
	}

	void set_hash_code() {
		std::size_t h1 = std::hash<std::string>()(user);
		std::size_t h2 = std::hash<std::string>()(db);
		std::size_t h3 = std::hash<std::string>()(command);
		std::size_t h4 = std::hash<std::string>()(data.getString());
		std::size_t h5 = std::hash<std::string>()(collection);
		std::size_t h6 = std::hash<std::string>()(raw_message);

		size_t hash_code = (h1 ^ (h2 << 1) ^ h3 ^ h4 ^ h5 ^ h6);
		uniqueMessageId = boost::lexical_cast<string>(hash_code);
		//cout << "unique message id is " << uniqueMessageId << endl;
	}

	std::string toString() {
		return raw_message;
	}
	msgPtr convert_to_buffered_message() {
        cout<<"pico_message : converToBuffers "<<endl;
        std::shared_ptr<pico_concurrent_list<pico_buffer>>  all_buffers (new pico_concurrent_list<pico_buffer>);

		if (messageSize < pico_buffer::max_size) {

			pico_buffer buf(raw_message);
			buf.parentMessageId = uniqueMessageId;
			buf.parentSequenceNumber = 0;
			all_buffers->push(buf);
            msgPtr msg (new pico_buffered_message (all_buffers));
			return msg;
		}

		//break down the string

		
		long numberOfBuffer = 0;
		const char* temp_buffer_message  = new char [sizeof(raw_message)];
        temp_buffer_message = raw_message.c_str();
		cout<<"pico_message : message is too big , breaking down the huge string to a list of buffers ...... "<<endl;
        
        while (*temp_buffer_message != 0) {
			pico_buffer currentBuffer;
			currentBuffer.parentMessageId = uniqueMessageId;
            
            cout<<"pico_message : uniqureMessageId  "<<uniqueMessageId<<endl;
			
            for (int i = 0; i < pico_buffer::max_size; i++) {
				currentBuffer.parentSequenceNumber = numberOfBuffer;
				if (*temp_buffer_message != 0)
					currentBuffer.data_[i] = *temp_buffer_message;
				else {
					break;
				}
				++temp_buffer_message;
			}
             cout<<"pico_message : buffer pushed back to all_buffers "<<endl;
			all_buffers->push(currentBuffer);
			numberOfBuffer++;
		}
        msgPtr msg (new pico_buffered_message (all_buffers));
		return msg;
	}
	 char* convertMessageToArrayBuffer() {
         
		char* temp_raw_message  = new char [sizeof(raw_message)];
         cout<<"pico_message : convertMessageToArrayBuffer   "<<endl;
		const char* charOfMessage = raw_message.c_str();
		for (long i = 0; i < sizeof(raw_message); i++) {
			temp_raw_message[i] = *charOfMessage;
			++charOfMessage;
		}
         cout<<"pico_message : convertMessageToArrayBuffer temp_raw_message is  "<<(*temp_raw_message)<<endl;

		return temp_raw_message;
	}
	pico_message get_pico_message(list<pico_buffer> all_buffers) {
		string all_raw_msg;
		int seq_number = 0;
		while (!all_buffers.empty()) {
			//get rid of all buffers that are not for this messageId
			for (list<pico_buffer>::iterator it; it != all_buffers.end();
					++it) {
                cout<<"get_pico_message : parentSequenceNumber :  "<<it->parentSequenceNumber<<endl;
                cout<<"get_pico_message : seq_number :  "<<seq_number<<endl;

				if (it->parentSequenceNumber == seq_number) {
					all_raw_msg.append((it->getString()));
					seq_number++;
				}
			}
			cout<<"pico_message : all_raw_msg is "<<all_raw_msg<<endl;
		}
		pico_message pico_msg(all_raw_msg);
		return pico_msg;

	}
	pico_message() {
	    cout<<"pico_message being created by default constructor ";

    }
	~pico_message() {
        cout<<"pico_message being destroyed now."<<endl;
    }

	void set() {

	}
};
}

#endif /* PICO_MESSAGE_H_ */

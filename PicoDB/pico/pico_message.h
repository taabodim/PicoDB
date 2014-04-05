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
#include <logger.h>
namespace pico {
class pico_message {
private:
	string uniqueMessageId;
public:
	std::string user;
	std::string db;
	std::string command;
	std::string collection;
    std::string key_of_message;
    std::string value_of_message;
    std::string json_form_of_message;
	pico_buffered_message  buffered_message; //a container for all the buffers that make up this pico_message
	long messageSize;
    logger mylogger;
    
    pico_message(const pico_message& msg)
    {
        std::cout<<("pico_message copy constructor being called.\n");
        this->user = msg.user;
        this->db = msg.db;
        this->command = msg.command;
        this->collection = msg.collection;
        this->messageSize = msg.messageSize;
        this->key_of_message=key_of_message;
        this->value_of_message=value_of_message;
        this->json_form_of_message=json_form_of_message;
        this->buffered_message=buffered_message; //a container for all the buffers that make up this pico_message
    }
	pico_message(std::string message_from_client) {//this is for processing shell commands
		
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;

		bool parsingSuccessful = reader.parse(message_from_client, root);
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
		set_hash_code();
	}
    pico_message operator=(pico_message& msg)
    {
        std::cout<<("pico_message copy operator being called.\n");
        this->user = msg.user;
        this->db = msg.db;
        this->command = msg.command;
        this->collection = msg.collection;
        this->messageSize = msg.messageSize;
        this->key_of_message=key_of_message;
        this->value_of_message=value_of_message;
        this->json_form_of_message=json_form_of_message;
        this->buffered_message=buffered_message; //a container for all the buffers that make up this pico_message
  
        return *this;
    }
    std::string convert_message_to_json()
    {
        
        Json::Value root;   // will contains the root value after parsing.
        root["key"] = key_of_message;
        root["value"] = value_of_message;
        root["db"]= db;
        root["user"]= user;
        root["collection"]= collection;
        root["command"]= command;
        
        Json::StyledWriter writer;
        // Make a new JSON document for the configuration. Preserve original comments.
        std::string output = writer.write(root);

        return output;
    }
    pico_message(std::string key,std::string value,std::string com,std::string database,std::string us
                 ,std::string col){

        command = com;
		collection = col;
		db = database;
		user = us;
        key_of_message = key;
        value_of_message = value;
        json_form_of_message = convert_message_to_json();
        messageSize = json_form_of_message.size();
        
        set_hash_code();
        convert_to_buffered_message();
	}
   
	void set_hash_code() {
		std::size_t h1 = std::hash<std::string>()(user);
		std::size_t h2 = std::hash<std::string>()(db);
		std::size_t h3 = std::hash<std::string>()(command);
		std::size_t h4 = std::hash<std::string>()(collection);
		//std::size_t h6 = std::hash<std::string>()(data.getString());//find a solution to hash long strings

		size_t hash_code = (h1 ^ (h2 << 1) ^ h3 ^ h4 );
		uniqueMessageId = boost::lexical_cast<string>(hash_code);
		
        cout << "unique message id is " << uniqueMessageId << endl;
	}

	std::string toString() {
        return json_form_of_message;
	}
	void convert_to_buffered_message() {
        std::cout<<("pico_message : converToBuffers ");
        
        std::cout<<("pico_message : converToBuffers : messageSize is ");
        std::cout<<messageSize<<endl;
        
		if (messageSize < pico_buffer::max_size) {
            std::cout<<"pico_message : message size is less than max size"<<endl;
			pico_buffer buf ;
			buf.parentMessageId= uniqueMessageId;
			buf.parentSequenceNumber = 0;
			std::cout<<"pico_message : parentMessageId "<<buf.parentMessageId<<endl;
            buffered_message.append(buf);
			std::cout<<"pico_message : pico_buffered_message was created"<<endl;
        
		}

		//break down the string

		
		long numberOfBuffer = 0;
//		const char* temp_buffer_message  = new char [sizeof(data.getString())];
        const char* temp_buffer_message = json_form_of_message.c_str();
        std::cout<<("pico_message : message is too big ,data.getString() is ");
        std::cout<<(json_form_of_message);
		std::cout<<("pico_message : message is too big , breaking down the huge string to a list of buffers ...... ");
        std::cout<<"*temp_buffer_message  is "<<*temp_buffer_message <<endl;
        while (*temp_buffer_message != 0) {
			bufferType currentBuffer;
			currentBuffer.parentMessageId = uniqueMessageId;
            
            std::cout<<("pico_message : uniqureMessageId  ");
            std::cout<<(uniqueMessageId);
			
            for (int i = 0; i < pico_buffer::max_size; i++) {
				currentBuffer.parentSequenceNumber = numberOfBuffer;
				if (*temp_buffer_message != 0)
					currentBuffer.data_[i] = *temp_buffer_message;
				else {
					break;
				}
				++temp_buffer_message;
			}
             std::cout<<("pico_message : buffer pushed back to all_buffers ");
			 buffered_message.append(currentBuffer);
			numberOfBuffer++;
		}
       
    
        	}
	 char* convertMessageToArrayBuffer() {
         
		char* raw_msg  = new char [sizeof(json_form_of_message)];
         std::cout<<("pico_message : convertMessageToArrayBuffer  " );
		const char* charOfMessage = json_form_of_message.c_str();
		for (long i = 0; i < sizeof(json_form_of_message); i++) {
			raw_msg[i] = *charOfMessage;
			++charOfMessage;
		}
//         std::cout<<("pico_message : convertMessageToArrayBuffer temp_data.getString() is  "<<(*temp_data.getString())<<endl;

		return raw_msg;
	}
	pico_message get_pico_message(list<pico_buffer> all_buffers) {
		string all_raw_msg;
		int seq_number = 0;
		while (!all_buffers.empty()) {
			//get rid of all buffers that are not for this messageId
			for (list<pico_buffer>::iterator it; it != all_buffers.end();
					++it) {
//                std::cout<<("get_pico_message : parentSequenceNumber :  "<<it->parentSequenceNumber<<endl;
//                std::cout<<("get_pico_message : seq_number :  "<<seq_number<<endl;
//
				if (it->parentSequenceNumber == seq_number) {
					all_raw_msg.append((it->getString()));
					seq_number++;
				}
			}
			std::cout<<("pico_message : all_raw_msg is ");
           
            
            std::cout<<(all_raw_msg);
		}
		pico_message pico_msg(all_raw_msg);
		return pico_msg;

	}
    void clear()
    {
    	std::cout<<("pico_message : clearing the message ");
        uniqueMessageId="";
    user="";
        db="";
        command="";
        collection="";
        key_of_message="";
        value_of_message="";
        json_form_of_message="";
        buffered_message.clear(); //a container for all the buffers that make up this pico_message
         messageSize=0;
        
    }
	~pico_message() {
        std::cout<<("pico_message being destroyed now.");
    }

};
}

#endif /* PICO_MESSAGE_H_ */

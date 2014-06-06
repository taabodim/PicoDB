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
#include <pico/ConcurrentVector.h>
#include <logger.h>
#include <pico_logger_wrapper.h>
#include <pico/pico_utils.h>
#include <pico/pico_buffered_message.h>
#include <list>


namespace pico {
class pico_message;
typedef std::shared_ptr<pico_message> msgPtr;
class pico_message: public pico_logger_wrapper {
private:

//	pico_buffered_message<pico_record> recorded_message; //a container for all the records that make up the whole message as a full json with all the value
//	pico_buffered_message<pico_record> keyValueInBuffers;
//	//a container for all the records that make up the key value

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
	std::string hashCodeOfMessage;
	long messageSize;
//    bool shutDownNormally;
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
		this->hashCodeOfMessage = msg.hashCodeOfMessage;
	}
	pico_message operator=(const pico_message& msg) {

		mylogger << "\npico_message operator assignment being called.\n";

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
		this->hashCodeOfMessage = msg.hashCodeOfMessage;
		return *this;
	}
    Json::Reader reader;

    pico_message(const std::string message_from_client)
    //this is for converting a string to pico_message when we read it from
    //the other side
    {
		Json::Value root;   // will contains the root value after parsing.
		      
		bool parsingSuccessful = reader.parse(message_from_client, root);
        if (!parsingSuccessful) {
			// report to the user the failure and their locations in the document.
            mylogger << "Failed to parse message :"<<message_from_client<<"\n"
            << reader.getFormattedErrorMessages();
        }
        
        assert(parsingSuccessful);
        
		this->json_form_of_message = message_from_client;
		this->command = root.get("command", "unknown").asString();
        
        this->messageId = root.get("messageId", "couldntparseit").asString();
		this->collection = root.get("collection", "unknown").asString();
		this->db = root.get("db", "unknown").asString();
		this->user = root.get("user", "unknown").asString();
		this->key = root.get("key", "unknown").asString();
		this->value = root.get("value", "unknown").asString();
		this->oldvalue = root.get("oldvalue", "unknown").asString();
        this->set_hash_code();
    }
	pico_message(const std::string message_from_client, string msgId) {
		Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;

		bool parsingSuccessful = reader.parse(message_from_client, root);
        		if (!parsingSuccessful) {
			// report to the user the failure and their locations in the document.
			                mylogger << "Failed to parse message :"<<message_from_client<<"\n"
			              << reader.getFormattedErrorMessages();
           		}
       
        assert(parsingSuccessful);

		this->json_form_of_message = message_from_client;
		this->command = root.get("command", "unknown").asString();

//		this->messageId = root.get("messageId", "couldntparseit").asString();
		this->messageId = msgId;
		this->collection = root.get("collection", "unknown").asString();
		this->db = root.get("db", "unknown").asString();
		this->user = root.get("user", "unknown").asString();
		this->key = root.get("key", "unknown").asString();
		this->value = root.get("value", "unknown").asString();
		this->oldvalue = root.get("oldvalue", "unknown").asString();

		//this->json_key_value_pair = createTheKeyValuePair();
		this->set_hash_code();
      }

	pico_message(const std::string json_message_from_client, bool simpleMessage,
			string msgId) {
        Json::Value root;   // will contains the root value after parsing.
		Json::Reader reader;
        
		//this is for processing shell commands
		this->messageId = msgId;
		this->json_form_of_message = json_message_from_client;
        bool parsingSuccessful = reader.parse(json_message_from_client, root);
        assert(parsingSuccessful);
		//this->json_key_value_pair = createTheKeyValuePair(); this line should always be commented because when the msg is simple
		//it will mess up the parser
		if (!json_message_from_client.empty()) {
			this->set_hash_code();

			//		this->convert_key_value_buffered_message();
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
	static msgPtr build_message_from_string(const string value,
			string messageId) {

		msgPtr msg (new pico_message(value, true, messageId)); //the message id is very important to be set here ,
		//it connects the response to the right request after request processor
		// processes it
		return msg;
	}


    bool operator==(const pico_message& msg){
        //this is used for removing the message
        //from the responseQueue
        if(
           (this->messageId.compare(msg.messageId)==0) &&
           (this->key.compare(msg.key)==0) &&
           (this->json_form_of_message.compare(msg.json_form_of_message)==0) &&
           (this->db.compare(msg.db)==0) &&
           (this->collection.compare(msg.collection)==0) &&
           (this->value.compare(msg.value)==0) 
           
           )
            return true;
        
        return false;
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
		//this->json_key_value_pair = msg.json_key_value_pair;
		this->hashCodeOfMessage = msg.hashCodeOfMessage;
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

		this->command = com;
		this->collection = col;
		this->db = database;
		this->user = us;
		this->key = newKey;
		this->value = newValue;
		this->messageId = calc_request_id();
		this->json_form_of_message = convert_message_to_json();
		this->messageSize = json_form_of_message.size();
 		set_hash_code();

	}
	pico_message(std::string newKey, std::string old_value_arg,
			std::string newValue, std::string com, std::string database,
			std::string us, std::string col) {
		this->command = com;
		this->collection = col;
		this->db = database;
		this->user = us;
		this->key = newKey;
		this->oldvalue = old_value_arg;
		this->value = newValue;
		this->messageId = calc_request_id();
		this->json_form_of_message = convert_message_to_json();
		this->messageSize = json_form_of_message.size();
		set_hash_code();

	}
	pico_message(std::string keyFromDB, std::string valueFromDB,
			string messageIdArg) {
		//this is used when we want to create a nice pico message
		//out of array of buffers
		this->command = "unknown";
		this->collection = "unknown";
		this->db = "unknown";
        this->user = "unknown";
            this->key = keyFromDB;
		this->value = valueFromDB;
		this->messageId = messageIdArg;
		this->json_form_of_message = convert_message_to_json();
		this->messageSize = json_form_of_message.size();
     	set_hash_code();

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
    std::shared_ptr<pico_record> getCompleteMessageInJsonAsOnBuffer()
    {
        std::shared_ptr<pico_record> recordPtr(new pico_record(json_form_of_message));
        return recordPtr;
    }

	~pico_message() {
        
       // assert(shutDownNormally);
		logger.log("pico_message being destroyed now.\n");
        
	}

    static  std::shared_ptr<pico_message> emptyInstance() //for template class
    {
        return NULL;
    }
    
};
    
    

    
      
}

#endif /* PICO_MESSAGE_H_ */

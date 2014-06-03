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

	//this function converts all the message to a list of pico_records , and
	//puts the all the message in the value part of records only
	//and set the messageId and append marker in the related parts in the data_ array in the records
	//the whole message that passes between client and server

//	pico_buffered_message<pico_record>& getCompleteMessageInJsonAsBuffers()
//	//because the return value is attached
//	//to obejct we can return by reference
//	{
//		if (mylogger.isTraceEnabled()) {
//			mylogger
//					<< " \npico_message : getCompleteMessageInJsonAsBuffers being called \n";
//		}
//        assert(!messageId.empty());
//		
//		return recorded_message;
//	}
	
//	pico_buffered_message<pico_record>& getKeyValueOfMessageInRecords()
//	//this method will add BEG KEY and CONKEY to the first and rest of buffers
//	{
//
//		if (mylogger.isTraceEnabled()) {
//			mylogger << " pico_message : getKeyValueOfMessageInRecords  : \n";
//		}
//
//		keyValueInBuffers = convert_message_to_records(this->value, true);
//
//		if (mylogger.isTraceEnabled()) {
//			mylogger << " pico_message : keyValueInBuffers is : \n";
//			keyValueInBuffers.print();
//		}
//		return keyValueInBuffers;
//	}

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
//    std::shared_ptr<pico_message>  convert_records_to_message(
//			pico_buffered_message<pico_record>& all_buffers, string messageIdArg,
//                                                              messageType type,std::shared_ptr<logger> mylogger) {
//        assert(!messageIdArg.empty());
//		if (mylogger->isTraceEnabled()) {
//			*mylogger<< "\n pico_message :  this is the start of the convert_records_to_message function \n";
//		}
//		all_buffers.print();
//        assert(all_buffers.size()>0);
//		string allMessage;
//		string key;
//		string value;
//        
//        
//		//pico_message msgToReturnEmpty;
//        std::shared_ptr<pico_message> msgToReturnEmpty(new pico_message());
//        
//        int sequenceNumber = 0;
//        assert(!all_buffers.msg_in_buffers->empty());
//		while (!all_buffers.msg_in_buffers->empty()) {
//            
//			pico_record buf = all_buffers.msg_in_buffers->pop(sequenceNumber);//pop a a record
//           
//			if (mylogger->isTraceEnabled()) {
//				*mylogger << " \n buffer popped at sequence Number "<<sequenceNumber<<" is \n " << buf.toString();
//			}
//            
//
//            //with specific sequence
//            sequenceNumber++;
//			
//            			if (type.compare(
//					COMPLETE_MESSAGE_AS_JSON_FORMAT_WITHOUT_BEGKEY_CONKEY)
//					== 0) {
//				string msgType(buf.getValueAsString());
//				allMessage.append(msgType);
//
//			} else if (type.compare(
//					LONG_MESSAGE_JUST_KEY_VALUE_WITH_BEGKEY_CONKEY) == 0) {
//
//				if (pico_record::recordStartsWithBEGKEY(buf)) {
//
//					if (mylogger->isTraceEnabled()) {
//						*mylogger<< "\n pico_message : convertBuffersToMessage : buffer starts with BEGKEY \n";
//					}
//					if(this->key.empty())
//                    {
//                        this->key = buf.getKeyAsString();
//                    }
//					if (mylogger->isTraceEnabled()) {
//						*mylogger<< "\n key extracted from key record as string is "<< this->key;
//					}
//                    assert(!this->key.empty());
//					string valueExtracted = buf.getValueAsString();
//					if (mylogger->isTraceEnabled()) {
//						*mylogger<< "\n the value part of key record as string is "<< valueExtracted;
//					}
//                    assert(!valueExtracted.empty());
//					
//					value.append(valueExtracted);
//				}	//its a begkey record
//				//if buffer is continuing key
//				else if (pico_record::recordStartsWithConKEY(buf)) {
//					//remove the marker
//					if (mylogger->isTraceEnabled()) {
//						*mylogger<< "\n pico_message : convertBuffersToMessage : buffer starts with CONKEY ";
//					}
//					string valueIncomplete = buf.getValueAsString();
//					if (mylogger->isTraceEnabled()) {
//						*mylogger<< "\n the incomplete value part of contining record as string is "<< valueIncomplete;
//					}
//                    assert(!valueIncomplete.empty());
//					value.append(valueIncomplete);
//				} else //its a message that passes between client and server and
//					   //is not in db
//				{
//					//log error
//					*mylogger<< "\n  record is neither BEGKEY NOR CONKEY, its a serious error.......................... ";
//
//				}
//
//			} else {
//				*mylogger<< "\n  record is neither BEGKEY NOR CONKEY nor complete JSON message without KEYS, its a serious error... ";
//
//			}
//
//		}                //end of while
//
//        assert(
//               (type.compare(COMPLETE_MESSAGE_AS_JSON_FORMAT_WITHOUT_BEGKEY_CONKEY)
//               == 0) ||
//               type.compare(LONG_MESSAGE_JUST_KEY_VALUE_WITH_BEGKEY_CONKEY)
//               == 0);
//               
//		if (type.compare(COMPLETE_MESSAGE_AS_JSON_FORMAT_WITHOUT_BEGKEY_CONKEY)
//				== 0) {
//            assert(!allMessage.empty());
//            assert(!messageIdArg.empty());
//			//pico_message pico_msg(allMessage, messageIdArg);
//            std::shared_ptr<pico_message> pico_msg(new pico_message(allMessage, messageIdArg));
//            
//            assert(!pico_msg->messageId.empty());
//            this->shutDownNormally = true;
//			return pico_msg;
//
//		} else if (type.compare(LONG_MESSAGE_JUST_KEY_VALUE_WITH_BEGKEY_CONKEY)
//				== 0) {
//			*mylogger<< "pico_message : convert_records_to_message : extracted key is "<< key << "\n value : " << value << "\n";
//            assert(!this->key.empty());
//            assert(!value.empty());
//            assert(!messageIdArg.empty());
//                   
//            //pico_message pico_msg(this->key, value, messageIdArg);
//            std::shared_ptr<pico_message> pico_msg(new pico_message(this->key, value, messageIdArg));
//            assert(!pico_msg->messageId.empty());
//			this->shutDownNormally = true;
//            return pico_msg;
//
//		}
//
//		*mylogger<< "pico_message : convertBuffersToMessage : should never reach here \n";
//        
//		return msgToReturnEmpty; //it should never reaches here
//	}
//	pico_buffered_message<pico_record> convert_message_to_records(
//			string theMessageToConvertToBuffers, bool addBEGKEY_CONKEY) {
//        pico_buffered_message<pico_record> buffersContainingMessage;
//
//        try{
//				size_t sizeOfMessage = theMessageToConvertToBuffers.length();
//		long numOfBuffersNeededForThisMessage = (sizeOfMessage
//				/ pico_record::max_value_size) + 1;
//
//		if (mylogger.isTraceEnabled()) {
//			mylogger
//					<< " number of records needed to contain the key value of this message is "
//					<< numOfBuffersNeededForThisMessage << "\n theMessageToConvertToBuffers : "
//                    <<theMessageToConvertToBuffers<<"\n";
//		}
//		for (int i = 0, indexInJsonMessage = 0;
//				i < numOfBuffersNeededForThisMessage; i++) {
//            
//			string valueForThisBuffer;
//			if ((indexInJsonMessage + pico_record::max_value_size)
//					< sizeOfMessage) {
//				valueForThisBuffer.append(
//						theMessageToConvertToBuffers.substr(indexInJsonMessage,
//								indexInJsonMessage
//										+ pico_record::max_value_size));
//			} else {
//				valueForThisBuffer.append(
//						theMessageToConvertToBuffers.substr(
//								indexInJsonMessage));
//
//			}
//			indexInJsonMessage += pico_record::max_value_size;
//			if (mylogger.isTraceEnabled()) {
//
//				mylogger
//						<< "\n convert_message_to_records : valueForThisBuffer is "
//						<< valueForThisBuffer;
//			}
//
//			pico_record currentBuffer;
//			if (addBEGKEY_CONKEY == true && i == 0) {
//				//add the BEGKEY to the first record
//				pico_record::addKeyMarkerToRecord(currentBuffer,
//						pico_record::BEGKEY);
//
//
//
//			} else if (addBEGKEY_CONKEY == true && i > 0) {
//				//add the CONKEY to the the other records record
//				pico_record::addKeyMarkerToRecord(currentBuffer,
//						pico_record::CONKEY);
//
//			}
//
//			//add the key part to the key part of the record
//			if (addBEGKEY_CONKEY == true )
//				{
//				pico_record::setTheKeyInData(currentBuffer, this->key);
//				
//				}
//            
//            assert(!messageId.empty());
//            
//			pico_record::setTheValueInData(currentBuffer, valueForThisBuffer);
//            pico_record::setTheMessageIdInData(currentBuffer, messageId);
//            pico_record::setTheSequenceNumberInData(currentBuffer,i); //i is the sequenceNumber
//			pico_record::addAppendMarkerToTheEnd(currentBuffer); //add append
//			buffersContainingMessage.append(currentBuffer);
//            
//		}

//		pico_record::removeTheAppendMarker(
//				buffersContainingMessage.getLastBuffer()); //removes the append marker from the last record
//        
//        assert(!pico_record::IsThisRecordAnAddOn(*buffersContainingMessage.getLastBuffer()));
//		} catch (std::exception& e) {
//            std::cerr << "Exception: in pico_message: convert_message_to_records function " << e.what() << "\n";
//            raise(SIGABRT);
//        } catch (...) {
//            std::cerr << "Exception: unknown happened in pico_message: convert_message_to_records function" << "\n";
//            raise(SIGABRT);
//        }
//        
//        this->shutDownNormally = true;
//        return buffersContainingMessage;
//	}
   
    
	~pico_message() {
        
       // assert(shutDownNormally);
//		logger << ("pico_message being destroyed now.\n");
        
	}

    static  std::shared_ptr<pico_message> emptyInstance() //for template class
    {
        return NULL;
    }
    
};
    
    

    
      
}

#endif /* PICO_MESSAGE_H_ */

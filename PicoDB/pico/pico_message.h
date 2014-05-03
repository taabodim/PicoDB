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
#include <pico_logger_wrapper.h>
#include <pico/pico_utils.h>
#include <pico/pico_buffered_message.h>
#include <list>
namespace pico {
    class pico_message : public pico_logger_wrapper{
    private:
        
    public:
        std::string key;
        std::string value;
        std::string oldvalue;//for update messages
        std::string user;
        std::string db;
        std::string command;
        std::string requestId;//each command that passes must have an Id
        //then when server sends the response, the server sends the same id back to
        //client then the client knows , this reply message is for which request
        
        std::string collection;
        std::string json_form_of_message;
        std::string json_key_value_pair;
        std::string uniqueMessageId;
       
        pico_buffered_message<pico_record> key_value_buffered_message;
        ////key value pair that came with this message in record list to
        //be saved in db
        
        pico_buffered_message<pico_record> recorded_message;//a container for all the records that make up the
        
        long messageSize;
     
        pico_message() {
        }
        pico_message(const pico_message& msg) {
            //            mylogger<<"pico_message copy constructor being called.\n";
            this->user = msg.user;
            this->db = msg.db;
            this->command = msg.command;
            this->requestId= msg.requestId;
            this->collection = msg.collection;
            this->messageSize = msg.messageSize;
            this->key = msg.key;
            this->value = msg.value;
            this->oldvalue= msg.oldvalue;
            this->json_form_of_message = msg.json_form_of_message;
            this->json_key_value_pair = msg.json_key_value_pair;
        
            this->uniqueMessageId=msg.uniqueMessageId;
            this->recorded_message = msg.recorded_message;
            this->key_value_buffered_message= msg.key_value_buffered_message;
            
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
            this->requestId=root.get("requestId", "0").asString();
            this->collection = root.get("collection", "unknown").asString();
            this->db = root.get("db", "unknown").asString();
            this->user = root.get("user", "unknown").asString();
            this->key = root.get("key", "unknown").asString();
            this->value = root.get("value", "unknown").asString();
            this->oldvalue= root.get("oldvalue", "unknown").asString();
            
            this->json_key_value_pair = createTheKeyValuePair();
            this->set_hash_code();
            this->convert_to_buffered_message();
                this->convert_key_value_buffered_message();
          }
        
        pico_message(const std::string message_from_client, bool simpleMessage) { //this is for processing shell commands
            this->json_form_of_message = message_from_client;
            //this->json_key_value_pair = createTheKeyValuePair(); this line should always be commented because when the msg is simple
            //it will mess up the parser
            if(!message_from_client.empty())
            {
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
        static pico_message build_message_from_string(const string value) {
            
            pico_message msg(value, true);
            return msg;
        }
        
        static pico_message build_complete_message_from_string(string value) {
            Json::Value root;   // will contains the root value after parsing.
            root["key"] = "simpleMessage";
            root["value"] = value;
            root["oldvalue"] = "unknown";
            root["db"] = "unknown";
            root["user"] = "unknown";
            root["collection"] = "unknown";
            root["command"] = "unknown";
            root["requestId"]= convertToString(calc_request_id());
            root["hashCode"] = "unknown";
            
            Json::StyledWriter writer;
            // Make a new JSON document for the configuration. Preserve original comments.
            std::string output = writer.write(root);
            pico_message msg(output);
            return msg;
        }
        static pico_message build_complete_message_from_key_value_pair(string key,string value) {
            Json::Value root;   // will contains the root value after parsing.
            root["key"] = key;
            root["value"] = value;
            root["oldvalue"] = "unknown";
            root["db"] = "unknown";
            root["user"] = "unknown";
            root["collection"] = "unknown";
            root["command"] = "unknown";
            root["requestId"]= convertToString(calc_request_id());
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
            this->requestId= msg.requestId;
            this->collection = msg.collection;
            this->messageSize = msg.messageSize;
            this->key = msg.key;
            this->value = msg.value;
            this->oldvalue = msg.oldvalue;
            this->json_form_of_message = msg.json_form_of_message;
            this->json_key_value_pair = msg.json_key_value_pair;
            this->uniqueMessageId=msg.uniqueMessageId;
            this->recorded_message=msg.recorded_message;
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
            root["requestId"]= convertToString(requestId);
            root["hashCode"] = uniqueMessageId;
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
            requestId = calc_request_id();
            json_form_of_message = convert_message_to_json();
            messageSize = json_form_of_message.size();
            json_key_value_pair = createTheKeyValuePair();
            set_hash_code();
            convert_to_buffered_message();
            convert_key_value_buffered_message();
        }
        pico_message(std::string newKey,std::string old_value_arg, std::string newValue, std::string com,std::string database, std::string us, std::string col) {
            command = com;
            collection = col;
            db = database;
            user = us;
            key = newKey;
            oldvalue = old_value_arg;
            value = newValue;
            requestId = calc_request_id();
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
            this->uniqueMessageId = boost::lexical_cast < string > (hash_code);
            
           // cout << "unique message id is " << this->uniqueMessageId << endl;
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
                        int i=0;
                        while (*keyArray != 0) {
                            continuingRecord.data_[i]=*keyArray;
                            ++i;
                            ++keyArray;
                    }//the key marker is put to first 6 letters of the first record
        
                }
        void convert_key_value_buffered_message() {
            
            mylogger
          << "pico_message : convert_to_list_of_records : messageSize is "
            << messageSize <<"\n";
            
            
            long numberOfRecords = 0;
            
            
            pico_record firstRecord;
            
            
            pico_record::addKeyMarkerToFirstRecord(firstRecord);
            
            const char* keyArray = key.c_str();
            int i=6;
            while (*keyArray != 0) {
                firstRecord.data_[i]=*keyArray;
                ++i;
                ++keyArray;
            }//the key is put to first record
            for(int k=i;k<pico_record::max_key_size;k++)
            {
                firstRecord.data_[i] = 0;
            }//put 0 after the key and before value
            
            const char* valueArray = value.c_str();
            bool dataEnded =false;
            for(int j=pico_record::max_key_size;j<pico_record::max_size-6;j++)
            {
                if(*valueArray!=0)
                {
                    firstRecord.data_[j] = *valueArray;
                    ++valueArray;
                }else{
                    dataEnded = true;
                    break;
                    
                }
            }//the value of the first record is populated with values
            
            if(!dataEnded)
            {//there is more data to be appended
                addAppendMarkerToTheEnd(firstRecord);

            }
            bool moreThanOneRecord = false;
            key_value_buffered_message.append(firstRecord);
			while (*valueArray != 0) {
                pico_record currentRecord;
                
                
                pico_record::replicateTheFirstRecordKeyToOtherRecords(firstRecord,currentRecord);
                
                for (int i = 0; i < pico_record::max_size-6 ; i++) {
                    
                    if (*valueArray != 0) {
                        currentRecord.data_[i] = *valueArray;
                    } else {
                        dataEnded= true;
                        break;
                    }//adding the rest of values to the second and third and etc records
                    ++valueArray;
                }
                
                addAppendMarkerToTheEnd(currentRecord);
                moreThanOneRecord = true;
                key_value_buffered_message.append(currentRecord);
            }
            if(moreThanOneRecord)
            {
            removeTheAppendMarker(key_value_buffered_message.getLastBuffer());
            }
            
        }
        
        
        //this function converts all the message to a list of pico_records
        //the whole message that passes between client and server
        void convert_to_buffered_message() {
            
//            mylogger << "pico_message : converToBuffers : messageSize is "
          //  << messageSize << endl;
            bool appendAllExceptLastOne = false;
            
            if (messageSize > pico_record::max_size - 6) {
                appendAllExceptLastOne = true;
            }
            
            long numberOfBuffer = 0;
            
            const char* temp_buffer_message = json_form_of_message.c_str();
            //mylogger<<("pico_message : message is too big ,data.getString() is ");
            //  mylogger<<(json_form_of_message);
            //mylogger<<("pico_message : message is too big , breaking down the huge string to a list of buffers ...... ");
            // mylogger<<"*temp_buffer_message  is "<<*temp_buffer_message <<endl;
            while (*temp_buffer_message != 0) {
                pico_record currentBuffer;
                currentBuffer.parentMessageId = uniqueMessageId;
                
                mylogger << "\npico_message : uniqureMessageId  "<< uniqueMessageId;
                
                
                
                for (int i = 0; i < pico_record::max_size - 6; i++) {
                    currentBuffer.parentSequenceNumber = numberOfBuffer;
                    if (*temp_buffer_message != 0) {
                        currentBuffer.data_[i] = *temp_buffer_message;
                    } else {
                        break;
                    }
                    ++temp_buffer_message;
                }
                
                mylogger<<("\npico_message : buffer pushed back to all_buffers ");
                addAppendMarkerToTheEnd(currentBuffer); //add append at the last characters of all, except the last one which completes the message
                //throws buffer overflow
//                mylogger
//                << "pico_message : currentBuffer after append except the last one is "
//                << currentBuffer.toString();
                
                recorded_message.append(currentBuffer);
                
                numberOfBuffer++;
            }
            removeTheAppendMarker(recorded_message.getLastBuffer());
            
        }
        static void removeTheAppendMarker(list<pico_record>::iterator currentBuffer) {
            int pos = pico_record::max_size - 1;
            currentBuffer->data_[pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            
        }
        static void removeTheAppendMarker(std::shared_ptr<pico_record> currentBuffer) {
            int pos = pico_record::max_size - 1;
            currentBuffer->data_[pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            
        }
        
        void addAppendMarkerToTheEnd(pico_record& currentBuffer)
        
        {
            int pos = pico_record::max_size - 1;
            currentBuffer.data_[pos] = 'd';
            currentBuffer.data_[--pos] = 'n';
            currentBuffer.data_[--pos] = 'e';
            currentBuffer.data_[--pos] = 'p';
            currentBuffer.data_[--pos] = 'p';
            currentBuffer.data_[--pos] = 'a';
            
        }
        char* convertMessageToArrayBuffer() {
            
            char* raw_msg = new char[sizeof(json_form_of_message)];
            mylogger << ("pico_message : convertMessageToArrayBuffer  ");
            const char* charOfMessage = json_form_of_message.c_str();
            for (long i = 0; i < sizeof(json_form_of_message); i++) {
                raw_msg[i] = *charOfMessage;
                ++charOfMessage;
            }
            
            
            return raw_msg;
        }
        
        
//        std::shared_ptr<pico_concurrent_list<type>> msg_in_buffers
        
        static pico_message convertBuffersToMessage(pico_buffered_message<pico_record> all_buffers
                                                  
                                                    )
        {
                                                        bool getTheKey=true;
            string all_raw_msg;
            string key;
                                                        string all_values;
            while (!all_buffers.msg_in_buffers->empty())
            {
                //get rid of all buffers that are not for this messageId
                pico_record buf =all_buffers.msg_in_buffers->pop();
                string temp (buf.data_,buf.max_size);
                all_raw_msg.append(temp);
                }//for
               
                             std::cout << "pico_message : convertBuffersToMessage : all_raw_msg is "<<all_raw_msg<<"\n";
        
            pico_message pico_msg (all_raw_msg);
            return pico_msg;
            
            
        }
        void clear() {
            mylogger << ("pico_message : clearing the message ");
            uniqueMessageId = "";
            user = "";
            db = "";
            command = "";
            collection = "";
            key = "";
            value = "";
            oldvalue = "";
            json_form_of_message = "";
            messageSize = 0;
            
        }
        ~pico_message() {
            //  mylogger<<("pico_message being destroyed now.\n");
        }
        
    };
}

#endif /* PICO_MESSAGE_H_ */

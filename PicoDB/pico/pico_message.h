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
        
    public:
        std::string key;
        std::string value;
        std::string user;
        std::string db;
        std::string command;
        std::string collection;
        std::string json_form_of_message;
        std::string json_key_value_pair;
        std::string uniqueMessageId;
        pico_buffered_message<bufferType> buffered_message; //a container for all the buffers that make up this pico_message
        
        pico_buffered_message<pico_record> recorded_message;//a container for all the records that make up the
    //key value pair that came with this message
        
        long messageSize;
        //logger mylogger;
        pico_message() {
        }
        pico_message(const pico_message& msg) {
            //            std::cout<<"pico_message copy constructor being called.\n";
            this->user = msg.user;
            this->db = msg.db;
            this->command = msg.command;
            this->collection = msg.collection;
            this->messageSize = msg.messageSize;
            this->key = msg.key;
            this->value = msg.value;
            this->json_form_of_message = msg.json_form_of_message;
            this->json_key_value_pair = msg.json_key_value_pair;
            this->buffered_message = msg.buffered_message; //a container for all the buffers that make up this pico_message
            this->set_hash_code();
            this->convert_to_buffered_message();
            this->convert_to_list_of_records();
            
        }
        pico_message(const std::string message_from_client) {
            Json::Value root;   // will contains the root value after parsing.
            Json::Reader reader;
            
            bool parsingSuccessful = reader.parse(message_from_client, root);
            if (!parsingSuccessful) {
                // report to the user the failure and their locations in the document.
                std::cout << "Failed to parse message\n"
                << reader.getFormattedErrorMessages();
                
                throw new pico_exception("failed to parse message from client");
            }
            this->json_form_of_message = message_from_client;
            this->command = root.get("command", "unknown").asString();
            this->collection = root.get("collection", "unknown").asString();
            this->db = root.get("db", "unknown").asString();
            this->user = root.get("user", "unknown").asString();
            this->key = root.get("key", "unknown").asString();
            this->value = root.get("value", "unknown").asString();
            
            this->json_key_value_pair = createTheKeyValuePair();
            this->set_hash_code();
            this->convert_to_buffered_message();
        this->convert_to_list_of_records();
        }
        
        pico_message(const std::string message_from_client, bool simpleMessage) { //this is for processing shell commands
            this->json_form_of_message = message_from_client;
            this->json_key_value_pair = createTheKeyValuePair();
            this->set_hash_code();
            this->convert_to_buffered_message();
            this->convert_to_list_of_records();
        }
        std::string createTheKeyValuePair() {
            
            Json::Value root;   // will contains the root value after parsing.
            root["key"] = key;
            root["value"] = value;
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
            root["db"] = "unknown";
            root["user"] = "unknown";
            root["collection"] = "unknown";
            root["command"] = "unknown";
            root["hashCode"] = "unknown";
            Json::StyledWriter writer;
            // Make a new JSON document for the configuration. Preserve original comments.
            std::string output = writer.write(root);
            pico_message msg(output);
            return msg;
        }
        pico_message operator=(pico_message& msg) {
            std::cout << ("pico_message copy operator being called.\n");
            this->user = msg.user;
            this->db = msg.db;
            this->command = msg.command;
            this->collection = msg.collection;
            this->messageSize = msg.messageSize;
            this->key = msg.key;
            this->value = msg.value;
            this->json_form_of_message = msg.json_form_of_message;
            this->buffered_message = msg.buffered_message; //a container for all the buffers that make up this pico_message
            this->json_key_value_pair = createTheKeyValuePair();
            this->set_hash_code();
            this->convert_to_buffered_message();
            this->convert_to_list_of_records();
            return *this;
        }
        std::string convert_message_to_json() {
            
            Json::Value root;   // will contains the root value after parsing.
            root["key"] = key;
            root["value"] = value;
            root["db"] = db;
            root["user"] = user;
            root["collection"] = collection;
            root["command"] = command;
            root["hashCode"] = uniqueMessageId;
            Json::StyledWriter writer;
            // Make a new JSON document for the configuration. Preserve original comments.
            std::string output = writer.write(root);
            
            return output;
        }
        pico_message(std::string key, std::string value, std::string com,
                     std::string database, std::string us, std::string col) {
            
            command = com;
            collection = col;
            db = database;
            user = us;
            key = key;
            value = value;
            json_form_of_message = convert_message_to_json();
            messageSize = json_form_of_message.size();
            json_key_value_pair = createTheKeyValuePair();
            set_hash_code();
            convert_to_buffered_message();
        }
        
        void set_hash_code() {
            std::size_t h1 = std::hash<std::string>()(user);
            std::size_t h2 = std::hash<std::string>()(db);
            std::size_t h3 = std::hash<std::string>()(command);
            std::size_t h4 = std::hash<std::string>()(collection);
            std::size_t h5 = std::hash<std::string>()(key); //find a solution to hash long strings
            std::size_t h6 = std::hash<std::string>()(value); //find a solution to hash long
            
            size_t hash_code = (h1 ^ (h2 << 1) ^ h3 ^ h4 ^ h5 ^ h6);
            uniqueMessageId = boost::lexical_cast < string > (hash_code);
            
            cout << "unique message id is " << uniqueMessageId << endl;
        }
        
        std::string toString() const {
            return json_form_of_message;
        }
        std::string toKeyValuePairString() const {
            return json_key_value_pair;
        }
        void convert_to_list_of_records() {
            
            std::cout
            << "pico_message : convert_to_list_of_records : messageSize is "
            << messageSize << endl;
            
            
            long numberOfRecords = 0;
            
            
            pico_record firstRecord;
            const char* keyArray = key.c_str();
            int i=0;
            while (*keyArray != 0) {
                firstRecord.key_[i]=*keyArray;
                ++i;
                ++keyArray;
            }//the key is put to first record
            
            const char* valueArray = value.c_str();
            for(int j=i;j<pico_record::max_value_size;j++)
            {
                if(*valueArray!=0)
                {
                    firstRecord.value_[j] = *valueArray;
                    ++valueArray;
                }else{
                    break;
                }
            }//the value of the first record is populated with values
            
            recorded_message.append(firstRecord);
			while (*valueArray != 0) {
                pico_record currentRecord;
                
                for (int i = 0; i < pico_record::max_value_size ; i++) {
                    
                    if (*valueArray != 0) {
                        currentRecord.value_[i] = *valueArray;
                    } else {
                        break;
                    }
                    ++valueArray;
                }
                recorded_message.append(currentRecord);
               }
           
            
        }
        void convert_to_buffered_message() {
            
            std::cout << "pico_message : converToBuffers : messageSize is "
            << messageSize << endl;
            bool appendAllExceptLastOne = false;
            
            if (messageSize > pico_buffer::max_size - 6) {
                appendAllExceptLastOne = true;
            }
            
            long numberOfBuffer = 0;
            
            const char* temp_buffer_message = json_form_of_message.c_str();
            //std::cout<<("pico_message : message is too big ,data.getString() is ");
            //  std::cout<<(json_form_of_message);
            //std::cout<<("pico_message : message is too big , breaking down the huge string to a list of buffers ...... ");
            // std::cout<<"*temp_buffer_message  is "<<*temp_buffer_message <<endl;
            while (*temp_buffer_message != 0) {
                bufferType currentBuffer;
                currentBuffer.parentMessageId = uniqueMessageId;
                
                std::cout << "\npico_message : uniqureMessageId  "
                << uniqueMessageId << endl;
                
                for (int i = 0; i < pico_buffer::max_size - 6; i++) {
                    currentBuffer.parentSequenceNumber = numberOfBuffer;
                    if (*temp_buffer_message != 0) {
                        currentBuffer.data_[i] = *temp_buffer_message;
                    } else {
                        break;
                    }
                    ++temp_buffer_message;
                }
                
                //                std::cout<<("pico_message : buffer pushed back to all_buffers ");
                addTheEndingTagsToAllBuffers(currentBuffer); //add append at the last characters of all, except the last one which completes the message
                std::cout
                << "pico_message : currentBuffer after append except the last one is "
                << currentBuffer.toString();
                
                buffered_message.append(currentBuffer);
                
                numberOfBuffer++;
            }
            removeTheEndingTags(buffered_message.getLastBuffer());
            
        }
        void removeTheEndingTags(list<bufferType>::iterator currentBuffer) {
            int pos = pico_buffer::max_size - 1;
            currentBuffer->data_[pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            
        }
        static void removeTheEndingTags(bufferTypePtr currentBuffer) {
            int pos = pico_buffer::max_size - 1;
            currentBuffer->data_[pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            currentBuffer->data_[--pos] = '\0';
            
        }
        
        void addTheEndingTagsToAllBuffers(bufferType& currentBuffer)
        
        {
            int pos = pico_buffer::max_size - 1;
            currentBuffer.data_[pos] = 'd';
            currentBuffer.data_[--pos] = 'n';
            currentBuffer.data_[--pos] = 'e';
            currentBuffer.data_[--pos] = 'p';
            currentBuffer.data_[--pos] = 'p';
            currentBuffer.data_[--pos] = 'a';
            
        }
        char* convertMessageToArrayBuffer() {
            
            char* raw_msg = new char[sizeof(json_form_of_message)];
            std::cout << ("pico_message : convertMessageToArrayBuffer  ");
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
                        all_raw_msg.append((it->toString()));
                        seq_number++;
                    }
                }
                std::cout << ("pico_message : all_raw_msg is ");
                
                std::cout << (all_raw_msg);
            }
            pico_message pico_msg(all_raw_msg);
            return pico_msg;
            
        }
        void clear() {
            std::cout << ("pico_message : clearing the message ");
            uniqueMessageId = "";
            user = "";
            db = "";
            command = "";
            collection = "";
            key = "";
            value = "";
            json_form_of_message = "";
            buffered_message.clear(); //a container for all the buffers that make up this pico_message
            messageSize = 0;
            
        }
        ~pico_message() {
            //  std::cout<<("pico_message being destroyed now.\n");
        }
        std::string getKey() const {
            return this->key;
        }
        
        std::string getValue() const {
            return this->value.substr();
        }
        std::string getCommand() const {
            return this->command;
        }
        std::string getCollection() const {
            return this->collection;
        }
        std::string getDB() const {
            return this->db;
        }
        std::string getUser() const {
            return this->user;
        }
    };
}

#endif /* PICO_MESSAGE_H_ */

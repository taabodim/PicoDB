/*
 * requestProcessor.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef REQUESTPROCESSOR_H_
#define REQUESTPROCESSOR_H_
#include <pico/pico_message.h>
#include <pico/pico_utils.h>
#include <logger.h>
using namespace std;
namespace pico {
    class request_processor {
    private:
        std::string insertCommand;
        std::string deleteCommand;
        std::string updateCommand;
        std::string findCommand;
        std::string addUserToDBCommand;
        std::string deleteUserToDBCommand;
        logger mylogger;
        
    public:
        static string logFileName;
        
        pico_message processRequest(const string messageFromClient) {
            
            string logMsg("request_processor : this is the message that is going to be processed now ");
            logMsg.append(messageFromClient);
            mylogger.log(logMsg);
            
            pico_message picoMessage(messageFromClient);
            
            logMsg.clear();
            logMsg.append("request_processor : this is the message was created ");
            logMsg.append(picoMessage.toString());
            mylogger.log(logMsg);
            
            cout << "session: processing request from client request: "
            << messageFromClient << std::endl;
            
            string str("message want processed");
            
            if (picoMessage.command.compare(insertCommand) == 0) {
                cout << "inserting one record per client request";
                str = insertOneMessage(picoMessage);
            }
            else if (picoMessage.command.compare(deleteCommand) == 0) {
                cout << "deleting one record per client request";
                string str = deleteRecords(picoMessage);
                
            } else if (picoMessage.command.compare(updateCommand) == 0) {
                cout << "updating one record per client request";
                string str = updateRecords(picoMessage);
            } else if (picoMessage.command.compare(findCommand) == 0) {
                cout << "finding records per client request";
                //	string str = findRecords(picoMessage);
            }
            else if (picoMessage.command.compare(addUserToDBCommand) == 0) {
                cout << "adding user per client request";
                //	string str = addUser(picoMessage);
                //return str;
            } else if (picoMessage.command.compare(deleteUserToDBCommand) == 0) {
                cout << "deleting user per client request";
                //	string str = deleteUser(picoMessage);
                //	return str;
            }
            
            pico_message retMsg = pico_message::build_message_from_string(str);
            return retMsg;
        }
        string insertOneMessage(pico_message picoMsg)
        {
            
            int i=0;
            pico_collection optionCollection(picoMsg.collection);
            
            pico_record firstrecord = picoMsg.recorded_message.msg_in_buffers->pop();
           
            offsetType whereToWriteThisRecord =-1;
            if (optionCollection.ifRecordExists(firstrecord))
            {
                 optionCollection.index.search(firstrecord);
                 whereToWriteThisRecord =firstrecord.offset_of_record;
                
            }
           
                
           do {
               pico_record record;
               if(i!=0)
                   record = picoMsg.recorded_message.msg_in_buffers->pop();
               else
                   record = firstrecord;
               
                std::cout<<"request_processor : record that is going to be saved is this : "<<record.toString()<<std::endl;
               if(whereToWriteThisRecord==-1)
               {
                   //this is the case that the record is unique
                optionCollection.append(record); //append the
               }
               else
               {
               //this is the case that we have the offset of the first record of this message
               //that should be replaced...
                   optionCollection.overwrite(record,whereToWriteThisRecord);
                   whereToWriteThisRecord+= pico_record::max_size;
                   
               }
                i++;
           } while(!picoMsg.recorded_message.msg_in_buffers->empty());
            string result("one message was added to database in ");
            result.append(convertToString(i));
            result.append(" seperate records");
            return result;
        }
        
        string deleteRecords(pico_message picoMsg) {
            
            pico_collection optionCollection(picoMsg.collection);

            //i am using collection pointer because, it should be passed to the
            //deleter thread , so it should be in heap
            std::shared_ptr<pico_collection> collectionPtr (new pico_collection(picoMsg.collection));
            pico_record firstrecord = picoMsg.recorded_message.msg_in_buffers->pop();
            std::cout<<"request_processor : record that is going to be deleted from this : "<<firstrecord.toString()<<std::endl;
//            optionCollection.deleteRecord(firstrecord,collectionPtr);
            collectionPtr->deleteRecord(firstrecord);
            string result("one message was deleted from database in unknown(todo)");
            result.append(" seperate records");
            return result;
        }
        string updateRecords(pico_message picoMsg) {
            pico_collection optionCollection(picoMsg.collection);
            pico_record firstrecord = picoMsg.recorded_message.msg_in_buffers->pop();
            if(optionCollection.ifRecordExists(firstrecord))
            {
                //if the record is found
                deleteRecords(picoMsg);
            }
            //if found, it will be deleted and and inserted agaain
            //if not found, it will be inserted
            insertOneMessage(picoMsg);
            
            //TODO add num to the reply
            std::string msg("records were updated");
            //			msg.append(num);
            //			msg.append("records were updated.");
            return msg;
            
        }
        

        string addUser(const std::string db,const std::string user){
            string result="not done yet";
            return result;
        }
        
        string deleteUser(const std::string db,const std::string user){
            string result="not done yet";
            return result;
        }
        
        string convertToJson(list<pico_record> all_records) {
            string result;
            
            while(!all_records.empty())
            {
                pico_record rec = all_records.front();
                //TODO add all records to a huge json array and return it as the reply
                all_records.pop_front();
            }
            return result;
            
        }
//        string findRecords(const std::string collection, pico_record record) {
//            pico_collection optionCollection(collection);
//            list<pico_record> all_records = optionCollection.find(record);
//            //TODO convert all_records to json string and return it to the client
//            //		return convertToJsonString(all_records);
//            string msg("records were found");
//            return msg;
//        }
        std::string getSuccessMessage() {
            string msg;
            msg.append("{ key : ");
            //		msg.append(" )
            
            return msg;
        }
        request_processor() :
        insertCommand("insert"), deleteCommand("delete"), updateCommand(
                                                                        "update"), findCommand("find"), addUserToDBCommand(
                                                                                                                           "adduser"), deleteUserToDBCommand("deleteuser"),mylogger(logFileName) {
            
        }
        ~request_processor() {
        }
    };
}
#endif /* REQUESTPROCESSOR_H_ */

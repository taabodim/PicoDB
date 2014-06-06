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
#include <pico/collection_manager.h>
#include <pico_logger_wrapper.h>
#include <RecordInserter.h>
#include <pico_message.h>
using namespace std;
namespace pico {
class RecordInserter;
class request_processor: public pico_logger_wrapper {
private:
    
	static std::string insertCommand;
	static std::string deleteCommand;
	static std::string updateCommand;
	static std::string findCommand;
	static std::string getCommand;
	static std::string addUserToDBCommand;
	static std::string deleteUserToDBCommand;
	static std::string deleteCollectionCommand;
	static std::string createCollectionCommand;
	RecordInserter recordInserter;
    bool shutDownNormally;
public:
    std::shared_ptr<collection_manager> collectionManager;
	static string logFileName;

	request_processor(): collectionManager(new collection_manager())
     {
         recordInserter.setCollectionManager(collectionManager);
	}

	msgPtr processRequest(msgPtr picoMessage) {

		msgPtr retMsg; //every function has to return this message

		bool messageWasProcessed = false;
		if (mylogger.isTraceEnabled()) {
			mylogger
					<< "request_processor : this is the message that is going to be processed now "
					<< "messageId : " << picoMessage->messageId
					<< " \n message content : " << picoMessage->toString();
		}
		string str("request_processr: message wasn't processed");

		if (picoMessage->command.compare(insertCommand) == 0) {
			mylogger
					<< "\nrequest_processr: inserting one record per client request";
			retMsg = recordInserter.insertOneMessage(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage->command.compare(deleteCommand) == 0) {
			mylogger
					<< "\nrequest_processr: deleting one record per client request";
			retMsg = deleteRecords(picoMessage, true); //delete asynchronously
			messageWasProcessed = true;
		} else if (picoMessage->command.compare(updateCommand) == 0) {
			mylogger
					<< "\nrequest_processr: updating one record per client request";
			retMsg = updateRecords(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage->command.compare(findCommand) == 0) {
			mylogger
					<< "\nrequest_processr: finding records per client request";
			//string str = findRecords(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage->command.compare(addUserToDBCommand) == 0) {
			mylogger << "\nrequest_processr: adding user per client request";
			//	string str = addUser(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage->command.compare(deleteUserToDBCommand) == 0) {
			mylogger << "\nrequest_processr: deleting user per client request";
			//	string str = deleteUser(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage->command.compare(getCommand) == 0) {
			mylogger
					<< "\n request_processr:  getting a record per client request\n";
			retMsg = getOneMessage(picoMessage);
			messageWasProcessed = true;

		} else if (picoMessage->command.compare(deleteCollectionCommand) == 0) {
			mylogger
					<< "\n request_processr:  deleting a collection  per client request\n";
			retMsg = deleteCollection(picoMessage);
			messageWasProcessed = true;

		}else if (picoMessage->command.compare(createCollectionCommand) == 0) {
			mylogger
					<< "\n request_processr:  creating a collection  per client request\n";
			retMsg = createCollection(picoMessage);
			messageWasProcessed = true;

		}



		if (!messageWasProcessed) {
			msgPtr retMsg= pico_message::build_message_from_string(str,
					picoMessage->messageId);

            
		}
		if (mylogger.isTraceEnabled()) {
			mylogger << "request_processr: the message with Id "
					<< picoMessage->messageId << " \n"
					<< "was processed and this is the , response Id "
					<< retMsg->messageId << " response  for it\n "
					<< retMsg->toString();
		}
		return retMsg;
	}

	msgPtr createCollection(msgPtr picoMsg) {

			string result, key;

			bool resultOfOperation = collectionManager->createCollection(picoMsg
					);
			if (resultOfOperation) {
				result.append("collection ");
				result.append(picoMsg->collection);
				result.append(" was added to database ");

				std::string key("SUCCESS");
			} else {

				result.append("collection ");
				result.append(picoMsg->collection);
				result.append(" was not created in database ");

				key.append("FAILURE");

			}

			msgPtr msg(new pico_message (key, result, picoMsg->messageId));

			return msg;
		}

	msgPtr deleteCollection(msgPtr picoMsg) {
		std::shared_ptr<pico_collection> collectionPtr =
				collectionManager->getTheCollection(picoMsg->collection);

		string result, key;

		bool resultOfOperation = collectionPtr->dropCollection(
				);
		if (resultOfOperation) {
			result.append("collection ");
			result.append(picoMsg->collection);
			result.append(" was deleted from database ");

			std::string key("SUCCESS");
		} else {

			result.append("collection ");
			result.append(picoMsg->collection);
			result.append(" was not deleted from database ");

			key.append("FAILURE");

		}

		msgPtr msg(new pico_message (key, result, picoMsg->messageId));

		return msg;
	}
	msgPtr deleteRecords(msgPtr picoMsg, bool async) {
		std::shared_ptr<pico_collection> collectionPtr =
				collectionManager->getTheCollection(picoMsg->collection);

		//i am using collection pointer because, it should be passed to the
		//deleter thread , so it should be in heap

//		pico_buffered_message<pico_record> msg_in_buffers =
//				picoMsg->getKeyValueOfMessageInRecords();
//
//		pico_record firstrecord = msg_in_buffers.pop();
//		mylogger
//				<< "\n request_processor : record that is going to be deleted from this : "
//				<< firstrecord.toString();
//
//		collectionPtr->deleteRecord(firstrecord, async);
		string result("one message was deleted from database in unknown(todo)");
		result.append(" seperate records");
		std::string key("SUCCESS");
		msgPtr msg(new pico_message (key, result, picoMsg->messageId));

		return msg;
	}
	msgPtr updateRecords(msgPtr picoMsg) {
		pico_collection optionCollection(picoMsg->collection);

		mylogger
				<< "\nrequest_processor : record that is going to be updated is this : "
				<< picoMsg->toString();

		pico_buffered_message<pico_record> msg_in_buffers ;
//        =picoMsg->getKeyValueOfMessageInRecords();

		pico_record firstrecord;
//        = msg_in_buffers.pop();
		if (optionCollection.ifRecordExists(firstrecord)) {
			//if the record is found
			deleteRecords(picoMsg, false); //delete it synchronously, wait for completion
		}
		//if found, it will be deleted and and inserted agaain
		//if not found, it will be inserted
		recordInserter.insertOneMessage(picoMsg, firstrecord.offset_of_record);

		//TODO add num to the reply
		std::string result("records were updated");
		std::string key("SUCCESS");
		msgPtr msg(new pico_message (key, result, picoMsg->messageId));

		return msg;

	}

	msgPtr addUser(const std::string db, const std::string user,
			pico_message picoMsg) {
		string result = "not done yet";
		msgPtr msg = pico_message::build_message_from_string(result,
				picoMsg.messageId);
          
		return msg;
	}

	msgPtr deleteUser(const std::string db, const std::string user,
			pico_message picoMsg) {
		string result = "not done yet";
		msgPtr msg = pico_message::build_message_from_string(result,picoMsg.messageId);
        
		return msg;
	}

	string convertToJson(list<pico_record> all_records) {
		string result;

		while (!all_records.empty()) {
//			pico_record rec = all_records.front();
			//TODO add all records to a huge json array and return it as the reply
			all_records.pop_front();
		}
		return result;

	}

	msgPtr getOneMessage(msgPtr requestMessage) {

		std::shared_ptr<pico_collection> collectionPtr =
				collectionManager->getTheCollection(requestMessage->collection);

		pico_buffered_message<pico_record> msg_in_buffers =requestMessage;

		pico_record firstrecord;
//        = msg_in_buffers.pop();
		mylogger
				<< "\n request_processor : record that is going to be fetched  from this : "
				<< firstrecord.toString() << " \n offset of record is "
				<< firstrecord.offset_of_record;

		assert(!requestMessage->messageId.empty());

        
		msgPtr msg = collectionPtr->getMessageByKey(firstrecord,
				requestMessage->messageId);
		mylogger << "\n request_processor : record that is fetched  db : "
				<< msg->toString();
        
        
       
        
		return msg;

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

	~request_processor() {
        assert(shutDownNormally);
	}
};
class RecordDeleter: public request_processor {
};
class RecordUpdater: public request_processor {
};
class RecordFetcher: public request_processor {
};

}
#endif /* REQUESTPROCESSOR_H_ */

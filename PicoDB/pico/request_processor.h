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
using namespace std;
namespace pico {
class request_processor: public pico_logger_wrapper {
private:
	collection_manager collectionManager;

	static std::string insertCommand;
	static std::string deleteCommand;
	static std::string updateCommand;
	static std::string findCommand;
	static std::string getCommand;
	static std::string addUserToDBCommand;
	static std::string deleteUserToDBCommand;

public:
	static string logFileName;

	request_processor() {

	}

	pico_message processRequest(pico_message picoMessage) {

		pico_message retMsg; //every function has to return this message

		bool messageWasProcessed = false;
		if (mylogger.isTraceEnabled()) {
			mylogger
					<< "request_processor : this is the message that is going to be processed now "
					<< "messageId : " << picoMessage.messageId
					<< " \n message content : " << picoMessage.toString();
		}
		string str("request_processr: message wasn't processed");

		if (picoMessage.command.compare(insertCommand) == 0) {
			mylogger
					<< "\nrequest_processr: inserting one record per client request";
			retMsg = insertOneMessage(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage.command.compare(deleteCommand) == 0) {
			mylogger
					<< "\nrequest_processr: deleting one record per client request";
			retMsg = deleteRecords(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage.command.compare(updateCommand) == 0) {
			mylogger
					<< "\nrequest_processr: updating one record per client request";
			retMsg = updateRecords(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage.command.compare(findCommand) == 0) {
			mylogger
					<< "\nrequest_processr: finding records per client request";
			//string str = findRecords(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage.command.compare(addUserToDBCommand) == 0) {
			mylogger << "\nrequest_processr: adding user per client request";
			//	string str = addUser(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage.command.compare(deleteUserToDBCommand) == 0) {
			mylogger << "\nrequest_processr: deleting user per client request";
			//	string str = deleteUser(picoMessage);
			messageWasProcessed = true;
		} else if (picoMessage.command.compare(getCommand) == 0) {
			mylogger
					<< "\n request_processr:  getting a record per client request\n";
			retMsg = getOneMessage(picoMessage);
			messageWasProcessed = true;

		}

		if (!messageWasProcessed) {
			retMsg = pico_message::build_message_from_string(str,
					picoMessage.messageId);

		}
		if (mylogger.isTraceEnabled()) {
			mylogger << "request_processr: the message with Id "
					<< picoMessage.messageId << " \n"
					<< "was processed and this is the , response Id "
					<< retMsg.messageId << " response  for it\n "
					<< retMsg.toString();
		}
		return retMsg;
	}
	pico_message insertOneMessage(pico_message picoMsg) {

		int i = 0;
		std::shared_ptr<pico_collection> optionCollection =
				collectionManager.getTheCollection(picoMsg.collection);

		pico_buffered_message<pico_record> msg_in_buffers =
				picoMsg.getKeyValueOfMessageInRecords();

		pico_record firstrecord = msg_in_buffers.pop();

		offsetType whereToWriteThisRecord = -1;
		if (collectionManager.getTheCollection(picoMsg.collection)->ifRecordExists(
				firstrecord)) {

			if (mylogger.isTraceEnabled()) {
				mylogger
						<< "request_processr: record already exists,going to overwrite it ";
			}

			optionCollection->index.search(firstrecord);
			whereToWriteThisRecord = firstrecord.offset_of_record;

		}

		
        do {
			
			pico_record record;
			if (i != 0) {
				record = msg_in_buffers.pop();
			} else {
				record = firstrecord;
			}
			pico_record::removeTheAppendMarkerNoPtr(record);
			mylogger
					<< "\nrequest_processor : record that is going to be saved is this : "
					<< record.toString();
			if (whereToWriteThisRecord == -1) {
				//this is the case that the record is unique

				optionCollection->append(record); //append the
			} else {
				//this is the case that we have the offset of the first record of this message
				//that should be replaced...
				optionCollection->overwrite(record, whereToWriteThisRecord);
				whereToWriteThisRecord += pico_record::max_size;

			}
			i++;
		} while (!msg_in_buffers.empty());
		string result("one message was added to database in ");
		result.append(convertToString(i));
		result.append(" seperate records");

		pico_message msg = pico_message::build_message_from_string(result,
				picoMsg.messageId);
		return msg;
	}

	pico_message deleteRecords(pico_message picoMsg) {
		std::shared_ptr<pico_collection> collectionPtr =
				collectionManager.getTheCollection(picoMsg.collection);

		//i am using collection pointer because, it should be passed to the
		//deleter thread , so it should be in heap

		pico_buffered_message<pico_record> msg_in_buffers =
						picoMsg.getKeyValueOfMessageInRecords();

		pico_record firstrecord = msg_in_buffers.pop();
		mylogger
				<< "\n request_processor : record that is going to be deleted from this : "
				<< firstrecord.toString();
//            optionCollection.deleteRecord(firstrecord,collectionPtr);
		collectionPtr->deleteRecord(firstrecord);
		string result("one message was deleted from database in unknown(todo)");
		result.append(" seperate records");
		pico_message msg = pico_message::build_message_from_string(result,
				picoMsg.messageId);

		return msg;
	}
	pico_message updateRecords(pico_message picoMsg) {
		pico_collection optionCollection(picoMsg.collection);

		pico_buffered_message<pico_record> msg_in_buffers =
						picoMsg.getKeyValueOfMessageInRecords();

		pico_record firstrecord = msg_in_buffers.pop();
		if (optionCollection.ifRecordExists(firstrecord)) {
			//if the record is found
			deleteRecords(picoMsg);
		}
		//if found, it will be deleted and and inserted agaain
		//if not found, it will be inserted
		insertOneMessage(picoMsg);

		//TODO add num to the reply
		std::string result("records were updated");
		//			msg.append(num);
		//			msg.append("records were updated.");
		pico_message msg = pico_message::build_message_from_string(result,
				picoMsg.messageId);

		return msg;

	}

	pico_message addUser(const std::string db, const std::string user,
			pico_message picoMsg) {
		string result = "not done yet";
		pico_message msg = pico_message::build_message_from_string(result,
				picoMsg.messageId);
		return msg;
	}

	pico_message deleteUser(const std::string db, const std::string user,
			pico_message picoMsg) {
		string result = "not done yet";
		pico_message msg = pico_message::build_message_from_string(result,
				picoMsg.messageId);
		return msg;
	}

	string convertToJson(list<pico_record> all_records) {
		string result;

		while (!all_records.empty()) {
			pico_record rec = all_records.front();
			//TODO add all records to a huge json array and return it as the reply
			all_records.pop_front();
		}
		return result;

	}

	pico_message getOneMessage(pico_message requestMessage) {

		std::shared_ptr<pico_collection> collectionPtr =
				collectionManager.getTheCollection(requestMessage.collection);

		pico_buffered_message<pico_record> msg_in_buffers =
				requestMessage.getKeyValueOfMessageInRecords();

		pico_record firstrecord = msg_in_buffers.pop();
		mylogger
				<< "\n request_processor : record that is going to be fetched  from this : "
				<< firstrecord.toString() << " \n offset of record is "
				<< firstrecord.offset_of_record;

		pico_message responseMsg = collectionPtr->getMessageByKey(firstrecord,
				requestMessage.messageId);
		mylogger << "\n request_processor : record that is fetched  db : "
				<< responseMsg.toString();

		return responseMsg;

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
	}
};
}
#endif /* REQUESTPROCESSOR_H_ */

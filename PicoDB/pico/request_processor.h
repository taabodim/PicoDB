/*
 * requestProcessor.h
 *
 *  Created on: Mar 21, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef REQUESTPROCESSOR_H_
#define REQUESTPROCESSOR_H_
#include <pico/pico_message.h>
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

public:

	string processRequest(string messageFromClient) {
		string message("unknown");
		pico_message picoMessage(messageFromClient);
		cout << "session: processing request from client request: "
				<< messageFromClient << std::endl;
		if (picoMessage.command.compare(insertCommand) == 0) {
			cout << "inserting one record per client request";
			string str = insertOneRecord(picoMessage.collection,
					picoMessage.data);
			return str;
		} else if (picoMessage.command.compare(deleteCommand) == 0) {
			cout << "deleting one record per client request";
			string str = deleteRecords(picoMessage.collection,
					picoMessage.data);
			return str;
		} else if (picoMessage.command.compare(updateCommand) == 0) {
			cout << "updating one record per client request";
			string str = updateRecords(picoMessage.collection,
					picoMessage.data);
			return str;
		} else if (picoMessage.command.compare(findCommand) == 0) {
			cout << "finding records per client request";
			string str = findRecords(picoMessage.collection, picoMessage.data);
			return str;
		}
//		else if (picoMessage.command.compare(addUserToDBCommand) == 0) {
//			cout << "adding user per client request";
//			string str = addUser(picoMessage.db,
//					picoMessage.user);
//			return str;
//		} else if (picoMessage.command.compare(deleteUserToDBCommand) == 0) {
//			cout << "deleting user per client request";
//			string str = deleteUser(picoMessage.db,
//					picoMessage.user);
//			return str;
//		}

		return message;
	}
	string insertOneRecord(std::string& collection, pico_record record) {
		pico_collection optionCollection(collection);
		optionCollection.insert(record);
		return record.getString();
	}
	string deleteRecords(std::string& collection, pico_record record) {
		pico_collection optionCollection(collection);
		optionCollection.deleteRecord(record);
		std::string msg("record was deleted");
		return msg;
	}
	string updateRecords(std::string& collection, pico_record record) {
		pico_collection optionCollection(collection);
		size_t num = optionCollection.update(record);
		//TODO add num to the reply
		std::string msg("records were updated");
//			msg.append(num);
//			msg.append("records were updated.");
		return msg;

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
	string findRecords(std::string& collection, pico_record record) {
		pico_collection optionCollection(collection);
		list<pico_record> all_records = optionCollection.find(record);
		//TODO convert all_records to json string and return it to the client
//		return convertToJsonString(all_records);
		string msg("records were found");
		return msg;
	}
	std::string getSuccessMessage() {
		string msg;
		msg.append("{ key : ");
//		msg.append(" )

		return msg;
	}
	request_processor() :
			insertCommand("insert"), deleteCommand("delete"), updateCommand(
					"update"), findCommand("find"), addUserToDBCommand(
					"adduser"), deleteUserToDBCommand("deleteuser") {

	}
	~request_processor() {
	}
};
}
#endif /* REQUESTPROCESSOR_H_ */

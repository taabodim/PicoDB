/*
 * ClientHandler.h
 *
 *  Created on: Mar 11, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef CLIENTHANDLER_H_
#define CLIENTHANDLER_H_
#include "pico/pico_server.h"
#include "pico/pico_record.h"
#include "pico/pico_collection.h"
#include "pico/asyncWriter.h"
#include "pico/asyncReader.h"
#include "pico/request_processor.h"
using boost::asio::ip::tcp;
using namespace std;
namespace pico {

class pico_session: public std::enable_shared_from_this<pico_session> {
private:
	socketType socket_;

public:
	pico_session(socketType r_socket) :
			writeMessageLock(sessionMutex) {
		socket_ = r_socket;

//		messageToClientQueue_.push_front(hiMsg);

	}
	void start() {

		try {
			cout << " session started already..\n";
			read_messages();
		} catch (const std::exception& e) {
			cout << " exception : " << e.what() << endl;
		} catch (...) {
			cout << "<----->unknown exception thrown.<------>";
		}
	}
	void readWriteSync() {

		string msgFromClient = read_messages_sync();
		if (msgFromClient.compare("insert") == 0) {
			cout << "server recienved insert message from client" << endl;
			//insertData to client

			std::cout << "server reading message : " << msgFromClient
					<< std::endl;
			write_messages_sync();
			boost::this_thread::sleep(boost::posix_time::seconds(4));
		}
	}

	void write_messages_sync() {

	}
	string read_messages_sync() {

//		char* data = buffer.getData();
//		std::size_t dataSize = buffer.getSize();
//
//		std::cout << "data to send is : " << data << std::endl;
//		std::cout << "dataSize to send is : " << dataSize << std::endl;
//
//		boost::array<char, 128> buf;
//		boost::system::error_code error;
//
//		size_t len = socket_->read_some(boost::asio::buffer(buf), error);
//		return buffer.getString();
		string empty;
		return empty;
	}

	void processCommand() {
//		std::cout << "processCommand :  " << readBuffer.getString() << "\n ";
//
//		if (readBuffer.getString().compare("insert") == 0) {
//			cout << "server recieved insert message from client" << endl;
//			//insertData to client
//			write_messages();
//		}
//		write_messages();
	}


	void readingAndWritingRecordData() {

		string key1 = "keyfromkey1";
		string value1 = "valuefromvalue1";
		string key2 = "key2";
		string value2 = "value2";
		pico_record x1(key1, value1);

		pico_record x2(key2, value2);
		pico_collection optionCollection("options-collection");

		for (int i = 0; i < 10; i++) {
			optionCollection.insert(x1);
		}
		cout << " number of records are : "
				<< optionCollection.getNumberOfRecords() << " \n";
		cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
		cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
		optionCollection.update(x1, x2);
		//optionCollection.deleteRecord(x1);

		std::cout << "end of function readingAndWritingRecordData() "
				<< std::endl;

	}
	void processDataFromClient(std::string messageFromClient) {

		try {

			string reply = requestProcessor_.processRequest(messageFromClient);

			messageToClientQueue_.push_front(reply);
			messageClientQueueIsEmpty.notify_all();

			write_messages();

		} catch (std::exception &e) {
			cout << " this is the error : " << e.what() << endl;
		}

	}
	void read_messages() {

		auto self(shared_from_this());
		cout << "session is trying to read messages" << endl;
		bufferPtrType currentBuffer = asyncReader_.getReadBuffer();

		boost::asio::async_read(*socket_,
				boost::asio::buffer(currentBuffer->getData(), pico_buffer::max_size),
				[this,self,currentBuffer](const boost::system::error_code& error,
						std::size_t t ) {

					string str =currentBuffer->getString();

					if(error) cout<<" error msg : "<<error.message()<<std::endl;
					std::cout << "Server received "<<std::endl;
					cout<<t<<" bytes read from Client "<<std::endl;
					std::cout<< " data read from client is "<<str<<std::endl;
					std::cout << "-------------------------"<<std::endl;

					processDataFromClient(str);

				});
	}
	void write_messages() {

		if (messageToClientQueue_.empty())
			messageClientQueueIsEmpty.wait(writeMessageLock);

		cout << " session : messageToClientQueue_ size is "<< messageToClientQueue_.size() << endl;
		string messageToClient = messageToClientQueue_.front();
		messageToClientQueue_.pop_front();
		bufferPtrType currentBuffer = asyncWriter_.getWriteBuffer();
		currentBuffer->setData(messageToClient);
		auto self(shared_from_this());
		boost::asio::async_write(*socket_,
				boost::asio::buffer(currentBuffer->getData(), currentBuffer->getSize()),
				[this,self,currentBuffer](const boost::system::error_code& error,
						std::size_t t) {

					string str = currentBuffer->getString();
					std::cout << "Server sent "<<std::endl;
					cout<<t<<" bytes sent to client "<<std::endl;
					if(error) cout<<" error msg : "<<error.message()<<std::endl;
					std::cout<< " data sent to client is "<<str<<std::endl;
					std::cout << "-------------------------"<<std::endl;
					read_messages();
				});

	}

	asyncWriter asyncWriter_;
	asyncReader asyncReader_;
	request_processor requestProcessor_;
	list<std::string> messageToClientQueue_;
	boost::mutex sessionMutex;   // mutex for the condition variable
	boost::condition_variable messageClientQueueIsEmpty;
	boost::unique_lock<boost::mutex> writeMessageLock;
};
}

#endif /* CLIENTHANDLER_H_ */
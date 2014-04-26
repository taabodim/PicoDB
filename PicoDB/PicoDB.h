/*
 * Sample.h
 *
 *  Created on: Mar 3, 2014
 *      Author: mahmoudtaabodi
 */

#ifndef FEL_FEL_DB_H
#define FEL_FEL_DB_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

using namespace std;
enum {
	max_length = 1024
};

namespace PicoDB {
class PicoDBEngine {
private:

	PicoDBEngine(bool serverStatus) {
		serverShutDown = serverStatus;
	}
	PicoDBEngine operator=(PicoDBEngine& engine) {
		return engine;
	}
	PicoDBEngine engine { false };
public:
	boost::asio::io_service io_service;
	bool serverShutDown;

	PicoDBEngine getInstance() {
		return engine;
	}
	void runService() {
//		cout << "data received from client " << stream.rdbuf() << endl;
//
//		endpoint endpoint(v4(), 8877);
//		acceptor acceptor(io_service, endpoint);
//		socket socket(io_service);
//		acceptor.accept(socket);
//		while (serverShutdown == false) {
//			iostream stream;
//			acceptor.accept(*stream.rdbuf());
//			mylogger << "data received from client " << stream.rdbuf()
//					//<< std::endl;
//			if (!stream) {
//				mylogger << "Error: " << stream.error().message() << "\n";
//			}
//		}
	}
	virtual ~PicoDBEngine();
};
class PicoConnection {
};
class ConnectionHandler {

public:
	ConnectionHandler(PicoConnection picConneciton) {
	}
	virtual ~ConnectionHandler() {
	}
};

class PicoData {

public:
	std::string key;
	std::string value;

	PicoData();
	virtual ~PicoData();
};
class PicoDriver {
public:
	PicoDriver();
	void insert(PicoData data);
	virtual ~PicoDriver();
};

class PicoClient {
public:
	PicoClient() {
	}
	void connect(string host, string port) {

	}
	void insert(PicoData data) {

	}
	virtual ~PicoClient() {
	}
};
void PicoTest() {
	PicoData data;

	data.key = "first key";
	data.value = "first value";
	PicoClient client;
	string host = "localhost";
	string port = "8877";
	client.connect(host, port);

	//	client.createDB("FirstDB");
	//	client.createCollection("FirstCollection");
	//	client.insert(dbName,collectionName,data);
	client.insert(data);

}

void runClient() {
//try
//{
//    if (argc != 3)
//    {
//      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
//      return 1;
//    }
//
//    boost::asio::io_service io_service;
//
//    tcp::socket s(io_service);
//    tcp::resolver resolver(io_service);
//    boost::asio::connect(s, resolver.resolve({argv[1], argv[2]}));
//
//    mylogger << "Enter message: ";
//    char request[max_length];
////    std::cin.getline(request, max_length);
//    string message = " hi, this is the message";
//    request[max_length] = message.c_str();
//
//    size_t request_length = std::strlen(request);
//    boost::asio::write(s, boost::asio::buffer(request, request_length));
//
//    char reply[max_length];
//    size_t reply_length = boost::asio::read(s,
//        boost::asio::buffer(reply, request_length));
//    mylogger << "Reply is: ";
//    mylogger.write(reply, reply_length);
//    mylogger << "\n";
//  }
//  catch (std::exception& e)
//  {
//    std::cerr << "Exception: " << e.what() << "\n";
//  }

}
}

#endif /* FEL_FEL_DB_H */

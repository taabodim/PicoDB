//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PICO_CLIENT_H
#define PICO_CLIENT_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <array>
#include "AsyncTcpClient.h"
#include <boost/thread.hpp>
#include <pico/pico_buffer.h>
#include <pico/asyncWriter.h>
#include <pico/asyncReader.h>
#include <pico/pico_message.h>
#include <pico/pico_utils.h>
#include <pico/pico_concurrent_list.h>
#include <pico/pico_buffered_message.h>
using boost::asio::ip::tcp;
using namespace std;


namespace pico {
    class DBClient;
 typedef DBClient clientType;
    typedef std::shared_ptr<pico_message> queueType;
class DBClient: public std::enable_shared_from_this<DBClient> {
private:
   
	
	socketType socket_;
	pico_concurrent_list <queueType> commandQueue_;
public:
	DBClient(socketType socket) :
			writeMessageLock(sessionMutex) {

		socket_ = socket;
		std::cout << "client initializing " << std::endl;

	}
    
	void start_connect(tcp::resolver::iterator endpoint_iter) {
		std::cout << " start_connect(tcp::resolver::iterator endpoint_iter) "
				<< std::endl;
		if (endpoint_iter != tcp::resolver::iterator()) {
			std::cout << "Trying " << endpoint_iter->endpoint() << "...\n";

			// Start the asynchronous connect operation.
			socket_->async_connect(endpoint_iter->endpoint(),
					boost::bind(&DBClient::start, this, _1, endpoint_iter));
		} else {
			// There are no more endpoints to try. Shut down the client.
//				stop();
		}
		std::cout << " start_connect ends" << std::endl;
	}
	void handle_connect(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
//			if (stopped_)
//				return;

// The async_connect() function automatically opens the socket at the start
// of the asynchronous operation. If the socket is closed at this time then
// the timeout handler must have run first.
		if (!socket_->is_open()) {
			std::cout << "Connect timed out\n";

			// Try the next available endpoint.
			start_connect(++endpoint_iter);
		}
	}

	void start(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
        cout<<"client starting the process..going to write_message to server"<<endl;
        
		try {
			write_messages();
//			read_messages();

		} catch (const std::exception& e) {
			cout << " exception : " << e.what() << endl;
		} catch (...) {
			cout << "<----->unknown exception thrown.<------>";
		}
	}
//	void readAsync() {
////		while (true) {
//		cout<<" client is going to read asynchronously..\n";
//		read_messages();
//
//		//boost::this_thread::sleep(boost::posix_time::seconds(4));
//
//	}

	void processDataFromServer(string messageFromServer) {

		write_messages();
	}
	void readSynchronously() {
		for (;;) {
			boost::array<char, 128> buf;
			boost::system::error_code error;

			size_t len = socket_->read_some(boost::asio::buffer(buf), error);

			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.

			std::cout << "client got this " << buf.data() << std::endl;
		}
	}
	void read_messages() {

        auto self(shared_from_this());
		cout << "client is trying to read messages" << endl;
		bufPtr currentBuffer = asyncReader_.getOneBuffer();
		//get a message in pico_buffer
        //convert it to string , if at the end of it is append.. add it to the list of buffers and create a
		//buffered_message out of all the pico_buffers and then convert the buffere_message to string and process
        
		//if the append is not at the end..convert the pico_buffer to string and process
		//
		boost::asio::async_read(*socket_,
                                boost::asio::buffer(currentBuffer->getData(),
                                                    pico_buffer::max_size),
                                [this,self,currentBuffer](const boost::system::error_code& error,
                                                          std::size_t t ) {
                                    string app("append");
                                    string str =currentBuffer->getString();
                                    append_to_last_message(currentBuffer);
                                    if(str.find_last_of(app)== string::npos)
                                    {
                                        cout<<"this buffer is an add on to the last message..dont process anything..read the next buffer"<<endl;
                                        
                                    }
                                    else {
                                        cout<<"message was read completely..process the last message "<<endl;
                                        str =last_read_message->toString();
                                        print(error,t,str);
                                        
                                        
                                        processDataFromServer(str);
                                        last_read_message->clear();
                                        
                                    }
                                    
                                });

	}
    void print(const boost::system::error_code& error,
               std::size_t t,string& str)
    {
        std::cout << "Client Received :  "<<std::endl;
        cout<<t<<" bytes from server "<<std::endl;
        if(error) cout<<" error msg : "<<error.message()<<std::endl;
        std::cout<< " data  read from server is "<<str<<std::endl;
        std::cout << "-------------------------"<<std::endl;
    }
    void append_to_last_message(bufPtr currentBuffer) {
		last_read_message->append(*currentBuffer);
        
	}
    void insert(std::string& key,std::string& value){
      
        //std::string key,std::string value,std::string com,std::string database,std::string us
        //,std::string col
       // cout<<"insert : key : "+key +" \n value : "+value<<endl;
        string command("insert");
        string database("currencyDB");
        string user("currencyUser");
        string col("currencyCollection");
       
        queueType msg (  new pico_message(key,value,command,database,user,col) );
        queueCommand(msg);
        
    }
	void write_messages() {
        
		queueType message;
		if (commandQueue_.empty())
        {
            cout<<"client queue of messages is empty...going to wait on the lock"<<endl;
			messageClientQueueIsEmpty.wait(writeMessageLock);
        }
		cout << "client is writing async now\n";

		if (!commandQueue_.empty()) {
			message = commandQueue_.pop();

		}
		cout << " client is going to send this message to server : " << message->toString()
				<< std::endl;

      

        msgPtr buffered_msg = message->convert_to_buffered_message();
        
	
        while(!buffered_msg->msg_in_buffers->empty())
        {
            auto curBuf = buffered_msg->msg_in_buffers->pop();
            bufPtr curBufPtr (new pico_buffer(curBuf));
            writeOneBuffer(curBufPtr);
          
      	}
    }
    void writeOneBuffer(bufPtr currentBuffer)
    {
        
        char* data = currentBuffer->getData();
		std::size_t dataSize = currentBuffer->getSize();
		auto self(shared_from_this());
        
		boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
                                 [this,self,currentBuffer](const boost::system::error_code& error,
                                                           std::size_t t) {
                                     string str = currentBuffer->getString();
                                     std::cout << "Client Sent :  "<<std::endl;
                                     cout<<t<<" bytes from server "<<std::endl;
                                     if(error) cout<<" error msg : "<<error.message()<<std::endl;
                                     std::cout<< " data sent to server is "<<str<<std::endl;
                                     std::cout << "-------------------------"<<std::endl;
                                     read_messages();
                                 });
 
    }
	void queueCommand(queueType queueValue) {
		
        commandQueue_.push(queueValue);
		messageClientQueueIsEmpty.notify_all();
	}
//	pico_client_server::pico_buffer buffer;

	asyncWriter asyncWriter_;
	asyncReader asyncReader_;
	boost::mutex sessionMutex;   // mutex for the condition variable
	boost::condition_variable messageClientQueueIsEmpty;
	boost::unique_lock<boost::mutex> writeMessageLock;
    	msgPtr last_read_message;

};


void startTheShell(std::shared_ptr<clientType> ptr) {
	std::string quitCmd("quit");

	cout << "Wlecome to the Pico Shell  " << endl;
	std::string shellCommand; // declare one variable
	do {

		cin >> shellCommand;
		try{
		queueType msg ( new pico_message(shellCommand));
		ptr->queueCommand(msg);
		}catch(...)
		{
			cout<<"error in parsing command"<<std::endl;
		}

	} while (shellCommand.compare(quitCmd) != 0);
}
    

    


} //end of namespace
#endif

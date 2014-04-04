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
#include <logger.h>
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
    logger mylogger;
	DBClient(socketType socket) :
			writeMessageLock(sessionMutex) {

		socket_ = socket;
		mylogger.log( "client initializing ");

	}
    
	void start_connect(tcp::resolver::iterator endpoint_iter) {
		mylogger.log( " start_connect(tcp::resolver::iterator endpoint_iter) ");
		if (endpoint_iter != tcp::resolver::iterator()) {
			mylogger.log(  "Trying ");
            //mylogger.log(endpoint_iter->endpoint() << "...\n";

			// Start the asynchronous connect operation.
			socket_->async_connect(endpoint_iter->endpoint(),
					boost::bind(&DBClient::start, this, _1, endpoint_iter));
		} else {
			// There are no more endpoints to try. Shut down the client.
//				stop();
		}
		mylogger.log(  " start_connect ends" );
	}
	void handle_connect(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
//			if (stopped_)
//				return;

// The async_connect() function automatically opens the socket at the start
// of the asynchronous operation. If the socket is closed at this time then
// the timeout handler must have run first.
		if (!socket_->is_open()) {
			mylogger.log(  "Connect timed out\n");

			// Try the next available endpoint.
			start_connect(++endpoint_iter);
		}
	}

	void start(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
        mylogger.log("client starting the process..going to write_message to server");
        
		try {
            for(int  i=0;i<1;i++)
                insert();
            
			write_messages();
//			read_messages();

		} catch (const std::exception& e) {
			mylogger.log(" exception : ",e.what());
		} catch (...) {
			mylogger.log( "<----->unknown exception thrown.<------>");
		}
	}
//	void readAsync() {
////		while (true) {
//		mylogger.log(" client is going to read asynchronously..\n";
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
		mylogger.log( "client is trying to read messages" );
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
                                        mylogger.log("this buffer is an add on to the last message..dont process anything..read the next buffer");
                                        
                                    }
                                    else {
                                        mylogger.log("message was read completely..process the last message ");
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
       mylogger.log("Client Received :  ");
//        mylogger.log(t<<" bytes from server "<<std::endl;
        if(error) mylogger.log(" error msg : ",error.message());
                      mylogger.log( " data  read from server is ");
                      mylogger.log(str);
        mylogger.log("-------------------------");
    }
    void append_to_last_message(bufPtr currentBuffer) {
		last_read_message->append(*currentBuffer);
        
	}
//    void insert(const std::string& key,const std::string& value){
        void insert(){
            std::string key;
            std::string value;
      key="They've been spotted and spotted again";
        value="They've been spotted and spotted again, those objects in the southern Indian Ocean. Every time a report comes out that something has been seen that may be related to missing Malaysia Flight 370, hopes have risen. And then, they have fallen. It's seemed like a daily exercise.showed about 300 objects ranging in size from 6 feet (2 meters) to 50 feet (15 meters). When photographed Monday, they were about 125 miles (201 kilometers) away from the spot ";
        //std::string key,std::string value,std::string com,std::string database,std::string us
        //,std::string col
       // mylogger.log("insert : key : "+key +" \n value : "+value<<endl;
        string command("insert");
        string database("currencyDB");
        string user("currencyUser");
        string col("currencyCollection");
        
        queueType msg (  new pico_message(key,value,command,database,user,col) );
        queueCommand(msg);
        
    }
	void write_messages() {
        
		
		if (commandQueue_.empty())
        {
            mylogger.log("client queue of messages is empty...going to wait on the lock");
			messageClientQueueIsEmpty.wait(writeMessageLock);
        }
		mylogger.log( "client is writing async now");

		if (!commandQueue_.empty()) {
			queueType message = commandQueue_.pop();

		
		 mylogger.log( " client is going to send this message to server : ");
         mylogger.log(message->db);
         mylogger.log(message->command);
         mylogger.log(message->collection);
         mylogger.log(message->user);
         mylogger.log(message->raw_message); //this line throws exception , check why

        msgPtr buffered_msg = message->convert_to_buffered_message();
        
	
        while(!buffered_msg->msg_in_buffers->empty())
        {
            mylogger.log("pico_client : popping current Buffer ");
            auto curBuf = buffered_msg->msg_in_buffers->pop();
            bufPtr curBufPtr (new pico_buffer(curBuf));
            writeOneBuffer(curBufPtr);
        
          
      	}
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
                                      mylogger.log( "Client Sent :  ");
                                     //mylogger.log(t" bytes from server "<<std::endl;
                                     if(error) mylogger.log(" error msg : ",error.message());
                                     mylogger.log( " data sent to server is ");
                                     mylogger.log(str);
                                     mylogger.log("-------------------------");
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
			cout<<("error in parsing command");
		}

	} while (shellCommand.compare(quitCmd) != 0);
}
    

    


} //end of namespace
#endif

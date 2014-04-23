//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef PICO_SERVER_H
#define PICO_SERVER_H

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <array>
#include "pico_client.h"
#include <boost/thread.hpp>
#include <pico/pico_buffer.h>
#include <pico/pico_session.h>
#include <logger.h>
using boost::asio::ip::tcp;
using namespace std;

namespace pico {
typedef std::shared_ptr<tcp::socket> socketType;
typedef tcp::acceptor acceptorType;

typedef std::string messageType;
class pico_server {

public:

	acceptorType acceptor_;
	socketType socket;
    //logger mylogger;
	pico_server(boost::asio::io_service& io_service, const tcp::endpoint& endpoint,
			socketType mySocket) :
			acceptor_(io_service, endpoint), socket(mySocket) {
		std::cout << "server initializing " << std::endl;

        acceptConnection();
	}

	void acceptConnection() {
      //  std::cout << "server before waiting for connections " << std::endl;
        
		acceptor_.async_accept(*socket, [this](boost::system::error_code ec)
		{
           
			if (!ec)
			{
               // std::cout<<"server accepted a conneciton"<<endl;
				initClientHandler(socket);
			}
            else{
              //  std::cout<<"server accepted a conneciton but there was some error\n";
              //  std::cout<< "error code is"<<ec.value()<<" "<<ec.message()<<endl;
                }
            
            //THIS LINE SHOULD BE HERE !!!!
            acceptConnection();
 		});
        
    
	}

	void initClientHandler(socketType socket) {
		std::cout<<"server accepted a connection...going to start the session";
		std::shared_ptr<pico_session> clientPtr (new pico_session (socket));
		//add clients to a set
		clientPtr->start();
	}

};

void runServer() {
	std::string port { "8877" };

	boost::asio::io_service io_service;


	std::list<std::shared_ptr<pico_server>> servers;

	tcp::endpoint endpoint(tcp::v4(), std::atoi(port.c_str()));

	socketType socket(new tcp::socket(io_service));

	std::shared_ptr<pico_server> serverPtr (new pico_server (io_service, endpoint, socket));
	servers.push_back(serverPtr);
    io_service.run();
	
    cout<<" io server is running going out of scope..\n";
}
} //end of namespace

#endif




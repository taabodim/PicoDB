//
// async_tcp_client.cpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//


#ifndef ASYNC_TCP_CLIENT_H
#define ASYNC_TCP_CLIENT_H

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <iostream>



using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

//
// This class manages socket timeouts by applying the concept of a deadline.
// Some asynchronous operations are given deadlines by which they must complete.
// Deadlines are enforced by an "actor" that persists for the lifetime of the
// client object:
//
//  +----------------+
//  |                |
//  | check_deadline |<---+
//  |                |    |
//  +----------------+    | async_wait()
//              |         |
//              +---------+
//
// If the deadline actor determines that the deadline has expired, the socket
// is closed and any outstanding operations are consequently cancelled.
//
// Connection establishment involves trying each endpoint in turn until a
// connection is successful, or the available endpoints are exhausted. If the
// deadline actor closes the socket, the connect actor is woken up and moves to
// the next endpoint.
//
//  +---------------+
//  |               |
//  | start_connect |<---+
//  |               |    |
//  +---------------+    |
//           |           |
//  async_-  |    +----------------+
// connect() |    |                |
//           +--->| handle_connect |
//                |                |
//                +----------------+
//                          :
// Once a connection is     :
// made, the connect        :
// actor forks in two -     :
//                          :
// an actor for reading     :       and an actor for
// inbound messages:        :       sending heartbeats:
//                          :
//  +------------+          :          +-------------+
//  |            |<- - - - -+- - - - ->|             |
//  | start_read |                     | start_write |<---+
//  |            |<---+                |             |    |
//  +------------+    |                +-------------+    | async_wait()
//          |         |                        |          |
//  async_- |    +-------------+       async_- |    +--------------+
//   read_- |    |             |       write() |    |              |
//  until() +--->| handle_read |               +--->| handle_write |
//               |             |                    |              |
//               +-------------+                    +--------------+
//
// The input actor reads messages from the socket, where messages are delimited
// by the newline character. The deadline for a complete message is 30 seconds.
//
// The heartbeat actor sends a heartbeat (a message that consists of a single
// newline character) every 10 seconds. In this example, no deadline is applied
// message sending.
//

namespace pico {
class client {
public:
	client(boost::asio::io_service& io_service) :
			stopped_(false), socket_(io_service), deadline_(io_service), heartbeat_timer_(
					io_service) {
	}

	// Called by the user of the client class to initiate the connection process.
	// The endpoint iterator will have been obtained using a tcp::resolver.
	void start(tcp::resolver::iterator endpoint_iter) {
		// Start the connect actor.

		//mylogger << " start(tcp::resolver::iterator endpoint_iter) "
				//<< std::endl;
		start_connect(endpoint_iter);

		// Start the deadline actor. You will note that we're not setting any
		// particular deadline here. Instead, the connect and input actors will
		// update the deadline prior to each asynchronous operation.
		deadline_.async_wait(boost::bind(&client::check_deadline, this));
	}

	// This function terminates all the actors to shut down the connection. It
	// may be called by the user of the client class, or by the class itself in
	// response to graceful termination or an unrecoverable error.
	void stop() {
		stopped_ = true;
		socket_.close();
		deadline_.cancel();
		heartbeat_timer_.cancel();
	}

private:
	void start_connect(tcp::resolver::iterator endpoint_iter) {
		//mylogger << " start_connect(tcp::resolver::iterator endpoint_iter) "
				//<< std::endl;
		if (endpoint_iter != tcp::resolver::iterator()) {
			//mylogger << "Trying " << endpoint_iter->endpoint() << "...\n";

			// Set a deadline for the connect operation.
			deadline_.expires_from_now(boost::posix_time::seconds(60));

			// Start the asynchronous connect operation.
			socket_.async_connect(endpoint_iter->endpoint(),
					boost::bind(&client::handle_connect, this, _1,
							endpoint_iter));
		} else {
			// There are no more endpoints to try. Shut down the client.
			stop();
		}
	}
//test sss
	void handle_connect(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
		if (stopped_)
			return;

		// The async_connect() function automatically opens the socket at the start
		// of the asynchronous operation. If the socket is closed at this time then
		// the timeout handler must have run first.
		if (!socket_.is_open()) {
		//	mylogger << "Connect timed out\n";

			// Try the next available endpoint.
			start_connect(++endpoint_iter);
		}

		// Check if the connect operation failed before the deadline expired.
		else if (ec) {
		//	mylogger << "Connect error: " << ec.message() << "\n";

			// We need to close the socket used in the previous connection attempt
			// before starting a new one.
			socket_.close();

			// Try the next available endpoint.
			start_connect(++endpoint_iter);
		}

		// Otherwise we have successfully established a connection.
		else {
			//mylogger << "Connected to " << endpoint_iter->endpoint() << "\n";

			// Start the input actor.
			start_read();

			// Start the heartbeat actor.
			start_write();
		}
	}

	void start_read() {
		// Set a deadline for the read operation.
		deadline_.expires_from_now(boost::posix_time::seconds(30));

		// Start an asynchronous operation to read a newline-delimited message.
		boost::asio::async_read_until(socket_, input_buffer_, '\n',
				boost::bind(&client::handle_read, this, _1));
	}

	void handle_read(const boost::system::error_code& ec) {
		if (stopped_)
			return;

		if (!ec) {
			// Extract the newline-delimited message from the buffer.
			std::string line;
			std::istream is(&input_buffer_);
			std::getline(is, line);

			// Empty messages are heartbeats and so ignored.
			if (!line.empty()) {
				//mylogger << "Received: " << line << "\n";
			}

			start_read();
		} else {
			//mylogger << "Error on receive: " << ec.message() << "\n";

			stop();
		}
	}

	void start_write() {
		if (stopped_)
			return;

		// Start an asynchronous operation to send a heartbeat message.
		boost::asio::async_write(socket_,
				boost::asio::buffer("aello from client...\n", 1),
				boost::bind(&client::handle_write, this, _1));
	}

	void handle_write(const boost::system::error_code& ec) {
		if (stopped_)
			return;

		if (!ec) {
			// Wait 10 seconds before sending the next heartbeat.
			heartbeat_timer_.expires_from_now(boost::posix_time::seconds(10));
			heartbeat_timer_.async_wait(
					boost::bind(&client::start_write, this));
		} else {
			//mylogger << "Error on heartbeat: " << ec.message() << "\n";

			stop();
		}
	}

	void check_deadline() {
		if (stopped_)
			return;

		// Check whether the deadline has passed. We compare the deadline against
		// the current time since a new asynchronous operation may have moved the
		// deadline before this actor had a chance to run.
		if (deadline_.expires_at() <= deadline_timer::traits_type::now()) {
			// The deadline has passed. The socket is closed so that any outstanding
			// asynchronous operations are cancelled.
			socket_.close();

			// There is no longer an active deadline. The expiry is set to positive
			// infinity so that the actor takes no action until a new deadline is set.
			deadline_.expires_at(boost::posix_time::pos_infin);
		}

		// Put the actor back to sleep.
		deadline_.async_wait(boost::bind(&client::check_deadline, this));
	}

private:
	bool stopped_;
	tcp::socket socket_;
	boost::asio::streambuf input_buffer_;
	deadline_timer deadline_;
	deadline_timer heartbeat_timer_;
};

}

#endif




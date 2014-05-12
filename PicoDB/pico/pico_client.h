#ifndef PonocoDriver_H
#define PonocoDriver_H

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
#include <chrono>
#include <pico/writer_buffer_container.h>
#include <pico/asyncReader.h>
#include <pico/pico_message.h>
#include <pico/pico_utils.h>
#include <pico/pico_concurrent_list.h>
#include <pico/pico_buffered_message.h>
#include <logger.h>
#include <pico/pico_session.h> //for the checking if appended function and sendmetherestofdata function
#include <pico/pico_test.h>
#include <pico/pico_logger_wrapper.h>
#include <ClientResponseProcessor.h>
#include <PonocoDriverHelper.h>

using boost::asio::ip::tcp;
using namespace std;
using namespace std::chrono;

namespace pico {

class PonocoDriver;
typedef PonocoDriver DriverType;
typedef std::shared_ptr<pico_message> queueType; //to avoid lots of copying and moving
class PonocoDriver: public std::enable_shared_from_this<PonocoDriver>,
		public pico_logger_wrapper
//,//public  PonocoDriverHelper
{
private:
	std::shared_ptr<tcp::socket> socket_;
	ResponseProcessor responseProcessor;
	//  typedef  std::shared_ptr<PonocoDriverHelper> helperType;
	typedef PonocoDriverHelper* helperType;
	// helperType syncHelper;
	pico_concurrent_list<queueType> responseQueue_;
	std::shared_ptr<pico_concurrent_list<std::shared_ptr<pico_record>>> bufferQueuePtr_;

	bool clientIsConnected;

	std::mutex writeOneBufferMutex;   // mutex for the condition variable
	std::mutex allowedToWriteLockMutex;
	std::mutex queueRequestMessagesMutex;
	std::mutex responseQueueMutex;
	std::mutex waitForClientToConnectMutex;

	std::condition_variable clientIsConnectedCV;
	std::condition_variable clientIsAllowedToWrite;
	std::condition_variable bufferQueueIsEmpty;
	std::condition_variable responseQueueIsEmpty;

public:
	PonocoDriver(helperType syncHelperArg )
	// syncHelper ( syncHelperArg),
	:bufferQueuePtr_(new pico_concurrent_list<std::shared_ptr<pico_record>>)
	{
		//            syncHelper = syncHelperArg;
		boost::unique_lock<std::mutex> waitForClientToConnectLock(waitForClientToConnectMutex);
		//clientIsConnectedCV.wait(waitForClientToConnectLock);
		//            while(!clientIsConnected)
		//            {
		//                //wait until client is connected
		//            }
		if(mylogger.isTraceEnabled())
		{
			mylogger<< "Ponoco Instance is initializing  ";
		}

	}
	//        PonocoDriver(std::shared_ptr<PonocoDriverHelper> syncHelperArg) {
	//
	//
	//            syncHelper = syncHelperArg;
	//            mylogger<< " Ponoco Driver is initializing ";
	//
	//        }

	void start_connect(std::shared_ptr<tcp::socket> socket,tcp::resolver::iterator endpoint_iter) {
		try {
			socket_ = socket;

			if(mylogger.isTraceEnabled())
			{
				mylogger<<" start_connect(tcp::resolver::iterator endpoint_iter) ";
			}

			if (endpoint_iter != tcp::resolver::iterator()) {

				if(mylogger.isTraceEnabled())
				{
					mylogger<< "Trying "<<endpoint_iter->endpoint() << "...\n";
				}
				// Start the asynchronous connect operation.
				socket_->async_connect(endpoint_iter->endpoint(),
						boost::bind(&PonocoDriver::start, this, _1, endpoint_iter));
			} else {
				// There are no more endpoints to try. Shut down the client.
				//				stop();
			}
			if(mylogger.isTraceEnabled())
			{
				mylogger<< " start_connect ends ...\n";
			}

		} catch(...)
		{
			if(mylogger.isTraceEnabled())
			{
				mylogger<< "client start_connect : start_connect : Exception: unknown thrown\n";
			}

			raise(SIGABRT);

		}
	}
	void handle_connect(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
		//			if (stopped_)
		//				return;

		// The async_connect() function automatically opens the socket at the start
		// of the asynchronous operation. If the socket is closed at this time then
		// the timeout handler must have run first.
		if (!socket_->is_open()) {
			mylogger<<( "Connect timed out\n");

			// Try the next available endpoint.
			start_connect(this->socket_,++endpoint_iter);
		}
	}

	void start(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
		//            mylogger<<"\nclient starting the process..going to write_message to server\n";

		try {
			if(!ec)
			{
				//clientIsConnectedCV.notify_all();
				clientIsConnected=true;
				writeOneBuffer();//this starts the writing, if bufferQueue is empty it waits for it.
			}
			else {
				if(mylogger.isTraceEnabled())
				{
					mylogger<<"\nclient : start : error is "<<ec.value()<<" error message is "<<ec.message();
					mylogger <<"\n error name is "<< ec.category().name();}
			}

		} catch (const std::exception& e) {
			mylogger<<" exception : "<<e.what();
			raise(SIGABRT);

		} catch (...) {
			mylogger<< "<----->unknown exception thrown.<------>\n";
			raise(SIGABRT);

		}
	}

	void readSynchronously() {
		for (;;) {
			boost::array<char, 128> buf;
			boost::system::error_code error;

			size_t len = socket_->read_some(boost::asio::buffer(buf), error);

			if (error == boost::asio::error::eof)
			break; // Connection closed cleanly by peer.
			else if (error)
			throw boost::system::system_error(error);// Some other error.

			mylogger << "\nclient got this " << buf.data();
		}
	}
	void readOneBuffer() {
		if(mylogger.isTraceEnabled())
		{
			mylogger<<"\nclient : going to read a buffer from server \n";

		}
		auto self(shared_from_this());
		// mylogger<<"client is trying to read one buffer\n" ;
		std::shared_ptr<pico_record> currentBuffer = asyncReader_.getOneBuffer();

		boost::asio::async_read(*socket_,
				boost::asio::buffer(currentBuffer->getDataForRead(),
						pico_record::max_size),
				[this,self,currentBuffer](const boost::system::error_code& error,
						std::size_t t ) {

					self->processTheMessageJustRead(currentBuffer,t);
				});

	}
	void writeOneBuffer()
	{

		mylogger<<"client : writeOneBuffer BEFORE getting the lock ...\n";
		std::unique_lock<std::mutex> writeOneBufferLock(writeOneBufferMutex);

		while(bufferQueuePtr_->empty())
		{

			mylogger<<"client : bufferQueue is empty..waiting ...\n";

			bufferQueueIsEmpty.wait(writeOneBufferLock);
			mylogger<<"client : bufferQueue waking up because there is some data in the queue ...\n";
		}

		mylogger<<"client : is going to send some data over ...\n";
		std::shared_ptr<pico_record> currentBuffer =bufferQueuePtr_->pop();

		char* data = currentBuffer->getDataForWrite();
		std::size_t dataSize = currentBuffer->getSize();

		auto self(shared_from_this());

		boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
				[this,self,currentBuffer](const boost::system::error_code& error,
						std::size_t t) {
					string str = currentBuffer->toString();

					if(mylogger.isTraceEnabled())
					{
						mylogger<<t<<"Client  bytes to server \n";
						if(error)
						{	mylogger<<"\n error msg : "<<error.message();}

						if(str.empty())
						{
							mylogger<<"\n ERROR  Client : is going to send empty data \n";
						}

						mylogger<<"\n Client : data sent to server is "<<str<<" end of data!";

					}

					assert(t!=0);

					if(pico_record::IsThisRecordAnAddOn(*currentBuffer)) //write to server until
					//buffer is an addon,if its not, go to reading mode to and wait for reply of
					//of the whole message
					{
						writeOneBuffer();
						if(mylogger.isTraceEnabled())
						{
							mylogger<<"\n Client :I am going to send the next incomplete buffer \n";
						}
					} else {
						mylogger<<"\n Client :I am done sending a complete message now going to wait for the reply of the message \n";
						readOneBuffer();
					}
				});

	}
	void processTheMessageJustRead(std::shared_ptr<pico_record> currentBuffer,std::size_t t) {
		//            currentBuffer->loadTheKeyValueFromData();
		string str =currentBuffer->toString();
		if(mylogger.isTraceEnabled())
		{

			mylogger<<"\n client : this is the message that client read just now  : "<<str;
		}

		if(pico_record::IsThisRecordAnAddOn(*currentBuffer))
		{
			if(mylogger.isTraceEnabled())
			{
				mylogger<<"\n Client : this buffer is an add on to the last message, messageId for this buffer is \n"
				<<currentBuffer->getMessageIdAsString()<<
				"..dont process anything..read the next buffer\n";

			}
			allBuffersReadFromTheOtherSide.append(*currentBuffer);
			readOneBuffer();					//read the next addon buffer
		}
		else {

			allBuffersReadFromTheOtherSide.append(*currentBuffer);

			pico_message util;
			std::shared_ptr<pico_message> last_read_message = util.convert_records_to_message(allBuffersReadFromTheOtherSide,currentBuffer->getMessageIdAsString(),COMPLETE_MESSAGE_AS_JSON_FORMAT_WITHOUT_BEGKEY_CONKEY);
			if(mylogger.isTraceEnabled())
			{
				mylogger<<"\n client : this is the complete message read from server \n";
				mylogger<<"\n client : last_read_message messageId is "<<last_read_message->messageId;
				mylogger<<"\n client : the content of last message read from server  "<<last_read_message->toString();
			}
			processDataFromOtherSide(last_read_message);
			allBuffersReadFromTheOtherSide.clear();
			writeOneBuffer();							//write the response
		}

	}

	bool sendmetherestofdata( string comparedTo)
	{
		bool result;
		string ignore("sendmetherestofdata");

		if(comparedTo.compare(ignore)==0)
		{
			return true;
		}
		else if (comparedTo.empty())
		{
			if(mylogger.isTraceEnabled())
			{
				mylogger<<"\n client : WARNING:  CLIENT RECIEVED EMPTY MESSAGE \n ";
			}
			result = true;
		}
		else {
			result= false;}

		return result;
	}
	void processDataFromOtherSide(queueType messageFromOtherSide) {

		try {

			if(mylogger.isTraceEnabled())
			{
				mylogger<<"\n client this is the complete message from server : "<<messageFromOtherSide->toString();
			}
			//process the data from server and queue the right message or dont

			// TODO
			responseProcessor.processResponse(messageFromOtherSide);

			queueTheResponse(messageFromOtherSide);

			if(mylogger.isTraceEnabled())
			{

				mylogger<<"\n Client : got the response with this messageId :  "<<
				messageFromOtherSide->messageId<<" and put it in ResponseQueue \n this is the full response : "<<messageFromOtherSide->toString();
				assert(messageFromOtherSide->toString().size()>0);
			}
			

		} catch (std::exception &e) {
			cout << " this is the error : " << e.what() << endl;
		}
	}
//	void tellHimSendTheRestOfData(string messageId)
//	{
//		if(mylogger.isTraceEnabled())
//		{
//			mylogger<<"\nClient is telling send the rest of data for this message Id  "<<messageId<<" \n";
//		}
//		string msg("sendmetherestofdata");
//
//		pico_message reply = pico_message::build_message_from_string(msg,messageId);
//		queueRequestMessages(reply);
//		if(mylogger.isTraceEnabled())
//		{
//			mylogger<<"\nClient put this message in the queue to be sent  "<<reply.toString()<<" \n";
//		}
//
//		writeOneBuffer(); //go to writing mode
//
//	}
//	void ignoreThisMessageAndWriterNextBuffer()
//	{
//		if(mylogger.isTraceEnabled())
//		{
//			mylogger<<"\nClient is ignoring this message and going to write mode  \n";
//
//		}
//		writeOneBuffer();
//
//	}
	void print(const boost::system::error_code& error,
			std::size_t t,string& str)
	{
		if(mylogger.isTraceEnabled())
		{
			mylogger<<"\nClient Received :  "<<t<<" bytes from server ";
			if(error) mylogger<<" error msg : "<<error.message()<<" data  read from server is "<<str<<"-------------------------\n";

		}

	}

	//this is the driver function thats part of driver api
	queueType insert(std::string key,std::string value) {

		string command("insert");
		string database("currencyDB");
		string user("currencyUser");
		string col("currencyCollection");

		//queueType msg (key,value,command,database,user,col );
		queueType msg (new pico_message(key,value,command,database,user,col ));
		queueRequestMessages(msg);

		if(mylogger.isTraceEnabled())
		{
			mylogger<<"\nClient : one message was pushed to requestQueue with this messageId "<< msg->messageId<<"\n";

		}
		return getTheResponseOfRequest(msg);
	}

	queueType deleteRecord(std::string key) {

		string command("delete");
		string database("currencyDB");
		string user("currencyUser");
		string col("currencyCollection");
		std::string value("unimportant");
		//queueType msg (key,value,command,database,user,col );
		queueType msg (new pico_message(key,value,command,database,user,col ));
		queueRequestMessages(msg);
		return getTheResponseOfRequest(msg);
	}

	queueType update(std::string key,std::string newValue) {

		string command("update");
		string database("currencyDB");
		string user("currencyUser");
		string col("currencyCollection");
		string oldValue("notimportant");
		//queueType msg (key,oldValue,newValue,command,database,user,col );
		queueType msg (new pico_message(key,newValue,command,database,user,col ));
		queueRequestMessages(msg);
		return getTheResponseOfRequest(msg);
	}

	queueType getTheResponseOfRequest(queueType msg,long userTimeOut=10)
	{
		bool testPassed = false;
		steady_clock::time_point t1 = steady_clock::now(); //time that we started waiting for result
		mylogger<<"Client : waiting for our response from server...msg.messageId = "<< msg->messageId<< " \n";
		std::unique_lock<std::mutex> responseQueueIsEmptyLock(responseQueueMutex);
        steady_clock::time_point t2 = steady_clock::now(); //time that we are going to check to determine timeout
        while(true)
		{

			if(responseQueue_.empty())
			{
				if(mylogger.isTraceEnabled()) {mylogger<<"Client : waiting for our responseQueue_ to be filled again  !\n";}
//				auto now = std::chrono::system_clock::now();
				responseQueueIsEmpty.wait_until(responseQueueIsEmptyLock, t2 +std::chrono::milliseconds(userTimeOut*1000));
                if(responseQueue_.empty()){break;}
                
			}
			
//            if(!responseQueue_.empty())
//            {
				queueType response = responseQueue_.peek();
                
				if(response->messageId.compare(msg->messageId)==0)
				{
					responseQueue_.remove(response); //remove this from the responseQueue_
					//this is our response
					if(mylogger.isTraceEnabled()) {
						mylogger<<"Client : got our response"<<response->messageId<<"\n"<<
						"this is our response "<<response->value;
					}
                    
					testPassed = true;
					if(response->value.compare("NODATAFOUND")==0)
					{
                        
						response->value = "NULL";
						//recalculate all the json form of message and hash code
						//and etc
                        
					}
					return response;
				}
//
//            }
//            
//				else
//				{
//					
//
//					duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
//					double timeoutInSeconds = time_span.count();
//					if(timeoutInSeconds>=userTimeOut)
//					{
//						//we ran out of time, get failed....
//						if(mylogger.isTraceEnabled()) {mylogger<<"Client : "<<msg->command<<" Operation TIMED OUT!!\n";}
//						break;
//
//					}
//					else {
//
//					}
//
//				
//			}
			
		} //while
		assert(testPassed);
		std::string timeout("OPERATION TIMED OUT for this command :  !");
		timeout.append(msg->command);
		timeout.append(" with this message id : ");
		timeout.append(msg->messageId);
		//pico_message timeoutResponse(msg.key,timeout,msg.messageId);
		queueType timeoutResponse(new pico_message(msg->key,timeout,msg->messageId));
		return timeoutResponse;

	}
	queueType get(std::string key,double userTimeOut=10) {

		string command("get");
		string database("currencyDB");
		string user("currencyUser");
		string col("currencyCollection");
		string oldValue("");
		string newValue("");
		//queueType msg (key,oldValue,newValue,command,database,user,col );
		queueType msg(new pico_message(key,oldValue,newValue,command,database,user,col) );
		queueRequestMessages(msg);
		return getTheResponseOfRequest(msg);

	}

	void queueTheResponse(queueType msg)

	{
		mylogger<<"client : putting the response in the queue "<<msg->toString();
		responseQueue_.push(msg);

		mylogger<<"\n client : response pushed to responseQUEUE , queue size is "<<responseQueue_.size()<<" \n";

		responseQueueIsEmpty.notify_all();

	}
	void queueRequestMessages(queueType message) {
		//TODO put a lock here to make the all the buffers in a message go after each other.
		try {

			boost::unique_lock<std::mutex> writeOneBufferMutexLock(writeOneBufferMutex);
			pico_buffered_message<pico_record> msg_in_buffers =
			message->getCompleteMessageInJsonAsBuffers();

			while(!msg_in_buffers.empty())
			{

				mylogger<<"\nPonocoDriver : queueRequestMessages : popping current Buffer \n";
				pico_record buf = msg_in_buffers.pop();
				mylogger<<"nPonocoDriver : popping current Buffer this is current buffer and pushing it to the bufferQueue to send "<<buf.toString()<<" \n";

				std::shared_ptr<pico_record> curBufPtr(new pico_record(buf));
				bufferQueuePtr_->push(curBufPtr);
			}

			bufferQueueIsEmpty.notify_all();
		} catch (...) {
			std::cerr << "Exception: queueRequestMessages  message : unknown thrown" << "\n";
			raise(SIGABRT);

		}
	}
	pico_buffered_message<pico_record> allBuffersReadFromTheOtherSide;

	asyncReader asyncReader_;
}; //end of class
}
 //end of namespace
#endif

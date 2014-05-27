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
#include <pico/ConcurrentVector.h>
#include <pico/pico_buffered_message.h>
#include <logger.h>
#include <pico/pico_session.h> //for the checking if appended function and sendmetherestofdata function
#include <pico/pico_test.h>
#include <pico/pico_logger_wrapper.h>
#include <ClientResponseProcessor.h>
#include <PonocoDriverHelper.h>
#include <PicoConfig.h>
#include <pico/MessageSender.h>
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
	ConcurrentVector<queueType,VectorTraits<pico_messageForResponseQueue_>> responseQueue_;
	std::shared_ptr<ConcurrentVector<std::shared_ptr<pico_record>,VectorTraits<pico_record>>> bufferQueuePtr_;

	bool clientIsConnected;

//	std::mutex writeOneBufferMutex;   // mutex for the condition variable
//	std::mutex allowedToWriteLockMutex;
//	std::mutex queueRequestMessagesMutex;
//	std::mutex responseQueueMutex;
//	std::mutex waitForClientToConnectMutex;

//	std::condition_variable clientIsConnectedCV;
//	std::condition_variable clientIsAllowedToWrite;
    std::mutex queueMessagesMutext;
	std::condition_variable bufferQueueIsEmpty;
	std::condition_variable responseQueueIsEmpty;
    std::mutex bufferQueueIsEmptyMutex;

	string database;
	string user;
	string col;
    MessageSender* messageSender;
    pico_buffered_message<pico_record> allBuffersReadFromTheOtherSide;
    bool shutDownNormally;
	asyncReader asyncReader_;

public:

	PonocoDriver(helperType syncHelperArg )

	:bufferQueuePtr_(new ConcurrentVector<std::shared_ptr<pico_record>,VectorTraits<pico_record>>)
    ,messageSender(new MessageSender()),shutDownNormally(false)
	{
		if(clientLogger->isTraceEnabled())
		{
			*clientLogger<< "Ponoco Instance is initializing  ";
            
		}
		setDefaultParameter();

	}
	void setDefaultParameter()
	{
		database.append("currencyDB");
		user.append("currencyUser");
		col.append("currencyCollection");

	}
	void start_connect(std::shared_ptr<tcp::socket> socket,tcp::resolver::iterator endpoint_iter) {
		try {
			socket_ = socket;
			startResponseQueueNotifier();
			if(clientLogger->isTraceEnabled())
			{
				*clientLogger<<" start_connect(tcp::resolver::iterator endpoint_iter) ";
			}

			if (endpoint_iter != tcp::resolver::iterator()) {

				if(clientLogger->isTraceEnabled())
				{
					*clientLogger<< "Trying "<<endpoint_iter->endpoint() << "...\n";
				}
				// Start the asynchronous connect operation.
				socket_->async_connect(endpoint_iter->endpoint(),
						boost::bind(&PonocoDriver::start, this, _1, endpoint_iter));
			} else {
				// There are no more endpoints to try. Shut down the client.
				//				stop();
			}
			if(clientLogger->isTraceEnabled())
			{
				*clientLogger<< " start_connect ends ...\n";
			}

		} catch(...)
		{
			if(clientLogger->isTraceEnabled())
			{
				*clientLogger<< "client start_connect : start_connect : Exception: unknown thrown\n";
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
			*clientLogger<<( "Connect timed out\n");

			// Try the next available endpoint.
			start_connect(this->socket_,++endpoint_iter);
		}
	}

	void start(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
		//            clientLogger<<"\nclient starting the process..going to write_message to server\n";

		try {
			if(!ec)
			{
				//clientIsConnectedCV.notify_all();
				clientIsConnected=true;
				writeOneBuffer();//this starts the writing, if bufferQueue is empty it waits for it.
			}
			else {
				if(clientLogger->isTraceEnabled())
				{
					*clientLogger<<"\nclient : start : error is "<<ec.value()<<" error message is "<<ec.message();
					*clientLogger <<"\n error name is "<< ec.category().name();}
			}

		} catch (const std::exception& e) {
			*clientLogger<<" exception : "<<e.what();
			raise(SIGABRT);

		} catch (...) {
			*clientLogger<< "<----->unknown exception thrown.<------>\n";
			raise(SIGABRT);

		}
	}

//	void readSynchronously() {
//		for (;;) {
//			boost::array<char, 128> buf;
//			boost::system::error_code error;
//
//			size_t len = socket_->read_some(boost::asio::buffer(buf), error);
//
//			if (error == boost::asio::error::eof)
//			break; // Connection closed cleanly by peer.
//			else if (error)
//			throw boost::system::system_error(error);// Some other error.
//
//			clientLogger << "\nclient got this " << buf.data();
//		}
//	}
    
    void readOneBuffer(int dataSizeToReadNext) {
        assert(dataSizeToReadNext>0);
 
        auto self(shared_from_this());
		// clientLogger<<"client is trying to read one buffer\n" ;
		std::shared_ptr<pico_record> currentBuffer = asyncReader_.getOneBuffer();
        int numberOfCharsToRead = dataSizeToReadNext;
        
        if (clientLogger->isTraceEnabled()) {
            *clientLogger<< "\n Client is going to read "<<numberOfCharsToRead<<" chars into buffer from server...\n";
        }
        
		boost::asio::async_read(*socket_,
				boost::asio::buffer(currentBuffer->getDataForRead(numberOfCharsToRead),
						numberOfCharsToRead),
				[this,self,currentBuffer,numberOfCharsToRead](const boost::system::error_code& error,
						std::size_t t ) {
                    if (clientLogger->isTraceEnabled()) {
                        *clientLogger<< "\n Client is done reading  "<<numberOfCharsToRead<<" chars from server...\n";
                    }
                    
                    self->processTheBufferJustRead(currentBuffer,t);
				});

	}
	void writeOneBuffer()
	{

        if (clientLogger->isTraceEnabled()) {
            *clientLogger<< "\n client is going to send a buffer to server..going to acquire lock \n";
        }
        
        std::unique_lock < std::mutex
        > writeOneBufferLock(bufferQueueIsEmptyMutex);
        while (bufferQueuePtr_->empty()) {
            if (clientLogger->isTraceEnabled()) {
                *clientLogger<< "\n client bufferQueue_ is empty...waiting for a message to come to queue. \n";
            }
            bufferQueueIsEmpty.wait(writeOneBufferLock);
        }
        if (clientLogger->isTraceEnabled()) {
            *clientLogger<< "\n client is going to send a buffer to server.. lock obtained \n";
        }
        
        std::shared_ptr<pico_record> currentBuffer = bufferQueuePtr_->pop();
        // cout << " session is writing one buffer to client : " <<currentBuffer->toString()//<< std::endl;
        string data = currentBuffer->getDataForWrite();
        std::size_t dataSize = currentBuffer->getSize();
        
        string dataSizeAsStr = convertToString(dataSize);
        
        string properMessageAboutSize;
        getProperMessageAboutSize(dataSizeAsStr,properMessageAboutSize);
        writeOneMessageToOtherSide(properMessageAboutSize.c_str(),10,true,data,dataSize);
        
        readOneBuffer(10);
        
	}
    
    void processTheBufferJustRead(std::shared_ptr<pico_record> currentBuffer,
                                  std::size_t t) {
       // messageNumber++;
        string str = currentBuffer->toString();
        *clientLogger << "\nClient : data read just now is  \n"<<str;
        if(pico_record::IsThisRecordASizeInfo(currentBuffer))
        {
            string sizeOfNextBufferToReadStr;
            for(int i=0;i<currentBuffer->getSize();i++)
            {
                if(currentBuffer->getChar(i)!='#')
                    sizeOfNextBufferToReadStr.push_back(currentBuffer->getChar(i));
            }
            long nextDataSize = convertToSomething<long>(sizeOfNextBufferToReadStr.c_str());
            *clientLogger << "\nClient : dataSize read just now says that next data size is   \n"<<nextDataSize;
            
            readOneBuffer(nextDataSize);
            
        }
        else{
            *clientLogger << "\nClient : data read just now is not a dataSize Info   \n";
            *clientLogger << "\nClient : this is the complete message read from server ";
            msgPtr last_read_message(new pico_message(str));
            processDataFromOtherSide(last_read_message);
            writeOneBuffer(); //going to writing mode to write the reply for this complete message
        }
        //            else
        //                            {
        //                                *sessionLogger << "\nsession : ERROR : reading empty message  \n";
        //                                readOneBuffer();		//read the next addon buffer
        //
        //                            }
        
    }

	void processDataFromOtherSide(queueType messageFromOtherSide) {

		try {

			if(clientLogger->isTraceEnabled())
			{
				*clientLogger<<"\n client this is the complete message from server : "<<messageFromOtherSide->toString();
			}
			//process the data from server and queue the right message or dont

			// TODO
			responseProcessor.processResponse(messageFromOtherSide);

			queueTheResponse(messageFromOtherSide);

			if(clientLogger->isTraceEnabled())
			{

				*clientLogger<<"\n Client : got the response with this messageId :  "<<
				messageFromOtherSide->messageId<<" and put it in ResponseQueue \n this is the full response : "<<messageFromOtherSide->toString();
				assert(messageFromOtherSide->toString().size()>0);
			}

		} catch (std::exception &e) {
			cout << " this is the error : " << e.what() << endl;
		}
	}

	void print(const boost::system::error_code& error,
			std::size_t t,const char* data)
	{
        if(clientLogger->isTraceEnabled())
        {
            string str(data);
            *clientLogger<< "\n Client Sent :  "<<t<<" bytes to Server ";
            if(error) *clientLogger<<" \nClient : a communication error happend...\n msg : "<<error.message();
            *clientLogger<<" nClient : data sent to server is "<<str<<"\n";
            *clientLogger<<("-------------------------");
        }
		

	}

	//this is the driver function thats part of driver api
	queueType insert(std::string key,std::string value) {

		string command("insert");

		//queueType msg (key,value,command,database,user,col );
		queueType msg (new pico_message(key,value,command,database,user,col ));
		queueRequestMessages(msg);

		if(clientLogger->isTraceEnabled())
		{
			*clientLogger<<"\nClient : one message was pushed to requestQueue with this messageId "<< msg->messageId<<"\n";

		}
		return getTheResponseOfRequest(msg);
	}

	queueType deleteCollection(std::string collectionName) {
		string key("unimportant");
		string value("unimportant");

		string command("deleteCollection");

		//queueType msg (key,value,command,database,user,col );
		queueType msg (new pico_message(key,value,command,database,user,col ));
		queueRequestMessages(msg);

		if(clientLogger->isTraceEnabled())
		{
			*clientLogger<<"\nClient : one message was pushed to requestQueue with this messageId "<< msg->messageId<<"\n";

		}
		return getTheResponseOfRequest(msg);

	}

	queueType createCollection(std::string collectionName) {
		string key("unimportant");
		string value("unimportant");
		string command("createCollection");

		//queueType msg (key,value,command,database,user,col );
		queueType msg (new pico_message(key,value,command,database,user,col ));
		queueRequestMessages(msg);

		if(clientLogger->isTraceEnabled())
		{
			*clientLogger<<"\nClient : one message was pushed to requestQueue with this messageId "<< msg->messageId<<"\n";

		}
		return getTheResponseOfRequest(msg);

	}

	queueType deleteRecord(std::string key) {

		string command("delete");
		std::string value("unimportant");

		queueType msg (new pico_message(key,value,command,database,user,col ));
		queueRequestMessages(msg);
		return getTheResponseOfRequest(msg);
	}

	queueType update(std::string key,std::string newValue) {

		string command("update");

		queueType msg (new pico_message(key,newValue,command,database,user,col ));
		queueRequestMessages(msg);
		return getTheResponseOfRequest(msg);
	}

	queueType getTheResponseOfRequest(queueType msg,long userTimeOut=PicoConfig::defaultTimeoutInSec)
	{
		bool testPassed = false;
		steady_clock::time_point t1 = steady_clock::now(); //time that we started waiting for result
		*clientLogger<<"Client : waiting for our response from server...msg.messageId = "<< msg->messageId<< " \n";
		steady_clock::time_point t2 = steady_clock::now();//time that we are going to check to determine timeout
		while(true)
		{

			if(responseQueue_.empty())
			{
			//	if(clientLogger->isTraceEnabled()) {
			//		*clientLogger<<"Client : waiting for our responseQueue_ to be filled again  !\n";}
//				auto now = std::chrono::system_clock::now();
//                std::unique_lock<std::mutex> responseQueueIsEmptyLock(responseQueueMutex);
//                responseQueueIsEmpty.wait_until(responseQueueIsEmptyLock, t2 +std::chrono::milliseconds(userTimeOut*1000));
//				if(responseQueue_.empty()) {break;}

			}

            if(!responseQueue_.empty())
            {
                queueType response = responseQueue_.peek();

			if(msg!=NULL && response!=nullptr && response->messageId.compare(msg->messageId)==0)
			{
				responseQueue_.remove(response); //remove this from the responseQueue_
				//this is our response
				if(clientLogger->isTraceEnabled()) {
					*clientLogger<<"Client : got our response"<<response->messageId<<"\n"<<
					"this is our response "<<response->toString();
				}

				testPassed = true;
				if(response->value.compare("NODATAFOUND")==0)
				{
					testPassed = false;
					response->value = "NULL";
					//recalculate all the json form of message and hash code
					//and etc

				}
				return response;
			}
//
            }
//
            
					duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
					double timeoutInSeconds = time_span.count();
					if(timeoutInSeconds>=userTimeOut)
					{
						//we ran out of time, get failed....
						if(clientLogger->isTraceEnabled()) {*clientLogger<<"Client : "<<msg->command<<" Operation TIMED OUT!!\n";}
						break;

					}
			

		} //while
		  //assert(testPassed);
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
		string newValue("");
		queueType msg(new pico_message(key,newValue,command,database,user,col) );
		queueRequestMessages(msg);
		return getTheResponseOfRequest(msg);

	}
	void startResponseQueueNotifier()
	{
		try {
//			boost::thread notifierThread(
//					boost::bind(&PonocoDriver::notifierService,this));

		} catch(std::exception& e)
		{
			*clientLogger<<" Exception in notifierThread "<<e.what()<<"\n";
		}

	}
	void notifierService()
	{
//		while(true)
//		{
//			if(!responseQueue_.empty())
//			{
//				responseQueueIsEmpty.notify_all();
//			}
//
//		}
	}
	void queueTheResponse(queueType msg)

	{
		if(clientLogger->isTraceEnabled())
		{
			*clientLogger<<"client : putting the response in the queue "<<msg->toString();

		}
		responseQueueIsEmpty.notify_all();
		responseQueue_.push_back(msg);

		if(clientLogger->isTraceEnabled())
		{	*clientLogger<<"\n client : response pushed to responseQUEUE , queue size is "<<responseQueue_.size()<<" \n";
		}

	}
    void queueRequestMessages(queueType message) {
        //TODO put a lock here to make the all the buffers in a message go after each other.
        
        std::unique_lock < std::mutex> queueMessagesLock(queueMessagesMutext);
        std::shared_ptr<pico_record> curBufPtr =  message->getCompleteMessageInJsonAsOnBuffer();
        bufferQueuePtr_->push_back(curBufPtr);
        bufferQueueIsEmpty.notify_all();
        
    }
    void writeOneMessageToOtherSide(const char* data,std::size_t dataSize,bool sendTheRealData,const string& realData,std::size_t realDataSize)
    {
        auto self(shared_from_this());
        
        boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
                                 [this,self,data,sendTheRealData,realData,realDataSize](const boost::system::error_code& error,
                                                  std::size_t t) {
                                     
                                     print(error,t,data);
                                     if(sendTheRealData)
                                     {
                                         writeOneMessageToOtherSide(realData.c_str(),realDataSize,false,"",-1);//this is the real data
                                     }
                                     
                                 });
        
    }
    ~PonocoDriver()
    {
        assert(shutDownNormally);
        *clientLogger<<"\nPoncoDriver being destroyed";
    }
}; //end of class
}
 //end of namespace
#endif

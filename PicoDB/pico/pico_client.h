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
#include <pico/pico_test.h>
#include <pico/pico_logger_wrapper.h>
#include <ClientResponseProcessor.h>
#include <PonocoDriverHelper.h>
#include <PicoConfig.h>
#include <pico/MessageSender.h>
//#include <pico/pico_session.h>//for format function, i dont know why
#include <boost/format.hpp>
#include <AtomicCounter.h>
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
	typedef PonocoDriverHelper* helperType;

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
    AtomicCounter numberOfMessageRead;
    AtomicCounter numberOfMessageSent;
    
public:

	PonocoDriver()

	:bufferQueuePtr_(new ConcurrentVector<std::shared_ptr<pico_record>,VectorTraits<pico_record>>)
    ,messageSender(new MessageSender()),shutDownNormally(false)
	{
		if(clientLogger->isTraceEnabled())
		{
			clientLogger->log( "Ponoco Instance is initializing  ");
            
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
				clientLogger->log(" start_connect(tcp::resolver::iterator endpoint_iter) ");
			}

			if (endpoint_iter != tcp::resolver::iterator()) {

				if(clientLogger->isTraceEnabled())
				{
                    string logMsg("Trying ");
                    logMsg.append(convertToString(endpoint_iter->endpoint()));
                    logMsg.append("...");
					clientLogger->log(logMsg );
                    
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
				clientLogger->log( " start_connect ends ...");
			}

		} catch(...)
		{
			if(clientLogger->isTraceEnabled())
			{
				clientLogger->log( "client start_connect : start_connect : Exception: unknown thrown");
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
			clientLogger->log( "Connect timed out");

			// Try the next available endpoint.
			start_connect(this->socket_,++endpoint_iter);
		}
	}

	void start(const boost::system::error_code& ec,
			tcp::resolver::iterator endpoint_iter) {
		//            clientLogger<<"client starting the process..going to write_message to server";

		try {
			if(!ec)
			{
				//clientIsConnectedCV.notify_all();
				clientIsConnected=true;
				while(true)
                {
                    writeOneBuffer();//this starts the writing, if bufferQueue is empty it waits for it.
                    readOneBuffer(10);//read the reply
                    
                }
			}
			else {
				if(clientLogger->isTraceEnabled())
				{
                    string logMsg("client : start : error is");
                    logMsg.append(convertToString(ec.value()));
                    logMsg.append(" error message is ");
                    logMsg.append(ec.message());
                    logMsg.append(" error name is ");
                    logMsg.append(ec.category().name());
                	clientLogger->log(logMsg);
                }
			}

		} catch (const std::exception& e) {
            string logMsg(" exception : ");
            logMsg.append(e.what());
            clientLogger->log(logMsg);
			raise(SIGABRT);

		} catch (...) {
			clientLogger->log( "<----->unknown exception thrown.<------>");
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
//			clientLogger << "client got this " << buf.data();
//		}
//	}
    
    void readOneBuffer(int dataSizeToReadNext) {
        assert(dataSizeToReadNext>0);
 
        auto self(shared_from_this());
		// clientLogger<<"client is trying to read one buffer" ;
		std::shared_ptr<pico_record> currentBuffer = asyncReader_.getOneBuffer();
        int numberOfCharsToRead = dataSizeToReadNext;
        
        if (clientLogger->isTraceEnabled()) {
            string logMsg(" Client is going to read ");
            logMsg.append(toStr(numberOfCharsToRead));
            logMsg.append(" chars into buffer from server...");
            clientLogger->log(logMsg);
        }
        
        numberOfCharsToRead = dataSizeToReadNext;
        
        boost::system::error_code error;
//        size_t length = boost::asio::read(*socket_,boost::asio::buffer(currentBuffer->getDataForRead(numberOfCharsToRead), numberOfCharsToRead), error);

        size_t length = socket_->read_some(boost::asio::buffer(currentBuffer->getDataForRead(numberOfCharsToRead), numberOfCharsToRead), error);

        if (clientLogger->isTraceEnabled()) {
            
            string logMsg(" Client is done reading  ");
            logMsg.append(toStr(numberOfCharsToRead));
            logMsg.append(" chars from server...");
            clientLogger->log(logMsg);
        
        }
        
        self->processTheBufferJustRead(currentBuffer,length);

        
//		boost::asio::async_read(*socket_,
//				boost::asio::buffer(currentBuffer->getDataForRead(numberOfCharsToRead),
//						numberOfCharsToRead),
//				[this,self,currentBuffer,numberOfCharsToRead](const boost::system::error_code& error,
//						std::size_t t ) {
//                    if (clientLogger->isTraceEnabled()) {
//                        clientLogger->log( " Client is done reading  "<<numberOfCharsToRead<<" chars from server...";
//                    }
//                    
//                    self->processTheBufferJustRead(currentBuffer,t);
//				});

	}
	void writeOneBuffer()
	{

        if (clientLogger->isTraceEnabled()) {
            clientLogger->log( " client is going to send a buffer to server..going to acquire lock ");
        }
        
      while (bufferQueuePtr_->empty()) {
            if (clientLogger->isTraceEnabled()) {
                clientLogger->log( " client bufferQueue_ is empty...waiting for a message to come to queue. ");
            }
            std::unique_lock <std::mutex> writeOneBufferLock(bufferQueueIsEmptyMutex);
            bufferQueueIsEmpty.wait(writeOneBufferLock);
        }
        if (clientLogger->isTraceEnabled()) {
            clientLogger->log( " client is going to send a buffer to server.. lock obtained ");
        }
        
        std::shared_ptr<pico_record> currentBuffer = bufferQueuePtr_->pop();
        // cout << " session is writing one buffer to client : " <<currentBuffer->toString()//<< std::endl;
        string data = currentBuffer->getDataForWrite();
        std::size_t dataSize = currentBuffer->getSize();
        
        string dataSizeAsStr = convertToString(dataSize);
        
        string properMessageAboutSize;
        getProperMessageAboutSize(dataSizeAsStr,properMessageAboutSize);
        writeOneMessageToOtherSide(properMessageAboutSize.c_str(),10,true,data,dataSize);
        
        
	}
    
    void processTheBufferJustRead(std::shared_ptr<pico_record> currentBuffer,
                                  std::size_t t) {
        string str = currentBuffer->toString();
        *clientLogger << "Client : data read just now is  "<<str;
        if(pico_record::IsThisRecordASizeInfo(currentBuffer))
        {
            string sizeOfNextBufferToReadStr;
            for(int i=0;i<currentBuffer->getSize();i++)
            {
                if(currentBuffer->getChar(i)!='#')
                    sizeOfNextBufferToReadStr.push_back(currentBuffer->getChar(i));
            }
            long nextDataSize = convertToSomething<long>(sizeOfNextBufferToReadStr.c_str());
            *clientLogger << "Client : dataSize read just now says that next data size is   "<<nextDataSize;
            
            readOneBuffer(nextDataSize);
            
        }
        else{
            *clientLogger << "Client : data read just now is not a dataSize Info   ";
            *clientLogger << "Client : this is the complete message read from server ";
            msgPtr last_read_message(new pico_message(str));
            processDataFromOtherSide(last_read_message);
    
        }
        //            else
        //                            {
        //                                *sessionLogger << "session : ERROR : reading empty message  ";
        //                                readOneBuffer();		//read the next addon buffer
        //
        //                            }
        
    }

	void processDataFromOtherSide(queueType messageFromOtherSide) {

		try {
            numberOfMessageRead.increment();
            
			if(clientLogger->isTraceEnabled())
			{
                
                clientLogger->log(toStr(boost::format(" client this is the complete message from server : %1% ,its the %2% message read from server") % messageFromOtherSide->toString() % numberOfMessageRead.get()));
                
            }
			//process the data from server and queue the right message or dont

			// TODO
			responseProcessor.processResponse(messageFromOtherSide);

			queueTheResponse(messageFromOtherSide);

			if(clientLogger->isTraceEnabled())
			{
                
                string logMsg(" Client : got the response with this messageId :  ");
                logMsg.append(messageFromOtherSide->messageId);
                logMsg.append(" and put it in ResponseQueue  this is the full response : ");
                logMsg.append(messageFromOtherSide->toString());
                clientLogger->log(logMsg);
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
            
            
            string logMsg(" Client Sent :  ");
            logMsg.append(toStr(t));
            logMsg.append(" bytes to Server ");
            clientLogger->log(logMsg);
            
            
          
            if(error)
            {
                string logMsg(" Client : a communication error happend... msg : ");
                logMsg.append(error.message());
                clientLogger->log(logMsg);
            }
            
            
            {
                string logMsg("Client : data sent to server is ");
                logMsg.append(str);
                logMsg.append("");
                
                clientLogger->log(logMsg);
                
            }
            
            
            clientLogger->log("-------------------------");
            
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
            
            string logMsg("Client : one message was pushed to requestQueue with this messageId ");
            logMsg.append(msg->messageId);
            logMsg.append("");
            clientLogger->log(logMsg);
        
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
            
            string logMsg("Client : one message was pushed to requestQueue with this messageId ");
            logMsg.append(msg->messageId);
            logMsg.append("");
            clientLogger->log(logMsg);
            
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
            
            string logMsg("Client : one message was pushed to requestQueue with this messageId ");
            logMsg.append(msg->messageId);
            logMsg.append("");
            clientLogger->log(logMsg);
    
            
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
		
        
        if(clientLogger->isTraceEnabled())
		{
            
            string logMsg("Client : waiting for our response from server...msg.messageId = ");
            logMsg.append(msg->messageId);
            logMsg.append("");
            clientLogger->log(logMsg);
            
            
        }
        
		steady_clock::time_point t2 = steady_clock::now();//time that we are going to check to determine timeout
		while(true)
		{

//			if(responseQueue_.empty())
//			{
//                
//			//	if(clientLogger->isTraceEnabled()) {
//			//		clientLogger->log("Client : waiting for our responseQueue_ to be filled again  !";}
////				auto now = std::chrono::system_clock::now();
////                std::unique_lock<std::mutex> responseQueueIsEmptyLock(responseQueueMutex);
////                responseQueueIsEmpty.wait_until(responseQueueIsEmptyLock, t2 +std::chrono::milliseconds(userTimeOut*1000));
////				if(responseQueue_.empty()) {break;}
//
//			}

            if(!responseQueue_.empty())
            {
                string logMsg(toStr(boost::format("Client : response queue sisze is %1% %2% %3% ") % responseQueue_.size() % "oo" % "O"));
                clientLogger->log(logMsg);

                queueType response = responseQueue_.peek();

			if(msg!=NULL && response!=nullptr && response->messageId.compare(msg->messageId)==0)
			{
				responseQueue_.remove(response); //remove this from the responseQueue_
				//this is our response
				if(clientLogger->isTraceEnabled()) {
					
                    
                    string logMsg("Client : got our response");
                    logMsg.append(response->messageId);
                    logMsg.append("");
                    logMsg.append("this is our response ");
                    logMsg.append(response->toString());
                    clientLogger->log(logMsg);
                    
				}
                return response;
                
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

            }
//
            
					duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
					double timeoutInSeconds = time_span.count();
					if(timeoutInSeconds>=userTimeOut)
					{
						//we ran out of time, get failed....
						
                        string logMsg("Client : ");
                        logMsg.append(msg->command);
                        logMsg.append("Operation TIMED OUT!!");
                        clientLogger->log(logMsg);
                        
  						break;

					}
//

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
			boost::thread notifierThread(
					boost::bind(&PonocoDriver::notifierService,this));

		} catch(std::exception& e)
		{
            string logMsg(" Exception in notifierThread ");
            logMsg.append(e.what());
            logMsg.append("");
            clientLogger->log(logMsg);
        }

	}
	void notifierService()
	{
        
		while(true)
		{
			if(!responseQueue_.empty())
			{
				responseQueueIsEmpty.notify_all();
			}
            if(!bufferQueuePtr_->empty())
            {
                bufferQueueIsEmpty.notify_all();
            }

		}
	}
	void queueTheResponse(queueType msg)

	{
		if(clientLogger->isTraceEnabled())
		{
            
            string logMsg("client : putting the response in the queue ");
            logMsg.append(msg->toString());
            clientLogger->log(logMsg);
		}
		responseQueueIsEmpty.notify_all();
		responseQueue_.push_back(msg);

		if(clientLogger->isTraceEnabled())
 		{
            
            string logMsg(" client : response pushed to responseQUEUE , queue size is");
            logMsg.append(toStr(responseQueue_.size()));
            logMsg.append("");
            clientLogger->log(logMsg);
            
		}

	}
    void queueRequestMessages(queueType message) {
        //TODO put a lock here to make the all the buffers in a message go after each other.
        
//        std::unique_lock < std::mutex> queueMessagesLock(queueMessagesMutext);
        std::shared_ptr<pico_record> curBufPtr =  message->getCompleteMessageInJsonAsOnBuffer();
        
        bufferQueuePtr_->push_back(curBufPtr);
        bufferQueueIsEmpty.notify_all();
        
    }
    
    void writeOneMessageToOtherSide(const char* data,std::size_t dataSize,bool sendTheRealData,const string& realData,std::size_t realDataSize)
    {
        numberOfMessageSent.increment();
        
        boost::asio::write(*socket_, boost::asio::buffer(data, dataSize));
        
        if(sendTheRealData)
        {
           writeOneMessageToOtherSide(realData.c_str(),realDataSize,false,"",-1);//this is the real data
        }
        if(clientLogger->isTraceEnabled())
        {
            
            clientLogger->log(toStr(boost::format("Client : sent the %2%th message to server %1% ") % data %numberOfMessageSent.get()));
            
        }
    }
    
//    void writeOneMessageToOtherSideAsync(const char* data,std::size_t dataSize,bool sendTheRealData,const string& realData,std::size_t realDataSize)
//    {
//        auto self(shared_from_this());
//        
//        boost::asio::async_write(*socket_, boost::asio::buffer(data, dataSize),
//                                 [this,self,data,sendTheRealData,realData,realDataSize](const boost::system::error_code& error,
//                                                  std::size_t t) {
//                                     
//                                     print(error,t,data);
//                                     if(sendTheRealData)
//                                     {
//                                         writeOneMessageToOtherSide(realData.c_str(),realDataSize,false,"",-1);//this is the real data
//                                     }
//                                     
//                                 });
//        
//    }
    ~PonocoDriver()
    {
        assert(shutDownNormally);
        clientLogger->log("PoncoDriver being destroyed");
    }
}; //end of class
}
 //end of namespace
#endif

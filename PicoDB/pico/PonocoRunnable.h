//
//  PonocoRunnable.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 5/1/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_PonocoRunnable_h
#define PicoDB_PonocoRunnable_h
#include <pico/pico_client.h>
#include <PonocoDriverHelper.h>
#include <PonocoClient.h>
namespace  pico  {
    class PonocoRunnable;
    typedef std::shared_ptr<PonocoRunnable> pRunnableType;
    
    class PonocoRunnable { //this is a class that helps bind whatever objects
      //we have to boost thread easily
        DriverType* driverPtr;
    PonocoDriverHelper* driverSyncHelper;
    public:
        PonocoRunnable( DriverType* driverPtrArg,
                       PonocoDriverHelper* driverSyncHelperArg) {
            driverPtr= driverPtrArg;
            driverSyncHelper=driverSyncHelperArg;
        }
        void runPoncoClientProgram() //this is the third party program that is going to put
        //messages in the request queue of the PonocoDriver
        {
        	assert(driverPtr!=NULL);
            PonocoClient myClient(driverPtr);
            myClient.setPonocoDriver(driverPtr);
            sleepViaBoost(1);
            myClient.currentTestCase();
            
            
        }
        virtual ~PonocoRunnable() {
//            delete driverPtr;
//            delete driverSyncHelper;
        }
   
  
    void runPonocoDriver() {
        try {
            //	mylogger << "starting client" //<< std::endl;
            std::string localhost { "0.0.0.0" };// #Symbolic name meaning all available interfaces
            
            //std::string localhost{"localhost"};
            //        std::string localhost{"127.0.0.1"};
            //only the local machine via a special interface only visible to programs running on the same compute
            std::string port { "8877" };
            
            boost::asio::io_service io_service;
            tcp::resolver r(io_service);
            
            std::shared_ptr<tcp::socket> socketWriter1(new tcp::socket(io_service));
            std::shared_ptr<tcp::socket> socketWriter2(new tcp::socket(io_service));
            std::shared_ptr<tcp::socket> socketReader1(new tcp::socket(io_service));
            std::shared_ptr<tcp::socket> socketReader2(new tcp::socket(io_service));
            
            driverPtr->start_connect(socketWriter1,r.resolve(tcp::resolver::query(localhost, port)));
            //		boost::thread shellThread(
            //				boost::bind(startTheShell, ptr)); //this will run the shell process that reads command and send to client
            //and client sends to server
            
            
            //        boost::thread hedgeThred(boost::bind(runPicoHedgeFundClient, ptr));
            //        hedgeThred.detach();
            
            io_service.run();
            std::cout << "Driver ending ..couldnt connect to the server...going out of scope" << std::endl;
            
            
            
        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << "\n";
            raise(SIGABRT);
            
            
        } catch (...) {
            std::cerr << "Exception: unknown happened for client" << "\n";
            raise(SIGABRT);
            
        }
        
        
    }
 };//class
}//namespace

#endif

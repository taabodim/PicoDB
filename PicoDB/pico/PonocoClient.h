//
//  PonocoClient.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/30/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_PonocoClient_h
#define PicoDB_PonocoClient_h
#include <pico/pico_client.h>
#include <pico/pico_test.h>
#include <pico_logger_wrapper.h>
namespace pico{

    class PonocoClient : public pico_logger_wrapper{
    private:
        PonocoDriver* driverPtr;
    public:
        
        PonocoClient(PonocoDriver* driverPtrArg)
        {
            driverPtr = driverPtrArg;
            
        }
        
        void insert1SmallKeyBigValue_And_GetIt()
        {
            
            std::string key(pico_test::smallKey0);
            driverPtr->insert(key,pico_test::bigValue0);
           
              driverPtr->get(key);
            //while(true){}//keep the thread forever
            
        }
//        void getTest(std::string key)
//        {
//            
//            std::size_t hashcodeOfSentKey =   calc_hash_code(key);
//            
//        }
//        void write1000smallRandomData()
//        {
//            
//            
//            for(int  i=0;i<1000;i++)
//                insert(random_string(20),random_string(20));
//            
//        }
//        void writeOneDeleteOne()
//        {
//            for(int  i=0;i<1;i++)
//                insert(pico_test::smallKey0,pico_test::smallValue0);
//            for(int  i=0;i<1;i++)
//                deleteTest(pico_test::smallKey0,pico_test::smallValue0);
//            
//        }
//        void writeThe_same_record_to_check_if_we_update_or_insert_multiple()
//        {
//            for(int  i=0;i<2;i++)
//            {
//                insert(pico_test::smallKey0,pico_test::smallValue0);
//            }
//        }
//        
//        void writeTenKEY0KEY1KEY2DeleteAllKEY2()
//        {
//            for(int  i=0;i<1;i++)
//            {
//                
//                insert(pico_test::smallKey0,pico_test::smallValue0);
//                insert(pico_test::smallKey1,pico_test::smallValue1);
//                insert(pico_test::smallKey2,pico_test::smallValue2);
//                insert(pico_test::smallKey2,pico_test::smallValue2);
//                insert(pico_test::smallKey2,pico_test::smallValue2);
//                insert(pico_test::smallKey0,pico_test::smallValue0);
//                insert(pico_test::smallKey1,pico_test::smallValue1);
//            }
//            for(int  i=0;i<1;i++)
//                deleteTest(pico_test::smallKey2,pico_test::smallKey2);
//            //
//        }
//        void write1000SmallKeysBigValues_and_deleteAll()
//        {
//            for(int  i=0;i<1000;i++)
//            {
//                std::string key(pico_test::smallKey0);
//                key.append(convertToString(i));
//                insert(key,pico_test::bigValue0);
//            }
//            
//            //            for(int  i=0;i<1000;i++)
//            //            {
//            //                std::string key(pico_test::smallKey0);
//            //                key.append(convertToString(i));
//            //
//            //                deleteTest(key,pico_test::smallKey2);
//            //            }
//            
//            
//        }
//        
//        void write1000SmallKeysValues_and_deleteAll()
//        {
//            for(int  i=0;i<1000;i++)
//            {
//                std::string key(pico_test::smallKey0);
//                key.append(convertToString(i));
//                insert(key,pico_test::smallValue0);
//            }
//            
//            for(int  i=0;i<1000;i++)
//            {
//                std::string key(pico_test::smallKey0);
//                key.append(convertToString(i));
//                
//                deleteTest(key,pico_test::smallKey2);
//            }
//            
//            
//        }
        
 
        void currentTestCase()
        {
            
            
            steady_clock::time_point t1 = steady_clock::now();
            
            
            
            // write1000smallRandomData();
            //            writeOneDeleteOne();
            insert1SmallKeyBigValue_And_GetIt();
            //write1000SmallKeysBigValues_and_deleteAll();
            
            
            steady_clock::time_point t2 = steady_clock::now();
            
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            
            mylogger << "****************************************\n";
            mylogger<< "\nIt took me " << time_span.count() << " seconds.\n";
            
        }
        
    };

}


#endif

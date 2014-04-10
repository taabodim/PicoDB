//
//  PicoHedgeFund.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 3/27/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//
#ifndef PicoDB_PicoHedgeFund_h
#define PicoDB_PicoHedgeFund_h


#include <currency.h>
#include <pico/pico_client.h>
#include <pico/pico_utils.h>
#include <logger.h>

using namespace std;

namespace pico {
 
    
    
    class PicoHedgeFund
    {
    public:
        std::shared_ptr<DBClient> client_;
        //logger mylogger;
        PicoHedgeFund(std::shared_ptr<DBClient> client)
        {
            std::cout<<(" hedge fund is going to buy some currencies");
            client_ = client;
           
        }
        void buy(int num)
        {
            std::cout<<(" hedge fund is waiting for 3 seconds");
            
            boost::this_thread::sleep(boost::posix_time::seconds(3));
            for(int i=0;i<num;i++)
            {
                //std::cout<<("hedge fund is buying currency..");
                Currency c1;
                c1.name = "They've been spotted and spotted again, those objects in the southern Indian Ocean. Every time a report comes out that something has been seen that may be related to missing Malaysia Flight 370, hopes have risen. And then, they have fallen. It's seemed like a daily exercise.showed about 300 objects ranging in size from 6 feet (2 meters) to 50 feet (15 meters). When photographed Monday, they were about 125 miles (201 kilometers) away from the spot where a French satellite captured images of a floating group of objects Sunday.They've been spotted and spotted again, those objects in the southern Indian Ocean. Every time a report comes out that something has been seen that may be related to missing Malaysia Flight 370, hopes have risen. And then, they have fallen. It's seemed like a daily exercise.showed about 300 objects ranging in size from 6 feet (2 meters) to 50 feet (15 meters). When photographed Monday, they were about 125 miles (201 kilometers) away from the spot where a French satellite captured images of a floating group of objects Sunday.They've been spotted and spotted again, those objects in the southern Indian Ocean. Every time a report comes out that something has been seen that may be related to missing Malaysia Flight 370, hopes have risen. And then, they have fallen. It's seemed like a daily exercise.showed about 300 objects ranging in size from 6 feet (2 meters) to 50 feet (15 meters). When photographed Monday, they were about 125 miles (201 kilometers) away from the spot where a French satellite captured images of a floating group of objects Sunday.They've been spotted and spotted again, those objects in the southern Indian Ocean. Every time a report comes out that something has been seen that may be related to missing Malaysia Flight 370, hopes have risen. And then, they have fallen. It's seemed like a daily exercise.showed about 300 objects ranging in size from 6 feet (2 meters) to 50 feet (15 meters). When photographed Monday, they were about 125 miles (201 kilometers) away from the spot where a French satellite captured images of a floating group of objects Sunday.";
                c1.volume=c1.name.size();
                insert(c1);
            
            }
        }
        void insert(Currency cur){
            
            string str(cur.toString());
          //  client_->insert(cur.name,str);
        }
        void buyCurrency(Currency& cur)
        {
            insert(cur);
        
        }
        ~PicoHedgeFund(){
            std::cout<<("picoHedgeFund being destructed....\n");
        }
    };

}

#endif

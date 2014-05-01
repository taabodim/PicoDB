//
//  PonocoRunnable.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 5/1/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_PonocoRunnable_h
#define PicoDB_PonocoRunnable_h

namespace  pico  {
      typedef std::shared_ptr<PonocoRunnable> pRunnableType;
    
    class PonocoRunnable { //this is a class that helps bind whatever objects
      //we have to boost thread easily
        DriverType* driverPtr;
    PonocoDriverHelper* driverSyncHelper;
    public:
        PonocoRunnable( DriverType* driverPtr,
                       PonocoDriverHelper* driverSyncHelper) {
            
        }
               virtual void run() =0;
        virtual ~Runnable() {
        }
    };
  
}

#endif

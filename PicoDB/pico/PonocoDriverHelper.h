//
//  PonocoDriverHelper.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 4/30/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_PonocoDriverHelper_h
#define PicoDB_PonocoDriverHelper_h

//the PonocoDriver Class extends this, to get the request
//and response queue..this is the class that wraps
//the mutex , condition variables and queues for ponoco driver
//then by passing one instance of this class to different
//ponocoDriver instances, we can sync them....and have a producer consumer
//pattern
namespace pico {
    class PonocoDriverHelper{
    
    };
}

#endif

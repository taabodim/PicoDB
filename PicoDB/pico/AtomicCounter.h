//
//  AtomicCounter.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 5/31/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_AtomicCounter_h
#define PicoDB_AtomicCounter_h
#include <atomic>
#include <memory>
class AtomicCounter {
    
private:
    
    std::atomic<long> count;
    
public:
    AtomicCounter() {
        count.store(0,std::memory_order_seq_cst);
    }
    void increment()
    {
        count++;
    }
    void decrement()
    {
        count--;
    }
    long get(){
        return count.load();
    }
    void set(long val) {
        count.store(val,std::memory_order_seq_cst);
    }
};



#endif

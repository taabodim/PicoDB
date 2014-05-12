//
//  OffsetManager.h
//  PicoDB
//
//  Created by Mahmoud Taabodi on 5/12/14.
//  Copyright (c) 2014 Mahmoud Taabodi. All rights reserved.
//

#ifndef PicoDB_OffsetManager_h
#define PicoDB_OffsetManager_h
#include <pico/pico_utils.h>
#include <atomic>
namespace pico {

class OffsetManager: public pico_logger_wrapper {
private:

	std::atomic<offsetType> offset(0);
public:

	offsetType getEndOfFileOffset() {

		offsetType x = offset.load(std::memory_order_relaxed);  // get value atomically
		return x;
	}

	void setEndOfFileOffset(offsetType x) {
		offset.store(x, std::memory_order_relaxed);     // set value atomically
	}

};
}

#endif

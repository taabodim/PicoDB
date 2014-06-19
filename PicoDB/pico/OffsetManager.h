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
#include <stdio.h>
#include <assert.h>
namespace pico {

class OffsetManager: public pico_logger_wrapper {
private:

	static std::atomic<offsetType> offset; //it is init to -1
	std::mutex offsetMutex;
	std::string filename;
public:
	OffsetManager(
	//std::shared_ptr<pico_collection> collection)
			std::string filenameArg) {
		filename = filenameArg;
	}
	offsetType getInitialOffset();

	offsetType getEndOfFileOffset() {
		std::unique_lock < std::mutex > offsetChangeLock(offsetMutex);
		offsetType x = offset.load(std::memory_order_relaxed); // get value atomically
		if (x == -1) {
			x = getInitialOffset();
		}
		assert(x >= 0);
		if (mylogger.isTraceEnabled()) {
			mylogger << "OffsetManager : offset is " << x << "\n";
		}

		return x;
	}

	void setEndOfFileOffset(offsetType x) {
		assert(x >= 0);
		std::unique_lock < std::mutex > offsetChangeLock(offsetMutex);
		offset.store(x, std::memory_order_relaxed);     // set value atomically
		if (mylogger.isTraceEnabled()) {
			mylogger << "OffsetManager : offset is set to " << x << "\n";
		}
	}

};
    
    }

#endif

/*
 * SimpleRunnable.h
 *
 *  Created on: Mar 10, 2014
 *      Author: mahmoudtaabodi
 */
#pragma once

namespace threadPool {

class SimpleRunnable {

public:
	void run() {
		for (int i = 1; i < 10; i++) {
			std::cout << "Simple Runnable is running ... by a thread  with id : "
					<< boost::this_thread::get_id() << std::endl;
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
	}

};
}


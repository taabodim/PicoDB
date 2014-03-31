/*
 * Runnable.h
 *
 *  Created on: Mar 10, 2014
 *      Author: mahmoudtaabodi
 */

#include <list>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#ifndef RUNNABLE_H_
#define RUNNABLE_H_

using namespace boost;

namespace threadPool {


template<typename runnableType>
class Runnable {
private:
	int taskId;
	bool taskDone;
	runnableType* runnable;
public:
	Runnable(int id) {
		taskId = id;
	}
	Runnable& Runnbable(Runnable& r) {
		return r;
	}
	Runnable& operator=(Runnable& r) {
		return r;
	}
	bool operator==(Runnable& r) {
		return taskId == r.taskId;
	}
	int getTaskId() {
		return taskId;
	}
	bool isTaskDone() {
		return taskDone;
	}
	void setTaskDone(bool done) {
		taskDone = done;
	}

	void run() {
		runnable->run();
		setTaskDone(true);
	}
	virtual ~Runnable() {
		std::cout << "Runnable object deleted by destructor." << std::endl;
	}
};

} /* namespace threadPool */

#endif /* RUNNABLE_H_ */

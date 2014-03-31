/*
 * ThreadWorker.h
 *
 *  Created on: Mar 10, 2014
 *      Author: mahmoudtaabodi
 */

#pragma once

#include <list>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "Runnable.h"
#include "SimpleRunnable.h"

namespace threadPool {


class ThreadWorker {

private:
	typedef Runnable<SimpleRunnable> taskType;
	int numberOfJobsDoneByTheWorker;
	bool stopFlag_;
	std::list<taskType> queueOfTasks;
	std::size_t WorkerQueueLimit;
	bool free;
	boost::mutex threadwokerMutext;
	boost::thread threadHandle;
	boost::condition_variable threadWorkerQueueIsEmpty;

public:

	void runIndefinitely() {

		while (!stopFlag_) {

			try {

				while (!queueOfTasks.empty()) {

					std::cout << "queueOfTasks size is : "
							<< queueOfTasks.size() << std::endl;

					taskType& task = queueOfTasks.front();
					queueOfTasks.pop_front();

					task.run();
					std::cout << "task was finished by thread worker.."
							<< std::endl;
				}
				if(queueOfTasks.size()>=WorkerQueueLimit)	setIsAvailable(false);
				else setIsAvailable(true);
				boost::unique_lock<boost::mutex> workerLock(threadwokerMutext);
				threadWorkerQueueIsEmpty.wait(workerLock);

			} catch (std::exception& e) {

				std::cout << "thread worker threw  exception..." << e.what()
						<< std::endl;

			} catch (...) {
				std::cout << "thread worker threw unknown exception..."
						<< std::endl;

			}

			std::cout << "thread worker is waiting for task" << std::endl;

		}
	}
	ThreadWorker() {
		stopFlag_ = false;
		free = false;
		WorkerQueueLimit = 5;
	}
	bool isAvailable() {
		return free;
	}
	void setIsAvailable(bool fr) {
		free = fr;
	}
	void start() {
		boost::function<void()> bound_func(
				boost::bind(&ThreadWorker::runIndefinitely, this));
		boost::thread threadHandle(bound_func);
		threadHandle.detach();


	}
	void assignJobToWorker(taskType& task) {

		queueOfTasks.push_front(task);
		threadWorkerQueueIsEmpty.notify_all();
	}

	virtual ~ThreadWorker() {
		stopFlag_ = true;
	}
};


} /* namespace threadPool */


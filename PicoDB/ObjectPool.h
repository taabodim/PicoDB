/*
 * ObjectPoolExample.cpp
 *
 *  Created on: Mar 4, 2014
 *      Author: mahmoudtaabodi
 */

/*Pool example */
#pragma once
#include <boost/thread/mutex.hpp>
#include "boost/interprocess/sync/scoped_lock.hpp"
#include <pico_logger_wrapper.h>
namespace ObjectPool {
class PoolableObject;

boost::mutex PoolMutex;

typedef PoolableObject objectType;
typedef PoolableObject* objectTypePtr;

using namespace std;

class PoolableObject {
public:
	bool isAvailable() {
		return _isAvailable;
	}
	void setAvailable(bool aval) {
		_isAvailable = aval;
	}
	virtual ~PoolableObject() {
	}
protected:
	bool _isAvailable;
};

class MongoConnection: public PoolableObject , public pico_logger_wrapper{
private :
	string name;
	string version;
public:
	MongoConnection() {
		mylogger << "mongo object created by default constructor.\n";
	}
	void connect() {
		mylogger << "mongo connecting..\n";
	}
	void disconnect() {
		mylogger << "mongo disconnecting..\n";
	}
	~MongoConnection() {
		mylogger << "mongo object deleted by  destructor.\n" ;
	}
};
class ObjectFactory {

public:
	void functionToHaveVtable(){}
	virtual objectTypePtr createObject()=0;
	virtual ~ObjectFactory(){}
};

class MongoConnectionObjectFactory: public ObjectFactory , public pico_logger_wrapper{

public:
	MongoConnectionObjectFactory() {
		mylogger
				<< "MongoConnectionObjectFactory object created by default constructor.\n"
				;
	}
	objectTypePtr createObject() {
		return new MongoConnection();
	}
	 ~MongoConnectionObjectFactory() {
		mylogger
				<< "MongoConnectionObjectFactory object deleted by  deconstructor.\n"
				;
	}
};

    class ObjectPool :public pico_logger_wrapper{

public:

	typedef vector<objectTypePtr> objPoolType;

	ObjectPool(ObjectFactory* objFactoryPtr) {
		poolSize = 20;

		for (int i = 0; i < poolSize; i++) {
			PoolableObject* item = objFactoryPtr->createObject();
			_bag.push_back(item);
		}
		mylogger << "pool initialized ....\n" ;
	}
	void insert(PoolableObject* item) {
		_bag.push_back(item);
	}

	objectTypePtr getObject() {
		for (objPoolType::iterator it = _bag.begin(); it != _bag.end(); ++it) {

			objectTypePtr obj = *it;
			boost::interprocess::scoped_lock<boost::mutex> lock(PoolMutex);
			if (obj->isAvailable()) {
				obj->setAvailable(false);
				mylogger << "retrieveing object from pool...\n" ;
				return obj;
			}

		}
		return NULL;
	}

	void returnObjectBackTooPool(objectTypePtr obj) {
		mylogger << "returning object back to the  pool...\n" ;
		boost::interprocess::scoped_lock<boost::mutex> lock(PoolMutex);
		obj->setAvailable(true);
	}

	int size() {
		return _bag.size();
	}

	virtual ~ObjectPool() {
		mylogger << "ObjectPool object deleted by  destructor.\n" ;
	}
private:
	objPoolType _bag;
	int poolSize;

};
    class MongoThread : public pico_logger_wrapper{

private:
    std::shared_ptr<ObjectPool> mongoPool;

public:

	MongoThread(std::shared_ptr<ObjectPool> pool) {
		mongoPool = pool;
	}
    MongoThread(const MongoThread& mThread) {
        assert(false);
    }
	void run() {

		int iter = 10;
		for (int i = 0; i < iter; i++) {

			objectTypePtr obj = mongoPool->getObject();

			MongoConnection* mongoPtr = static_cast<MongoConnection*> (obj);
			mongoPtr->connect();
			boost::this_thread::sleep(boost::posix_time::seconds(1));
			mongoPtr->disconnect();
			mongoPool->returnObjectBackTooPool(obj);
			delete mongoPtr;
			//	myMutex.lock();
			//	for (int i = 0; i < 100; i++) {
			//		cout << " consumer thread started , thread id is : "
			//				<< this_thread::get_id() << endl;
			//		boost::this_thread::yield();
			//	}
			//	myMutex.unlock();
		}
	}
	virtual ~MongoThread(){
		mylogger << "MongoThread object deleted by destructor.\n";
	}
};

void poolTestExample() {

	std::shared_ptr<ObjectFactory> mngFactory(
			new MongoConnectionObjectFactory { });

	std::shared_ptr<ObjectPool> poolPtr { new ObjectPool(mngFactory.get()) };

	MongoThread mThread(poolPtr);

	auto bound_member_fn = std::bind(&MongoThread::run, mThread);

	boost::thread consumerThread1(bound_member_fn);
	boost::thread consumerThread2(bound_member_fn);

	consumerThread1.join();
	consumerThread2.join();
}

}//end of namespace ObjectPool

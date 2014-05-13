#include <cstdlib>
#include <list>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <memory>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/foreach.hpp>
#include <array>
#include <cstring>
#include <iostream>
#include <string>
#include <currency.h>
#include <vector>
#include <list>
#include <chrono>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <boost/thread.hpp>
#include <PonocoRunnable.h>
#include <memory>
#include <utility>
#include <array>

#include <file_test.h>

#include <ctime>
#include <ratio>
#include <chrono>
//#include <boost/archive/text_oarchive.hpp> //these libs are made for 32 bit but my laptop is 64 bit , solve it later
//#include <boost/archive/text_iarchive.hpp>

#include <sstream>
#include <boost/asio.hpp>
#include <functional>
#include <exception>      // std::set_unexpected
#include <stock.h>
#include "ObjectPool.h"
#include "Runnable.h"
#include "ThreadWorker.h"
#include <pico_logger_wrapper.h>
#include <iostream>
#include <SimpleRunnable.h>

#include <PonocoClient.h>
#include <jsonCppExamples.h>
#include <string>
#include <vector>
#include <PicoHedgeFund.h>
#include <logger.h>
#include <chat_server.h>

#include <pico/ThreadPool.h>
#include <pico/pico_test.h>
#include <pico/pico_server.h>
#include <pico/pico_utils.h>
#include <pico/pico_record.h>
#include <pico/pico_collection.h>
#include <pico/pico_client.h>
#include <pico/pico_index.h>
#include <pico/pico_concurrent_list.h>
#include <type_traits>
#include <memory>
#include <cstdio>

class Foo { };
typedef std::shared_ptr<Foo> SharedFoo;

template <class T>
struct getValue {
    getValue() {
        printf("not shared!\n");
    }
};

template <class T>
struct getValue<std::shared_ptr<T> > {
    getValue() {
        printf("shared!\n");
    }
};

int templateSpecialiaztionOfSharedPtrExample()
{
    getValue<SharedFoo>();
    getValue<Foo>();
    return 0;
}
using namespace pico;
using namespace std;

boost::mutex myMutex;


typedef std::vector<std::shared_ptr<Currency>> vectorSharedCurPtr;

namespace std {

template<> struct hash<Currency> { //overriding std:hash for Currency class to use in unoredered set

	std::size_t operator()(Currency const& other) const {
        return other.hash_code();
	
	}
};
}

void runProducerThread();
void runConsumerThread();

void dateExample();



void serilizeClassAndLoadItViaBoost() {
//	std::ofstream file("archive.txt");
//	boost::archive::text_oarchive oa(file);
//	Currency c1;
//	c1.setName("jpy");
//	oa << c1;
	//loading
	//	std::stringstream ssFile("archive.txt");
	//	boost::archive::text_iarchive ia(ssFile);
	//	Currency c2;
	//	ia >> c2;
	//	mylogger << c2.name //<< std::endl;

}

void timerHandler(const boost::system::error_code &ec) {
	//mylogger << " every 5 seconds, this is called ..." //<< std::endl;
}
struct MyException: public std::exception {
	std::string s;
	MyException(std::string ss) :
			s(ss) {
	}
	~MyException() throw () {
	} // Updated
	const char* what() const throw () {
		return s.c_str();
	}
};


/**/

void timerExample() {

	boost::asio::io_service io_service;
	boost::asio::deadline_timer timer(io_service,
			boost::posix_time::seconds(5));
	timer.async_wait(timerHandler);
	io_service.run();
}

class EventHandler {
public:
	void addHandler(std::function<void(int)> callback) {
		cout << "Handler added..." << endl;
		// Let's pretend an event just occured
		callback(1);
	}
};

void myunexpected() {
	std::cerr << "unexpected called\n";
	throw 0; // throws int (in exception-specification)
}int f(int a, int b) {
	return a + b;
}

int g(int a, int b, int c) {
	return a + b + c;
}

template<typename finType>
struct isComplicated {
	bool getTypeComplication() {
		return false;
	}
};
template<>
struct isComplicated<Stock<double>> {
	bool getTypeComplication() {
		return true;
	}
};

template<typename finType>
class financialPackage {
public:
	typedef isComplicated<finType> complicatedFinancialInstrument;
	complicatedFinancialInstrument trait;
	void printValue(finType f) {
		if (trait.getTypeComplication()) {
			std::cout
					<< "this is the special algo for calculating the value of stock : "
					<< f.calPrice() << endl;
		} else {
			std::cout << "this is the value of financial instrument "
					<< f.getValue() << endl;
		}
	}

};


void fBindExample(int n1, int n2, int n3, const int& n4, int n5)
{
   // mylogger << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
}

int g(int n1)
{
    return n1;
}

struct Foo1 {
    void print_sum(int n1, int n2)
    {
      //  mylogger << n1+n2 << '\n';
    }
    int data = 10;
};

struct FooBind {
    FooBind(int num) : num_(num) {}
    void print_add(int i) const {
        //mylogger << num_+i << '\n';
    }
    int num_;
};
int stdBindExamples()
{
//    using namespace std::placeholders;  // for _1, _2, _3...
//    
//    // demonstrates argument reordering and pass-by-reference
//    int n = 7;
//    // (_1 and _2 are from std::placeholders, and represent future
//    // arguments that will be passed to f1)
//  //  auto f1 = std::bind(fBindExample, _2, _1, 42, std::cref(n), n);
//    n = 10;
//    f1(1, 2, 1001); // 1 is bound by _1, 2 is bound by _2, 1001 is unused
//    
//    // nested bind subexpressions share the placeholders
//    auto f2 = std::bind(fBindExample, _3, std::bind(g, _3), _3, 4, 5);
//    f2(10, 11, 12);
//    
//    // common use case: binding a RNG with a distribution
//    std::default_random_engine e;
//    std::uniform_int_distribution<> d(0, 10);
//    std::function<int()> rnd = std::bind(d, e);
//    for(int n=0; n<10; ++n)
//        mylogger << rnd() << ' ';
//    mylogger << '\n';
//    
//    // bind to a member function
//    Foo foo;
//    auto f3 = std::bind(&FooBind::print_add, &foo, 95, _1);
//    f3(5);
//    
//    // bind to member data
//    auto f4 = std::bind(&FooBind::num_, _1);
//    mylogger << f4(foo) << '\n';
    return 1;
}



void print_num(int i)
{
   // mylogger << i << '\n';
}

struct PrintNum {
    void operator()(int i) const
    {
     //   mylogger << i << '\n';
    }
};



int stdFunctionBindingExample()
{
    // store a free function
//    std::function<void(int)> f_display = print_num;
//    f_display(-9);
//    
//    // store a lambda
//    std::function<void()> f_display_42 = []() { print_num(42); };
//    f_display_42();
//    
//    // store the result of a call to std::bind
//    std::function<void()> f_display_31337 = std::bind(print_num, 31337);
//    f_display_31337();
//    
//    // store a call to a member function
//    std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
//    const Foo foo(314159);
//    f_add_display(foo, 1);
//    
//    // store a call to a member function and object
//    using std::placeholders::_1;
//    std::function<void(int)> f_add_display2= std::bind( &Foo::print_add, foo, _1 );
//    f_add_display2(2);
//    
//    // store a call to a function object
//    std::function<void(int)> f_display_obj = PrintNum();
//    f_display_obj(18);
    return 1;
}
//template<>
//class financialPackage<Stock<double>>{
//	public :
//	void printValue(Stock<double> f){
//		mylogger<<("this is the special algo for calculating the value of stock : "<<f.calPrice()<<endl;
//	}
//
//};

void templateSpecializationAndTraitsExample() {
	Currency c1;
	Stock<double> sDouble1(1, 2);
	Stock<int> sInt1(1, 2);
	c1.price = "123123";
	financialPackage<Currency> curPackage;
	curPackage.printValue(c1);
	financialPackage<Stock<double>> stockDoublePackage;
	stockDoublePackage.printValue(sDouble1);

}
void lockExamples() {
//	boost::mutex wokerMutext;
//	boost::unique_lock<boost::mutex> workerLock(wokerMutext);
//
    
//    boost::mutex wokerMutext; //dont ever use scoped_lock it works very bad under multi threading situations
//				boost::interprocess::scoped_lock<boost::mutex> workerLock( wokerMutext);
////syntax exmaples
//			boost::thread thrd(&ThreadWorker::runIndefinitely, &workerPtr);
//			workerType threadWorkerPtr (new boost::thread (&ThreadWorker::runIndefinitely, &workerPtr));
//			workerType threadWorkerPtr (new boost::thread (&ThreadWorker::runIndefinitely, &workerPtr));

}


std::string make_daytime_string() {
	using namespace std;
	// For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}





void create100RecordsInCollection(){
  
   
	pico_collection optionCollection("options-collection");
    
	for (int i = 0; i < 10; i++) {
        string key=random_string(10);
        string value = "valueForThisKey";
        value.append(key);
//        pico_record x1(key, value);
//        cout<<"pico_record is "<<x1.toString()<<std::endl;
//        optionCollection.insert(x1);
	}
    
	cout << " number of records are : " << optionCollection.getNumberOfMessages()
    << " \n";
//	cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
//	cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
    
    optionCollection.index.print_tree();
	
//    optionCollection.update(x1, x2);
//	
    //optionCollection.deleteRecord(x1);
    
	//mylogger <<std::endl<< "end of function create100RecordsInCollection() "//<< std::endl;

}
void test_pico_binary_index_tree()
{
    create100RecordsInCollection();
  }
void createACollection() {

}

void runPicoHedgeFundClient(std::shared_ptr<DriverType> ptr)
{
    cout<<("hedge fund is starting...");
    //PicoHedgeFund hedgefund(ptr);
   // hedgefund.buy(1);
    cout<<("hedge fund finished buying currencies...");
    
}
void runPicoDriver(PonocoDriverHelper* syncHelper) {
	try {
	//	mylogger << "starting PicoDriver" //<< std::endl;
		std::string localhost { "0.0.0.0" };// #Symbolic name meaning all available interfaces
        //localhost{"localhost"} only the local machine via a special interface only visible to programs running on the same compute
		std::string port { "8877" };
        
		boost::asio::io_service io_service;
        tcp::resolver r(io_service);
        
		std::shared_ptr<tcp::socket> socket(new tcp::socket(io_service));
        std::shared_ptr<DriverType> ptr(new DriverType(syncHelper));
		 ptr->start_connect(socket,r.resolve(tcp::resolver::query(localhost, port)));
        //		boost::thread shellThread(
        //				boost::bind(startTheShell, ptr)); //this will run the shell process that reads command and send to client
        //and client sends to server
        
        
//        boost::thread hedgeThred(boost::bind(runPicoHedgeFundClient, ptr));
//        hedgeThred.detach();
        
		io_service.run();
	//	mylogger << "ptr to client going out of scope" //<< std::endl;
     
        
        
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
         raise(SIGABRT);
	} catch (...) {
		std::cerr << "Exception: unknown happened for client" << "\n";
        raise(SIGABRT);
	}
}

struct A {
    A(int&& n) {
        //mylogger << "rvalue overload, n=" << n << "\n";
    }
    A(int& n)  {
        //mylogger << "lvalue overload, n=" << n << "\n";
    }
};

class B {
public:
    template<class T1, class T2, class T3>
    B(T1&& t1, T2&& t2, T3&& t3) :
    a1_{std::forward<T1>(t1)},
    a2_{std::forward<T2>(t2)},
    a3_{std::forward<T3>(t3)}
    {
    }
    
private:
    A a1_, a2_, a3_;
};

template<class T, class U>
std::unique_ptr<T> make_unique1(U&& u)
{
    return std::unique_ptr<T>(new T(std::forward<U>(u)));
}

template<class T, class... U>
std::unique_ptr<T> make_unique(U&&... u)
{
    return std::unique_ptr<T>(new T(std::forward<U>(u)...));
}

void forwarding_example()
{
    auto p1 = make_unique1<A>(2); // rvalue
    int i = 1;
    auto p2 = make_unique1<A>(i); // lvalue
    
 //   mylogger << "B\n";
    auto t = make_unique<B>(2, i, 3);
}

//std::shared_ptr<boost::mutex>  logger::log_mutex (new boost::mutex());//initializing the staic member which is mutext with this syntax



void clientServerExample() {
	try {
       
	
        using namespace pico;

    //     std::shared_ptr<PonocoDriverHelper> sharedSyncHelper  (new PonocoDriverHelper);
         PonocoDriverHelper* sharedSyncHelper  =new PonocoDriverHelper();
std::shared_ptr<DriverType> ptr(new DriverType(sharedSyncHelper));
//         DriverType* ptr = new DriverType(sharedSyncHelper);
        boost::thread serverThread(runServer);
       // sleepViaBoost(1);

        // boost::bind(runPonocoDriver,_1, _2)(*ptr,sharedSyncHelper);
//auto func = std::bind(runPonocoDriver,_1, _2,ptr,sharedSyncHelper);
       
         PonocoRunnable driverThreadRunnable(ptr.get(),sharedSyncHelper);
       // boost::thread picoDriverThread(boost::bind(runPonocoDriver,_1, _2)(ptr,sharedSyncHelper));
       boost::thread  poncoDriverThread(boost::bind(&PonocoRunnable::runPonocoDriver,driverThreadRunnable));
       // sleepViaBoost(1);
        
        //bind(f, _2, _1)(x, y);
        
       // auto func1= std::bind(runPoncoClientProgram,_1, _2,ptr,sharedSyncHelper);
        PonocoRunnable clientThreadRunnable(ptr.get(),sharedSyncHelper);
        boost::thread poncoClientThread(boost::bind(&PonocoRunnable::runPoncoClientProgram,clientThreadRunnable));
       
       
//        poncoClientThread.detach();
//        picoDriverThread.detach();
//        serverThread.detach();
       
        //a thread that is waiting on a condition variable, cannot be joined
        
        if(serverThread.joinable()) {serverThread.join();}
//        if(poncoClientThread.joinable())
//        {poncoClientThread.join();}
//        if(picoDriverThread.joinable())
//        {picoDriverThread.join();}
		      
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
        raise(SIGABRT);

	} catch (...) {
		std::cerr << "Exception: unknown thrown" << "\n";
        raise(SIGABRT);

	}
}
void performanceExample()
{

    using namespace std::chrono;
    
    steady_clock::time_point t1 = steady_clock::now();
    
   // mylogger << "printing out 1000 stars...\n";
 //   for (int i=0; i<1000; ++i) mylogger << "*";
    std::cout << std::endl;
    
    steady_clock::time_point t2 = steady_clock::now();
    
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    
   // mylogger << "It took me " << time_span.count() << " seconds.";
    //mylogger //<< std::endl;
    
}
void dateExample() {
	boost::gregorian::date d(2010, 1, 30);
	//mylogger << "year is " << d.year() //<< std::endl;
//	mylogger << "month is " << d.month() //<< std::endl;
//	mylogger << "day is " << d.day() //<< std::endl;
//	mylogger << "day of week is " << d.day_of_week() //<< std::endl;
//	mylogger << "end of month is " << d.end_of_month() //<< std::endl;

	boost::gregorian::date d1(2008, 1, 31);
	boost::gregorian::date d2(2008, 8, 31);
	boost::gregorian::date_duration dd = d2 - d1;
//	mylogger << "date difference is " << dd.days() //<< std::endl;

	boost::gregorian::date d3(2009, 3, 31);
	boost::gregorian::months ms(1);
	boost::gregorian::date d4 = d3 + ms;
//	mylogger << "adding month to date " << d4 //<< std::endl;
	boost::gregorian::date d5 = d3 - ms;
//	mylogger << "substracting month from date " << d5 //<< std::endl;

	{
		boost::gregorian::date d(2009, 1, 5);
		boost::gregorian::day_iterator it(d);
	//	mylogger << *++it //<< std::endl;
		std::cout
				<< boost::date_time::next_weekday(*it,
						boost::gregorian::greg_weekday(
								boost::date_time::Friday)) << std::endl;
	}
	{
		boost::gregorian::date d1(2009, 1, 30);
		boost::gregorian::date d2(2009, 10, 31);
		boost::gregorian::date_period dp(d1, d2);
	//	mylogger << "date exist in date period " << dp.contains(d1)
				//<< std::endl;
	//	mylogger << "date exist in date period " << dp.contains(d2)
				//<< std::endl;
	}
	{

		boost::gregorian::date d1(2009, 1, 30);
		boost::gregorian::date d2(2009, 10, 31);
		boost::gregorian::date_period dp(d1, d2);
		boost::gregorian::date_duration dd = dp.length();
//		mylogger << "days in a date period " << dd.days() //<< std::endl;
	}
}

void runProducerThread() {
	myMutex.lock();
	for (int i = 0; i < 100; i++) {
		cout << " producer thread started , thread id is : "
				<< boost::this_thread::get_id() << std::endl;
		boost::this_thread::yield();
	}
	myMutex.unlock();
}
void runConsumerThread() {
	myMutex.lock();
	for (int i = 0; i < 100; i++) {
		cout << " consumer thread started , thread id is : "
				<< boost::this_thread::get_id() << std::endl;
		boost::this_thread::yield();
	}
	myMutex.unlock();
}
class STL_Containers_Examples {
public:

	void unordered_mapExamples() {

		Currency c1, c2, c3, c4, c5, c6;
		c1.setName("jpy");
		c2.setName("usa");
		c3.setName("iri");

		unordered_map<string, Currency> mapOfCurrencies { { "jpy", c1 }, {
				"usa", c2 }, { "ir", c3 } };

		for (unordered_map<string, Currency>::iterator it =
				mapOfCurrencies.begin(); it != mapOfCurrencies.end(); ++it) {
			cout << "unordered_map entry : key : " << it->first << " ==> "
					<< " value : " << it->second.toJson() << endl;
		}

	}
	void unordered_multisetExamples() {
		Currency c1, c2, c3, c4, c5, c6;
		c1.setName("jpy");
		c2.setName("usa");
		c3.setName("iri");

		unordered_set<Currency> allCur { c1, c2, c3, c4, c5, c6 };
		unordered_multiset<Currency> multiCur { c1, c2, c3, c4, c5, c6 };
		multiCur.insert(c2);
		multiCur.insert(c3);
		multiCur.insert(c3);
		multiCur.insert(c3);
		multiCur.insert(c3);

		allCur.insert(c2);
		allCur.erase(c1);

		for (unordered_set<Currency>::iterator it = allCur.begin();
				it != allCur.end(); ++it) {
			cout << "unordered  set entry : " << it->toJson() << endl;

		}

		for (unordered_set<Currency>::iterator it = multiCur.begin();
				it != multiCur.end(); ++it) {
			cout << "unordered  multi set entry : " << it->toJson() << endl;

		}
	}

	void multimapExamples() {
		Currency c1, c2, c3;
		multimap<string, Currency> mapOfCurrencies { { "jpy", c1 },
				{ "usa", c2 }, { "ir", c3 } };
		mapOfCurrencies.erase("jpy");

		for (multimap<string, Currency>::iterator it = mapOfCurrencies.begin();
				it != mapOfCurrencies.end(); ++it) {
			cout << "map entry : key : " << it->first << " ==> " << " value : "
					<< it->second.toJson() << endl;
		}

	}
	void multisetExamples() {
		Currency c1, c2;
		multiset<Currency> bag { c1, c2 };
		bag.insert(c1);
		bag.insert(c2);

		bag.erase(c1);

		for (multiset<Currency>::iterator it = bag.begin(); it != bag.end();
				++it) {
			//		Currency c = *it;
			cout << "set iterator ==> " << it->toJson() << endl;
		}
	}
	void listExamples() {
		list<Currency> listOfCurrencies;
		Currency c1;
		Currency c2;
		listOfCurrencies.push_back(c1);
		listOfCurrencies.push_back(c2);

		for (list<Currency>::iterator i = listOfCurrencies.begin();
				i != listOfCurrencies.end(); ++i) {
			cout << "list iterator ==> " << i->toJson() << endl;
		}
		while (!listOfCurrencies.empty()) {
			cout << " list front " << listOfCurrencies.front().toJson() << endl;
			listOfCurrencies.pop_front();

		}
	}
};
void chronoExamples() {

	//	cout << "system clock is " << std::chrono::system_clock() << endl;
	//	cout << "high_resolution_clock is " << std::chrono::high_resolution_clock()
	//			<< endl;
//    mylogger<<("steady_clock  is ",std::chrono::steady_clock());

}

void readCurrencyViaWeakPointer(std::weak_ptr<Currency> w_ptr) {
	auto ptrAfterShared = w_ptr.lock();
	if (ptrAfterShared) {
		cout << " reading via weak ptr " << ptrAfterShared->toJson() << endl;
	} else {
		cout << " can't read via weak ptr " << endl;

	}

}
class Pointer_Examples {
public:
	void uniquePointerExample() {
		string jpyStr = string("jpy");
		string priceStr = string("12.12");
		string volumeStr = string("10000");

		cout << "this is the unique pointer example" << endl;
		Currency c1;
		c1.setName(jpyStr).setPrice(priceStr).setVolume(volumeStr);
		unique_ptr<Currency> uniquePtr1(new Currency);
		cout << "unique pointer : " << uniquePtr1->toJson() << endl;
		//this is how you transfer ownership to another unqiue pointer
		unique_ptr<Currency> uniquePtr2(std::move(uniquePtr1));
		unique_ptr<Currency> uniquePtr3(std::move(uniquePtr2));
		unique_ptr<Currency> uniquePtr4(std::move(uniquePtr3));
		cout << "unique pointer : " << uniquePtr4->toJson() << endl;

	}
	void weakPointerExample() {
		string jpyStr = string("jpy");
		string priceStr = string("12.12");
		string volumeStr = string("10000");

		cout << "this is the weak pointer example" << endl;
		Currency c1;
		c1.setName(jpyStr).setPrice(priceStr).setVolume(volumeStr);
		std::shared_ptr<Currency> cShared2(new Currency(c1));
		cout << "shared pointer " << cShared2->toJson() << endl;
		std::weak_ptr<Currency> weakPtrToJPY = cShared2;

		std::weak_ptr<Currency> w_ptr;
		{

			readCurrencyViaWeakPointer(w_ptr);
			auto ptr = std::make_shared<Currency>();
			w_ptr = ptr;
			std::cout << "w_ptr.use_count() inside scope: " << w_ptr.use_count()
					<< '\n';
			readCurrencyViaWeakPointer(w_ptr);
		}

		std::cout << "w_ptr.use_count() out of scope: " << w_ptr.use_count()
				<< '\n';
	}
	void vectorAndSharedPointerExamples() {

		cout << "this is a big example of what STL can offer us" << endl;
		Currency cdefault { };
		Currency cAssigned = cdefault;
		Currency copy { cdefault };
		Currency movedCurrency { std::move(copy) };
		cout << "copy currency content is " << copy.toString() << endl;
		cout << "movedCurrency currency content is " << movedCurrency.toString()
				<< endl;
		std::shared_ptr<Currency> curPointer(new Currency);
		std::cout << "in someSharedPtr.toJson() ==>  " << curPointer->toJson()
				<< std::endl;

		std::cout << "shared pointer currency content is "
				<< curPointer->toJson() << std::endl;
		std::vector<std::shared_ptr<Currency>> someSharedPtr { };
		cout << "someSharedPtr size is " << someSharedPtr.size() << endl;

		someSharedPtr.push_back(make_shared<Currency>(cdefault));
		someSharedPtr.push_back(curPointer);
		cout << "someSharedPtr size is " << someSharedPtr.size() << endl;

		for (std::vector<std::shared_ptr<Currency>>::iterator it =
				someSharedPtr.begin(); it != someSharedPtr.end(); ++it) {
			cout << "in vector of currency ==>" << *it << endl;
			Currency c = **it;
			cout << "in vector of currency toJson ==>" << c.toJson() << endl;
			cout << "in vector of currency toJson ==>" << c() << endl;

		}

		vector<int> allIntegers;
		allIntegers.push_back(12);
		for (vector<int>::iterator it = allIntegers.begin();
				it != allIntegers.end(); ++it) {
			cout << "in vector ==> " << *it << endl;
		}
		std::shared_ptr<string> pTaabodi(new string("Taabodi"));
		std::shared_ptr<string> pHamid(new string("Hamid"));

		std::vector<std::shared_ptr<string> > allNames;
		allNames.push_back(pTaabodi);
		allNames.push_back(pHamid);
		for (std::vector<std::shared_ptr<string>>::iterator it =
				allNames.begin(); it != allNames.end(); ++it) {
			cout << "in vector of strings ==> pointer to string " << *it
					<< std::endl;
			cout << "in vector of strings content of string pointers " << **it
					<< std::endl;

		}

		cout << "in vector of strings pTaabodi ==> " << *pTaabodi << std::endl;

	}

};
//string toStr(int Number) {
////	string String =
////			static_cast<ostringstream*>(&(ostringstream() << Number))->str();
////	return String;
//}

//----------------------------------------------------------------------
typedef boost::shared_ptr<chat_server> chat_server_ptr;
typedef std::list<chat_server_ptr> chat_server_list;

void runChatServer()
{
    try
    {
        
        std::string port { "8877" };
        boost::asio::io_service io_service;
        
        chat_server_list servers;
        
        using namespace std; // For atoi.
        tcp::endpoint endpoint(tcp::v4(), std::atoi(port.c_str()));
        chat_server_ptr server(new chat_server(io_service, endpoint));
        servers.push_back(server);
        
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
}
void file_example()
{
    file_test fileTest;
    fileTest.write10Records();
    fileTest.updateSomeRecords();
    fileTest.writeInSpecificOffsets();
    fileTest.deleteSomeFile();
    fileTest.deleteSomeRecords();

}
void test_pico_index()
{
    pico_binary_index_tree index;
    index.test_tree();
}



//void bar() { baz(); }
//void foo() { bar(); }

logger* pico_logger_wrapper::myloggerPtr=new logger("gicapods");
void printStackTraceHandler(int sig) {
    void *array[10];
    size_t size;
    
    // get void*'s for all entries on the stack
    size = backtrace(array, 10);
    
    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    
    
    pico_logger_wrapper::myloggerPtr->log("this is the stack trace :\n ");
//    for(int i=0;i<10;i++)
//    {
//    std::string *sp = static_cast<std::string*>(array[i]);
//    // You could use 'sp' directly, or this, which does a copy.
//    std::string s = *sp;
//    // Don't forget to destroy the memory that you've allocated.
//  //  delete sp;
//    
//    pico_logger_wrapper::myloggerPtr->log(s);
//    }
    //realname = abi::__cxa_demangle(e.what(), 0, 0, &status);
    //  std::cout << ti.name() << "\t=> " << realname << "\t: " << status << '\n';
    
    //exit(1);
}

void registerPrintStackHandlerForSignals() {
    
    signal(SIGSEGV, printStackTraceHandler);   // install our handler
    
	signal(SIGHUP, printStackTraceHandler);
	signal(SIGINT, printStackTraceHandler);
    signal(SIGQUIT, printStackTraceHandler);
	signal(SIGILL, printStackTraceHandler);
	signal(SIGTRAP, printStackTraceHandler);
	signal(SIGABRT, printStackTraceHandler);
    signal(SIGFPE, printStackTraceHandler);
	signal(SIGKILL, printStackTraceHandler);
    signal(SIGSEGV, printStackTraceHandler);
    signal(SIGSYS, printStackTraceHandler);
    signal(SIGTERM, printStackTraceHandler);
    
}

std::unique_ptr<ThreadPool>  pico_collection::delete_thread_pool (new ThreadPool(numberOfDeletionThreads));
    
//    

std::string pico_session::logFileName ("session");
std::string request_processor::logFileName ("session");
std::string SimpleRunnable::logFileName ("gicapods");
std::string DeleteTaskRunnable::logFileName ("gicapods");

string pico_test::smallKey0 {"smallKey0"};
string pico_test::smallKey1  ("smallKey1");
string pico_test::smallKey2  ("smallKey2");
string pico_test::smallKey3  ("smallKey3");


string  pico_test::smallValue0  ("smallValue0");
string  pico_test::smallValue1 ("smallValue1");
string  pico_test::smallValue2 ("smallValue2");
string  pico_test::smallValue3 ("smallValue3");

std::string request_processor::insertCommand("insert");
std::string request_processor::deleteCommand("delete");
std::string request_processor::updateCommand("update");
std::string request_processor::findCommand("find");
std::string request_processor::getCommand("get");
std::string request_processor::addUserToDBCommand("adduser");
std::string request_processor::deleteUserToDBCommand("deleteuser");
std::string request_processor::deleteCollectionCommand("deleteCollection");
std::string request_processor::createCollectionCommand("createCollection");
std::string PicoConfig::dataDir("/Users/mahmoudtaabodi/Documents/");
long PicoConfig::defaultTimeoutInSec(10);
std::string pico_record::BEGKEY {"BEGKEY"};
std::string pico_record::CONKEY {"CONKEY"};
std::atomic<offsetType> OffsetManager::offset(-1);

string  pico_test::bigValue0("Families skepticalFamilies of the 239 people who were aboard when the plane disappeared from radar screens early March 8 met Friday with Malaysia Airlines and government officials. They came away unpersuaded that progress was being made.Today, all they said was that they were confident, family representative Steve Wang said. But that really doesn't mean that they have confirmed it. They didn't use the word 'confirm.' So it could be that it's a real lead, but it could also not be. I think that, at the moment, everyone needs to wait for final, confirmed information.That view was echoed by Sarah Bajc, whose partner, Philip Wood, was among the passengers.Every time some official gives one of those absolute statements of 'We're sure it's the pings from the black boxes' or 'We're sure it's in the ocean,' we all crash, she told CNNs New Day.Our feet get knocked out from underneath us. But then it always ends up reversing itself, and they step back from it.She expressed skepticism about the way the investigation has been handled. The fox is very much in charge of the henhouse here, she told New Day. We've got a country leading the investigation who also has the primary liability in the case, and it makes us question every step that's taken.\" More cluesA senior Malaysian government official and another source involved in the investigation divulged new details about the flight to CNN on Thursday, including information about what radar detected, the last words from the cockpit and how high the plane was flying after it went off the grid.Malaysia Airlines Flight 370 disappeared from military radar for about 120 nautical miles after it crossed back over the Malay Peninsula, sources said. Based on available data, this means the plane must have dipped in altitude to between 4,000 and 5,000 feet, sources said.The dip could have been programmed into the computers controlling the plane as an emergency maneuver, said aviation expert David Soucie.The real issue here is it looks like -- more and more -- somebody in the cockpit was directing this plane and directing it away from land,said Peter Goelz, a CNN aviation analyst and former National Transportation Safety Board managing director.And it looks as though they were doing it to avoid any kind of detection.But former U.S. Department of Transportation Inspector General Mary Schiavo was not convinced. She said the reported dip could have occurred in response to a loss of pressure, to reach a level where pressurization was not needed and those aboard the plane would have been able to breathe without oxygen, or to get out of the way of commercial traffic123456endOfMessage");

string  pico_test::bigValue1("Families skepticalFamilies of the 239 people who were aboard when the plane disappeared from ;radar screens early March 8 met Friday with Malaysia Airlines and government officials. They came away unpersuaded that progress was being made.Today, all they said was that they were confident, family representative Steve Wang said. But that really doesn't mean that they have confirmed it.endOfMessage");
int main(int argc, char** argv) {
	try {
        if( remove( "/Users/mahmoudtaabodi/Documents/gicapods.log" ) != 0 )
            perror( "Error deleting file" );
        
        pico_logger_wrapper logger;//just to initialize the static pointer
        registerPrintStackHandlerForSignals();
		std::set_unexpected(myunexpected);
//        test_pico_index();
      //  testThreadPool();
        clientServerExample();
        
       // runChatServer();
        //		readingAndWritingRecordData();
        //		jsonCPPexample() ;
        //		readingAndWritingComplexData();
       // file_example();
     //   templateSpecialiaztionOfSharedPtrExample();
	} catch (const std::exception& e) {
		cout << " exception : " << e.what() << endl;
        raise(SIGABRT);

	}
    catch (...) {
		cout << "<----->unknown exception thrown.<------>";
        raise(SIGABRT);

	}
	return 0;
}
    

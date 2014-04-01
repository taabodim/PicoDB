#include <cstdlib>
#include <list>
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
#include <boost/filesystem/fstream.hpp>
#include <memory>
#include <utility>
#include <array>

//#include <boost/archive/text_oarchive.hpp> //these libs are made for 32 bit but my laptop is 64 bit , solve it later
//#include <boost/archive/text_iarchive.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <functional>
#include <exception>      // std::set_unexpected
#include <stock.h>
#include "ObjectPool.h"
#include "Runnable.h"
#include "ThreadWorker.h"
#include "pico/pico_server.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <pico/pico_buffer.h>
#include <pico/pico_record.h>
#include <pico/pico_collection.h>
#include <jsonCppExamples.h>
#include <string>
#include <vector>
#include <PicoHedgeFund.h>
using namespace boost::filesystem;
using namespace pico;
using namespace std;

boost::mutex myMutex;
const int sizeOfArray = 20;
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
void writeToFileExample() {
	boost::filesystem::path p("text.txt");
	boost::filesystem::ofstream ofs(p);
	ofs << "hello mahmoud taabodi" << std::endl;

}
void getInfoOnDirectory() {
	boost::filesystem::path p("/");

}
void getSizeOfFile() {

	std::cout << "gmon.out  size is " << " " << file_size("gmon.out") << '\n';
}
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
	//	std::cout << c2.name << std::endl;

}

void timerHandler(const boost::system::error_code &ec) {
	std::cout << " every 5 seconds, this is called ..." << std::endl;
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
    std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
}

int g(int n1)
{
    return n1;
}

struct Foo {
    void print_sum(int n1, int n2)
    {
        std::cout << n1+n2 << '\n';
    }
    int data = 10;
};

struct FooBind {
    FooBind(int num) : num_(num) {}
    void print_add(int i) const { std::cout << num_+i << '\n'; }
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
//        std::cout << rnd() << ' ';
//    std::cout << '\n';
//    
//    // bind to a member function
//    Foo foo;
//    auto f3 = std::bind(&FooBind::print_add, &foo, 95, _1);
//    f3(5);
//    
//    // bind to member data
//    auto f4 = std::bind(&FooBind::num_, _1);
//    std::cout << f4(foo) << '\n';
    return 1;
}



void print_num(int i)
{
    std::cout << i << '\n';
}

struct PrintNum {
    void operator()(int i) const
    {
        std::cout << i << '\n';
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
//		std::cout<<"this is the special algo for calculating the value of stock : "<<f.calPrice()<<endl;
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

//				boost::interprocess::scoped_lock<boost::mutex> workerLock( wokerMutext);
//syntax exmaples
	////			boost::thread thrd(&ThreadWorker::runIndefinitely, &workerPtr);
	//			workerType threadWorkerPtr (new boost::thread (&ThreadWorker::runIndefinitely, &workerPtr));
	//			workerType threadWorkerPtr (new boost::thread (&ThreadWorker::runIndefinitely, &workerPtr));

}

void runThreadPool() {

//	threadPool::ThreadPool pool(1);
//	threadPool::ThreadPool::taskType r1(123);

//	, r2<SimpleRunnable>(124), r3<
//			SimpleRunnable>(125), r4<SimpleRunnable>(126), r5<SimpleRunnable>(
//			127), r6<SimpleRunnable>(128);
//
//	pool.submitTask(r1);
////	.submitTask(r2).submitTask(r3).submitTask(r4).submitTask(
////			r5).submitTask(r6);
//	pool.start();

	std::cout << "end of runThreadPool()" << std::endl;

}
std::string make_daytime_string() {
	using namespace std;
	// For time_t, time and ctime;
	time_t now = time(0);
	return ctime(&now);
}

void testThreadPool() {

	std::cout << "Running ten thread" << endl;
	boost::thread threadPoolHandle(runThreadPool);

	threadPoolHandle.join();
	std::cout << "Thread pool is done." << std::endl;

}

void writeBinaryExample() {
//	 streampos size;
//	  char * memblock;
//
//	  ifstream file ("example.bin", ios::in|ios::binary|ios::ate);
//	  if (file.is_open())
//	  {
//	    size = file.tellg();
//	    memblock = new char [size];
//	    file.seekg (0, ios::beg);
//	    file.read (memblock, size);
//	    file.close();
//
//	    cout << "the entire file content is in memory";
//
//	    delete[] memblock;
//	  }
//	  else cout << "Unable to open file";
}

void create100RecordsInCollection(){
  
   
	pico_collection optionCollection("options-collection");
    
	for (int i = 0; i < 10; i++) {
        string key=random_string(10);
        string value = "valueForThisKey";
        value.append(key);
        pico_record x1(key, value);
        cout<<"pico_record is "<<x1.getString()<<std::endl;
        optionCollection.insert(x1);
	}
    
	cout << " number of records are : " << optionCollection.getNumberOfRecords()
    << " \n";
//	cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
//	cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
    
    optionCollection.index_of_collection.printTheTree();
	
//    optionCollection.update(x1, x2);
//	
    //optionCollection.deleteRecord(x1);
    
	std::cout <<std::endl<< "end of function create100RecordsInCollection() "<< std::endl;

}
void test_pico_binary_index_tree()
{
    create100RecordsInCollection();
  }
void createACollection() {

}
void sleepViaBoost(int seconds)
{
boost::this_thread::sleep(boost::posix_time::seconds(seconds));
}
void runPicoHedgeFundClient(std::shared_ptr<clientType> ptr)
{
    cout<<"hedge fund is starting..."<<endl;
    PicoHedgeFund hedgefund(ptr);
    hedgefund.buy(10);
    
}
void runClient() {
	try {
		std::cout << "starting client" << std::endl;
		std::string localhost { "localhost" };
		std::string port { "8877" };
        
		boost::asio::io_service io_service;
        tcp::resolver r(io_service);
        
		socketType socket(new tcp::socket(io_service));
        std::shared_ptr<clientType> ptr(new clientType(socket));
        
		ptr->start_connect(r.resolve(tcp::resolver::query(localhost, port)));
        //		boost::thread shellThread(
        //				boost::bind(startTheShell, ptr)); //this will run the shell process that reads command and send to client
        //and client sends to server
        
        
        boost::thread hedgeThred(boost::bind(runPicoHedgeFundClient, ptr));
        
		io_service.run();
		std::cout << "ptr to client going out of scope" << std::endl;
     
        
        
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	} catch (...) {
		std::cerr << "Exception: unknown happened for client" << "\n";
	}
}

struct A {
    A(int&& n) { std::cout << "rvalue overload, n=" << n << "\n"; }
    A(int& n)  { std::cout << "lvalue overload, n=" << n << "\n"; }
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
    
    std::cout << "B\n";
    auto t = make_unique<B>(2, i, 3);
}
void clientServerExample() {
	try {
		using namespace pico;
		boost::thread serverThread(runServer);
		sleepViaBoost(4);
        
        
        
		boost::thread clientThread(runClient);
        sleepViaBoost(4);
        
        
		serverThread.join();
        clientThread.join();
		      
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	} catch (...) {
		std::cerr << "Exception: unknown thrown" << "\n";
	}
}

void readingAndWritingRecordData() {

	string key1 = "keyfromkey1";
	string value1 = "valuefromvalue1";
	string key2 = "key2";
	string value2 = "value2";
	pico_record x1(key1, value1);

	pico_record x2(key2, value2);
	pico_collection optionCollection("options-collection");

	for (int i = 0; i < 10; i++) {
		optionCollection.insert(x1);
	}
	cout << " number of records are : " << optionCollection.getNumberOfRecords()
			<< " \n";
	cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
	cout << " record 4 : " << optionCollection.get(3).getString() << " \n";
	optionCollection.update(x1, x2);
	//optionCollection.deleteRecord(x1);

	std::cout << "end of function readingAndWritingRecordData() "<< std::endl;

}

void fileExamples() {
	string p = "gmon.out";
	if (exists(p))    // does p actually exist?
			{
		if (is_regular_file(p))        // is p a regular file?
			cout << p << " size is " << file_size(p) << '\n';

		else if (is_directory(p))      // is p a directory?
			cout << p << "is a directory\n";

		else
			cout << p
					<< "exists, but is neither a regular file nor a directory\n";
	} else
		cout << p << "does not exist\n";

}
void binaryFileExample() {
	writeBinaryExample();
}


void dateExample() {
	boost::gregorian::date d(2010, 1, 30);
	std::cout << "year is " << d.year() << std::endl;
	std::cout << "month is " << d.month() << std::endl;
	std::cout << "day is " << d.day() << std::endl;
	std::cout << "day of week is " << d.day_of_week() << std::endl;
	std::cout << "end of month is " << d.end_of_month() << std::endl;

	boost::gregorian::date d1(2008, 1, 31);
	boost::gregorian::date d2(2008, 8, 31);
	boost::gregorian::date_duration dd = d2 - d1;
	std::cout << "date difference is " << dd.days() << std::endl;

	boost::gregorian::date d3(2009, 3, 31);
	boost::gregorian::months ms(1);
	boost::gregorian::date d4 = d3 + ms;
	std::cout << "adding month to date " << d4 << std::endl;
	boost::gregorian::date d5 = d3 - ms;
	std::cout << "substracting month from date " << d5 << std::endl;

	{
		boost::gregorian::date d(2009, 1, 5);
		boost::gregorian::day_iterator it(d);
		std::cout << *++it << std::endl;
		std::cout
				<< boost::date_time::next_weekday(*it,
						boost::gregorian::greg_weekday(
								boost::date_time::Friday)) << std::endl;
	}
	{
		boost::gregorian::date d1(2009, 1, 30);
		boost::gregorian::date d2(2009, 10, 31);
		boost::gregorian::date_period dp(d1, d2);
		std::cout << "date exist in date period " << dp.contains(d1)
				<< std::endl;
		std::cout << "date exist in date period " << dp.contains(d2)
				<< std::endl;
	}
	{

		boost::gregorian::date d1(2009, 1, 30);
		boost::gregorian::date d2(2009, 10, 31);
		boost::gregorian::date_period dp(d1, d2);
		boost::gregorian::date_duration dd = dp.length();
		std::cout << "days in a date period " << dd.days() << std::endl;
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
	// cout<<"steady_clock  is "<<std::chrono::steady_clock()<<endl;

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


int main(int argc, char** argv) {
	try {
        
		std::set_unexpected(myunexpected);
       // test_pico_binary_index_tree();
        clientServerExample();
        //		readingAndWritingRecordData();
        //		jsonCPPexample() ;
        //		readingAndWritingComplexData();
	} catch (const std::exception& e) {
		cout << " exception : " << e.what() << endl;
	} catch (...) {
		cout << "<----->unknown exception thrown.<------>";
	}
	return 0;
}
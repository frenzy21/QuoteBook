#ifndef BOOK2_QUOTEBOOK_H
#define BOOK2_QUOTEBOOK_H

#include "spdlog/spdlog.h"
//#include <map>
//#include <thread>

#include <iostream>
#include <stdexcept>
#include <atomic>





#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>



using namespace boost::interprocess;

//struct SharedData {
//    interprocess_mutex* mutex;
//    int value;
//};

struct MyMutexContainer {
    int* mynum;
    //std::atomic<bool> bboSeqNum{true};
    interprocess_mutex* mutex;
    // Constructor
};


struct MessageData{
    int QuoteSeqNum;
    int BboSeqNum;

};

struct SharedMemoryMap {
  typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager>CharAllocator;
  typedef boost::interprocess::basic_string<char, std::char_traits<char>,CharAllocator>SharedString;
  typedef std::pair<const int, SharedString> ValueType;
  typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager>Allocator;
  typedef boost::unordered_map<int, SharedString, std::hash<int>,std::equal_to<int>, Allocator>MapType;
  typedef vector<SharedString, allocator<SharedString, managed_shared_memory::segment_manager>> ShmemVector;
  typedef std::vector<int, allocator<int, managed_shared_memory::segment_manager>> MyVector;

    typedef allocator<MyMutexContainer, managed_shared_memory::segment_manager> MeAllocator;
    typedef std::vector<MyMutexContainer,MeAllocator> Me;

   // typedef allocator<MySequenceContainer, managed_shared_memory::segment_manager> MeSeqAllocator;
   // typedef std::vector<MySequenceContainer,MeSeqAllocator> MeSeq;

};



class QuoteBook {
private: std::thread bookPrintThread;
            std::thread queuePrintThread;
public:
    std::string Name = "TEST";
    std::string LockName = "";
    std::string SrcName = "";
    std::string StateName = "";
    std::string QueueName = "";

    std::string SessionType = "None";

    int NumLevels = 0;
    int myNumLevels = 5;
    float startPrice=0;
    float myInc=0;
    std::vector<std::string> Srcs = std::vector<std::string>();
    std::vector<MyMutexContainer> myMutexes;

    std::atomic<bool> queueStopFlag{false};



    ////////////////////////////////////////////////////////////

    struct SharedState {
        int rows;
        int cols;
        boost::interprocess::interprocess_mutex mutex;
        int bboSeqNum;
        double startPrice;
        double myInc;
        int bidLevel;
        int offerLevel;
        SharedMemoryMap::MapType *myPidMap;

        void print()
        {
            spdlog::info( " rows = {}; cols= {}; bboSeqNum={}; startprice={}; myinc={} bidlevel={}; offerlevel={}.",rows, cols, bboSeqNum, startPrice, myInc,bidLevel,offerLevel);
        }
    };












    /////////////////////////////////////////////////////////////

    managed_shared_memory shmSrc;
    SharedMemoryMap::ShmemVector *mySrcMap;
    SharedState *myState;
    SharedMemoryMap::MyVector *myVectorBids;
    SharedMemoryMap::MyVector *myVectorOffers;
    managed_shared_memory shm;

    std::unique_ptr<boost::interprocess::message_queue> myMessageQueue;


    QuoteBook(std::string s, bool b, std::string message = "No Message",
              std::vector<std::string> srcs = std::vector<std::string>(),
              int levels = 500, bool clean = false, int QueueLength = 1000,float startprice=1.0,float inc=0.00001) {
        spdlog::info("Welcome to QuoteBook!");
        spdlog::info("Welcome to QuoteBook called with argments {} {}", s, b);
        Name = s;
        SrcName = Name + "_Srcs";
        LockName = Name + "_Book";
        StateName = Name + "_State";
        QueueName = Name + "_Queue";

        NumLevels = levels;
        startPrice=startprice;
        myInc=inc;
        if (clean) {
            spdlog::info("Clearing SharedMemory Region.");
            removeMemorySpace();


        }
        shm = managed_shared_memory(open_or_create, LockName.c_str(), 1024);
        MyMutexContainer *data = shm.find_or_construct<MyMutexContainer>("MyMutexContainer")();
        interprocess_mutex *m = shm.find_or_construct<interprocess_mutex>("ThisMutex")();
        myMessageQueue = std::make_unique<message_queue>(open_or_create, QueueName.c_str(), 256, sizeof(MessageData));

        data->mutex = m;
        spdlog::info("QuoteBook mutex created. Locking during init.");
        m->lock();
        data->mynum++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (b) {
            spdlog::info("QuoteBook Session started as server");
            if (srcs.empty()) {
                spdlog::info("Srcs empty in server start up.");
                throw std::invalid_argument("Srcs not defined.");
            };
            Srcs = srcs;
            QuoteBook_SERVER();
        };
        if (!b) {
            spdlog::info("QuoteBook session started as a client");
            QuoteBook_CLIENT();
        };
        spdlog::info("QuoteBook session pid is {}", getpid());
        addToStateMap(message);
        m->unlock();
        spdlog::info("Init Mutex unlocked.");
        print();
        spdlog::info("Constructor finished.");
    }

    void QuoteBook_CLIENT() {
        spdlog::info("Attaching to existing Shared memory");
        SessionType = "Client";

        spdlog::info("Client mySrcMap being attached. {}", SrcName);
        shmSrc = managed_shared_memory(open_only, SrcName.c_str());
        mySrcMap = shmSrc.find<SharedMemoryMap::ShmemVector>("mySrcMap").first;
        for (const auto &val: *mySrcMap) {
            std::string s = (std::string) val;
            Srcs.emplace_back(s);
        }

        spdlog::info("Client myBook being attached");

        myVectorBids = shmSrc.find<SharedMemoryMap::MyVector>("MyVectorBids").first;
        myVectorOffers = shmSrc.find<SharedMemoryMap::MyVector>("MyVectorOffers").first;

        myState = shmSrc.find<SharedState>("myState").first;
        myState->print();
        NumLevels = myState->rows;
        startPrice=myState->startPrice;
        myInc=myState->myInc;
        myState->myPidMap = shmSrc.find<SharedMemoryMap::MapType>("myPidMap").first;
        myState->print();
        for (int i = 0; i < Srcs.size(); ++i) {
            //interprocess_mutex mutex;
            std::string s = "SharesDataMutex_" + Srcs.at(i);
            std::string si = "SharedInterger_" + Srcs.at(i);

            spdlog::info("Creating the Shared Mutex for Src  {}.", s, i);
            interprocess_mutex *m = shmSrc.find<interprocess_mutex>(s.c_str()).first;
            int *thisint = shmSrc.find<int>(si.c_str()).first;
            MyMutexContainer mutex_container;
            mutex_container.mynum = thisint;
            mutex_container.mutex = m;
            myMutexes.push_back(mutex_container);
            myMutexes.at(i).mutex->lock();
            myMutexes.at(i).mutex->unlock();

            spdlog::info("Finished the Shared Mutex for {}.", Srcs.at(i));
        }
        spdlog::info("Checking/Creating the Mutex list.");

        spdlog::info("Finished Checking the Mutex list.");

        if (!myState) {
            std::cerr << "Could not find struct or map in shared memory.\n";
        }

        spdlog::info("Finished Client Setup for Shared Memory.");
    }

    void QuoteBook_SERVER() {
        SessionType = "Server";
        spdlog::info("Cleaning Shared memory");
        //Clealry Im misisng soemthing here as this seg faults if I dont make this larger.
        int totsize = 65536 + 100000 + 2 * NumLevels * Srcs.size() * sizeof(int);


        shmSrc = managed_shared_memory(create_only, SrcName.c_str(), totsize);

        mySrcMap = shmSrc.construct<SharedMemoryMap::ShmemVector>("mySrcMap")(shmSrc.get_segment_manager());

        std::vector<std::string>::iterator it;
        for (it = Srcs.begin(); it != Srcs.end(); ++it) {
            std::string s = (std::string) *it;
            mySrcMap->emplace_back(s, shmSrc.get_segment_manager());
        }

        spdlog::info("Creating Book Segment.");

        myVectorBids = shmSrc.construct<SharedMemoryMap::MyVector>("MyVectorBids")(
                allocator<int, managed_shared_memory::segment_manager>(
                        shmSrc.get_segment_manager())); // first ctor parameter
        myVectorOffers = shmSrc.construct<SharedMemoryMap::MyVector>("MyVectorOffers")(
                allocator<int, managed_shared_memory::segment_manager>(
                        shmSrc.get_segment_manager())); // first ctor parameter


        spdlog::info("Server myBook being attached and filled.");

        for (int i = 0; i < NumLevels * Srcs.size(); i++) {
            myVectorBids->push_back(-1 * i * 0);
            myVectorOffers->push_back(-1 * i * 0);
        }

        spdlog::info("Creating the State memory.", StateName);

        //////////////////////////////////////////////////////////

        myState = shmSrc.construct<SharedState>("myState")();

        myState->cols = Srcs.size();
        myState->rows = NumLevels;
        myState->bboSeqNum = 0;
        myState->startPrice=startPrice;
        myState->myInc=myInc;
        myState->bidLevel=0;
        myState->offerLevel=NumLevels;

        myState->myPidMap = shmSrc.find_or_construct<SharedMemoryMap::MapType>("myPidMap")(
                SharedMemoryMap::Allocator(shmSrc.get_segment_manager()));
        myState->print();
        for (int i = 0; i < Srcs.size(); ++i) {
            //interprocess_mutex mutex;

            std::string s = "SharesDataMutex_" + Srcs.at(i);
            std::string si = "SharedInterger_" + Srcs.at(i);

            spdlog::info("Creating the Shared Mutex and integer {} {}.", s, i);
            interprocess_mutex *m = shmSrc.construct<interprocess_mutex>(s.c_str())();
            int *thisint = shmSrc.construct<int>(si.c_str())(1 + (i + 1) * (i + 1));
            MyMutexContainer mutex_container;
            mutex_container.mynum = thisint;
            mutex_container.mutex = m;
            myMutexes.push_back(mutex_container);


            spdlog::info("Finished the Shared Mutex for {}.", Srcs.at(i));
        }


        spdlog::info("Mystate Mutex list size.");

        spdlog::info("Finished Server Setup for Shared Memory.");

    }

    QuoteBook() {
        spdlog::info("Welcome to QuoteBook! null constructor.");
        QuoteBook(Name, true);
    }

    ~QuoteBook() {
        spdlog::info("Goodbye to QuoteBook! with {}", SessionType);
        cleanthreads();
        unlockall();
        spdlog::info("Goodbye to QuoteBook!. Message Queue thread joined and closed.");
    }

    void print() {
        spdlog::info("Welcome to Print!");
        spdlog::info("This session is of type {}", SessionType);

        spdlog::info("SrcsMap pointer", (long) mySrcMap);

        spdlog::info("myState has the location {}", (long) myState);

        spdlog::info(" Srcs element = {}", vectorToString(Srcs));
        spdlog::info(" myState pidMap contents size {}", myState->myPidMap->size());

        //spdlog::info(" myState pidMap contents {}",
         //            mapToString(*myState->myPidMap));

        spdlog::info(" myState pidMap contents finished. ");
        // printbook();
    }

    void removeMemorySpace() {
        spdlog::info("Cleaning and removing Shared memory {}", SrcName);
        shared_memory_object::remove(SrcName.c_str());
        shared_memory_object::remove(LockName.c_str());
        message_queue::remove(QueueName.c_str());
        spdlog::info("Cleaned up shared memory");
    }

    int getSizeBid(double price) {

        int total = (float) 0;
        for (int i = 0; i < Srcs.size(); i++) {
           // myMutexes.at(i).mutex->lock();
            total = total + myVectorBids->at((NumLevels * i) + getPosition(price));
           // myMutexes.at(i).mutex->unlock();
        }
        return total;
    }

    int getSizeOffer(double price) {

        int total = 0;
        for (int i = 0; i < Srcs.size(); i++) {
           // myMutexes.at(i).mutex->lock();
            total = total + myVectorOffers->at((NumLevels * i) + getPosition( price));
           // myMutexes.at(i).mutex->unlock();
        }
        return total;
    }

    void printbook() {

        spdlog::info(" Srcs element = {}", vectorToString(Srcs));
        spdlog::info(" Print Book [Bids/level] {} {} {}", myVectorBids->size(),myState->bidLevel,myState->myInc);
        int cnt = 0;


        for (int i = 0; i < Srcs.size(); ++i) {
            for (int j = 0; j < NumLevels; ++j) {

                std::cout << (*myVectorBids)[cnt] << ", ";

                cnt = cnt + 1;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        cnt = 0;
        spdlog::info(" Print Book [Offers/level]{} {} {}", myVectorOffers->size(),myState->offerLevel,myState->myInc);
        for (int i = 0; i < Srcs.size(); ++i) {
            for (int j = 0; j < NumLevels; ++j) {

                std::cout << (*myVectorOffers)[cnt] << ", ";

                cnt = cnt + 1;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

    }

    void clearBook() {
        spdlog::info(" Clearing Book.");
        //We should really look all and unlock all here.
        for (int i = 0; i < NumLevels * Srcs.size(); i++) {
            myVectorBids->at(i) = 0;
            myVectorOffers->at(i) = 0;
        }

    }

    void runPrintBook() {
        for (int i = 0; i < 1000; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            printbook();
        }
    }

    void runPrintBookOnUpdate() {
        spdlog::info(" Starting to read Message Queue");
        MessageData mymessage;
        std::size_t received_size;

        unsigned int priority;


        while (!queueStopFlag.load(std::memory_order_relaxed)) {
            bool received = myMessageQueue->timed_receive(
                    &mymessage,
                    sizeof(mymessage),
                    received_size,
                    priority,
                    boost::posix_time::microsec_clock::universal_time() + boost::posix_time::milliseconds(300)
            );
            if (!received) {
                spdlog::info(" No data Recieved.");
            }

            if (received) {
                spdlog::info(" Data Recieved. {} {} {} ", getBboSeqNum(), mymessage.BboSeqNum, mymessage.QuoteSeqNum);
            }
            // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }


    }

    void sendToMessageQueue(int seq, int myseq) {
        MessageData m;
        //spdlog::info(" nding ");
        int i = addBboSeqNum();
        m.QuoteSeqNum = i;
        m.QuoteSeqNum = i;

        myMessageQueue->try_send(&m, sizeof(m), 0);
        //spdlog::info(" Finished Sending");

    }

    void unlockall() {
        for (int i = 0; i < myMutexes.size(); ++i) {

            spdlog::info(" Unlocking mutex for Src...{}  ", Srcs.at(i));
            myMutexes.at(i).mutex->unlock();

        }

    }

    int addBboSeqNum() {
        scoped_lock<interprocess_mutex> lock(myState->mutex);
        myState->bboSeqNum++;
        //spdlog::info(" Incrementing Bbo seqnum {}",myState->bboSeqNum);
        return myState->bboSeqNum;
    }


    int getBboSeqNum() {
        scoped_lock<interprocess_mutex> lock(myState->mutex);
        return myState->bboSeqNum;
    }

    void cleanthreads() {

        spdlog::info(" Cleaning up threads");
        queueStopFlag.store(true, std::memory_order_relaxed);
        try {
            queuePrintThread.join();
        }
        catch (...) {
            spdlog::info(" Exception caught closing threads. Message Queue may not have been started potentially? ");
        }
        spdlog::info(" Cleaned up threads");
    }

    void BookPrint() {
        spdlog::info(" Starting to print book. ");
        bookPrintThread = std::thread(&QuoteBook::runPrintBook, this);
    }

    void BookPrintOnUpdate() {
        spdlog::info(" Starting to print book from Queue. ");
        queuePrintThread = std::thread(&QuoteBook::runPrintBookOnUpdate, this);
    }

    int getsrcindex(std::string Src) {
        auto it = std::find(Srcs.begin(), Srcs.end(), Src);
        int index = -1;
        if (it != Srcs.end()) {
            index = std::distance(Srcs.begin(), it);
            //  std::cout << "Index of " << Src << " is: " << index << std::endl;
        } else {
            std::cout << "Element not found!" << std::endl;
        }

        return index;
    }

    //We need to do some silly c++ stuff to overlaod these to acocunt for the fact thatthe price iesnt an integer.

    int getPosition(double price) {
       // spdlog::info("getPosition  {} {} {} {}",price,startPrice,(price-startPrice),(price-startPrice)/(myInc));
        return ((int)10*(price-startPrice)/(myInc))/10;

    }

    double getPrice(int position) {
        // spdlog::info("getPosition  {} {} {} {}",price,startPrice,(price-startPrice),(price-startPrice)/(myInc));
        return startPrice+myInc*position;

    }

    void BookAddBid(std::string Src, double price, int size) {
        // spdlog::info( " hello {} ", Src);
        int index = getsrcindex(Src);
        //myState->mutexes->at(index)->lock();
        myMutexes.at(index).mutex->lock();
        myVectorBids->at((NumLevels * index) + getPosition( price)) = size;
        setBidLevel(Src,price, size);
        myMutexes.at(index).mutex->unlock();

        sendToMessageQueue(index, index);
    }

    void BookAddOffer(std::string Src, double price, int size) {
        // spdlog::info( " hello {} ", Src);
        int index = getsrcindex(Src);
        myMutexes.at(index).mutex->lock();
        myVectorOffers->at((NumLevels * index) + getPosition(price)) = size;
        setOfferLevel(Src,price, size);
        myMutexes.at(index).mutex->unlock();
        sendToMessageQueue(index, index);
    }

    void addToStateMap(std::string message) {


        myState->myPidMap->insert(std::make_pair(
                getpid(),
                SharedMemoryMap::SharedString(message, SharedMemoryMap::CharAllocator(shmSrc.get_segment_manager()))));
    }


    // Helper function to convert a vector to a string
    template<typename T>
    std::string vectorToString(const std::vector<T> &vec) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i != vec.size() - 1) {
                oss << ", ";
            }
        }
        oss << "]";
        return oss.str();
    }

    template<typename mapType>
    std::string mapToString(const mapType &m) {
        std::ostringstream oss;
        oss << "{";
        for (auto it = m.begin(); it != m.end(); ++it) {
            oss << it->first << ": " << it->second;
            if (std::next(it) != m.end()) {
                oss << ", ";
            }
        }
        oss << "}";
        return oss.str();
    }

    //We simply store the integer that the best bid is at so we can use it later and not search throught he whole vector
    //The inputs used are just for optimizing later. This function is called after each add on each level.
    void setBidLevel(std::string Src, double price, int size){

        int thistot=0;
        int i=0;
        //spdlog::info("Setting Best Bid level.");
        for(i=NumLevels-1;i>=0;i--)
        {
            // spdlog::info(" Best Bid level. {} {}",i,getPrice(i));
            //getSizeOffer(getPrice(i));
            // spdlog::info("Setting Best Bid level. {} {} {}",i,getPrice(i),getSizeBid(getPrice(i)));
            thistot=getSizeBid(getPrice(i));
            //This line needs updated with that atomic stuff.
            if (thistot>0){
                myState->bidLevel=i;
                   // spdlog::info("Setting the Best Bid level to {} {} {} {}",i,getPrice(i),NumLevels,thistot);
                return;
            };
            thistot=0;

        }
        myState->bidLevel=0;
        return;


    }

    void setOfferLevel(std::string Src, double price, int size){
        int thistot=0;
        int i=0;
        //spdlog::info("Setting Best Offer level.");
        for(i=0;i<NumLevels;i++)
        {
           // spdlog::info(" Best Offer level. {} {}",i,getPrice(i));
            //getSizeOffer(getPrice(i));
           // spdlog::info("Setting Best Offer level. {} {} {}",i,getPrice(i),getSizeOffer(getPrice(i)));
            thistot=getSizeOffer(getPrice(i));
            //spdlog::info(" Best Offer level. {}",i);
            //This line needs updated with that atomic stuff.
            if (thistot>0){
            myState->offerLevel=i;
            //    spdlog::info("Setting the Best offer level to {} {} {} {}",i,getPrice(i),NumLevels,thistot);
            return;
            };
            thistot=0;

        }
        myState->offerLevel=NumLevels;
        return;
    }


    int getBestOfferLevel() {

        return myState->offerLevel;
    }


    int getBestBidLevel() {

        return myState->bidLevel;
    }
    double getBestOffer() {

        return getPrice(getBestOfferLevel()) ;  //myState->offerLevel;
    }


    double getBestBid() {

        return getPrice(getBestBidLevel()); //myState->bidLevel;
    }

};

#endif // BOOK2_QUOTEBOOK_H
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



template <typename K, typename V> class QuoteBook {
public:
    std::string Name = "TEST";
    std::string LockName = "";
    std::string SrcName = "";
    std::string StateName = "";
    std::string QueueName = "";

    std::string SessionType = "None";

    int NumLevels = 0;
    int myNumLevels = 5;
    std::vector <std::string> Srcs = std::vector<std::string>();
    std::vector <MyMutexContainer> myMutexes;
    std::thread bookPrintThread;
    std::thread queuePrintThread;
    std::atomic<bool> queueStopFlag{false};



    ////////////////////////////////////////////////////////////

    struct SharedState {
        int rows;
        int cols;
        boost::interprocess::interprocess_mutex mutex;
        int bboSeqNum;

        SharedMemoryMap::MapType *myPidMap;

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
              std::vector <std::string> srcs = std::vector<std::string>(),
              int levels = 5, bool clean = false,int QueueLength=1000)
              {
        spdlog::info("Welcome to QuoteBook! with {}", typeid(K).name());
        spdlog::info("Welcome to QuoteBook called with argments {} {}", s, b);
        Name = s;
        SrcName = Name + "_Srcs";
        LockName = Name + "_Book";
        StateName = Name + "_State";
        QueueName = Name + "_Queue";

        NumLevels = levels;
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
        addtoStateMap(message);
        m->unlock();
        spdlog::info("Init Mutex unlocked.");
        print();
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
        NumLevels = myState->rows;

        myState->myPidMap = shmSrc.find<SharedMemoryMap::MapType>("myPidMap").first;

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

        int totsize = 65536 + 10000 + 2 * NumLevels * Srcs.size() * sizeof(V);


        shmSrc = managed_shared_memory(create_only, SrcName.c_str(), totsize);

        mySrcMap = shmSrc.construct<SharedMemoryMap::ShmemVector>("mySrcMap")(shmSrc.get_segment_manager());

        std::vector<std::string>::iterator it;
        for (it = Srcs.begin(); it != Srcs.end(); ++it) {
            std::string s = (std::string) * it;
            mySrcMap->emplace_back(s, shmSrc.get_segment_manager());
        }

        spdlog::info("Creating Book Segment.");

        myVectorBids = shmSrc.construct<SharedMemoryMap::MyVector>("MyVectorBids")(
                allocator<int, managed_shared_memory::segment_manager>(
                        shmSrc.get_segment_manager())); // first ctor parameter
        myVectorOffers = shmSrc.construct<SharedMemoryMap::MyVector>("MyVectorOffers")(
                allocator<int, managed_shared_memory::segment_manager>(
                        shmSrc.get_segment_manager())); // first ctor parameter


        spdlog::info("Client myBook being attached and filled.");

        for (int i = 0; i < NumLevels * Srcs.size(); i++) {
            myVectorBids->push_back(-1 * i * 0);
            myVectorOffers->push_back(-1 * i * 0);
        }

        spdlog::info("Creating the State memory.", StateName);

        //////////////////////////////////////////////////////////

        myState = shmSrc.construct<SharedState>("myState")();

        myState->cols = Srcs.size();
        myState->rows = NumLevels;
        myState->bboSeqNum=0;

        myState->myPidMap = shmSrc.find_or_construct<SharedMemoryMap::MapType>("myPidMap")(
                SharedMemoryMap::Allocator(shmSrc.get_segment_manager()));

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
        spdlog::info("Goodbye to QuoteBook! with {} {}", typeid(K).name(), SessionType);
        cleanthreads();
        unlockall();
        spdlog::info("Goodbye to QuoteBook!. Message Queue thread joined and closed.");
    }

    void print() {
        spdlog::info("Welcome to Print! with generic template{}", typeid(K).name());
        spdlog::info("This session is of type {}", SessionType);

        spdlog::info("SrcsMap pointer", (long) mySrcMap);

        spdlog::info("myState has the location {}", (long) myState);

        spdlog::info(" Srcs element = {}", vectorToString(Srcs));
        spdlog::info(" myState pidMap contents size {}", myState->myPidMap->size());

        spdlog::info(" myState pidMap contents {}",
                     mapToString(*myState->myPidMap));

        // printbook();
    }

    void removeMemorySpace() {
        spdlog::info("Cleaning and removing Shared memory {}", SrcName);
        shared_memory_object::remove(SrcName.c_str());
        shared_memory_object::remove(LockName.c_str());
        message_queue::remove(QueueName.c_str());
        spdlog::info("Cleaned up shared memory");
    }

    V getSizeBids(K price) {

        V total=(V)0;
        for(int i=0;i<Srcs.size();i++) {
            myMutexes.at(i).mutex->lock();
            total=total+myVectorBids->at((NumLevels * i) + (int) price);
            myMutexes.at(i).mutex->unlock();
        }
        return (V) total;
    }

    V getSizeOffer(K price) {

        V total=(V)0;
        for(int i=0;i<Srcs.size();i++) {
            myMutexes.at(i).mutex->lock();
            total=total+myVectorOffers->at((NumLevels * i) + (int) price);
            myMutexes.at(i).mutex->unlock();
        }
        return (V) total;
    }

    void printbook() {

        spdlog::info(" Srcs element = {}", vectorToString(Srcs));
        spdlog::info(" Print Book [Bids]{}", myVectorBids->size());
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
        spdlog::info(" Print Book [Offers]{}", myVectorOffers->size());
        for (int i = 0; i < Srcs.size(); ++i) {
            for (int j = 0; j < NumLevels; ++j) {

                std::cout << (*myVectorOffers)[cnt] << ", ";

                cnt = cnt + 1;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

    }

    void clearBook()
    {
        spdlog::info(" Clearing Book");
     //We should really look all and unlock all here.
        for (int i = 0; i < NumLevels * Srcs.size(); i++) {
            myVectorBids->at(i)=0;
            myVectorOffers->at(i)=0;
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
         if( !received){
             spdlog::info(" No data Recieved.");
         }

          if( received){
              spdlog::info(" Data Recieved. {} {} {} ",getBboSeqNum(),mymessage.BboSeqNum,mymessage.QuoteSeqNum);
          }
         // std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }



  }

  void sendToMessageQueue(int seq,int myseq)
  {
        MessageData m;
      //spdlog::info(" nding ");
      int i= addBboSeqNum();
        m.QuoteSeqNum=i;
        m.QuoteSeqNum=i;

        myMessageQueue->try_send(&m,sizeof(m),0);
      //spdlog::info(" Finished Sending");

  }
  void unlockall(){
      for (int i = 0; i <myMutexes.size(); ++i) {

         spdlog::info(" Unlocking mutex for Src...{}  ",Srcs.at(i));
         myMutexes.at(i).mutex->unlock();

      }

  }

  int addBboSeqNum()
  {
      scoped_lock<interprocess_mutex> lock(myState->mutex);
      myState->bboSeqNum++;
      //spdlog::info(" Incrementing Bbo seqnum {}",myState->bboSeqNum);
        return myState->bboSeqNum;
  }


  int getBboSeqNum()
  {
      scoped_lock<interprocess_mutex> lock(myState->mutex);
      return myState->bboSeqNum;
  }

  void cleanthreads(){

        spdlog::info(" Cleaning up threads");
        queueStopFlag.store(true, std::memory_order_relaxed);
        try
        {
            queuePrintThread.join();
        }
        catch(...)
        {
            spdlog::info(" Exception caught closing threads. Message Queue may not have been started potentially? ");
        }
        spdlog::info(" Cleaned up threads");
    }

  void BookPrint() {
    spdlog::info(" Starting to print book. ");
    bookPrintThread=std::thread(&QuoteBook::runPrintBook, this);
  }

  void BookPrintOnUpdate() {
        spdlog::info(" Starting to print book from Queue. ");
        queuePrintThread=std::thread(&QuoteBook::runPrintBookOnUpdate, this);
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

  void BookAddBid(std::string Src, float price, K size) {
    // spdlog::info( " hello {} ", Src);
    int index = getsrcindex(Src);
    //myState->mutexes->at(index)->lock();
    myMutexes.at(index).mutex->lock();
    myVectorBids->at((NumLevels * index) + (int)price) = size;
    myMutexes.at(index).mutex->unlock();
    //myState->mutexes->at(index)->unlock();
      sendToMessageQueue(index,index);
  }
    void BookAddOffer(std::string Src, float price, K size) {
        // spdlog::info( " hello {} ", Src);
        int index = getsrcindex(Src);
        //myState->mutexes->at(index)->lock();
        myMutexes.at(index).mutex->lock();
        myVectorOffers->at((NumLevels * index) + (int)price) = size;
        myMutexes.at(index).mutex->unlock();
        //myState->mutexes->at(index)->unlock();
        sendToMessageQueue(index,index);
    }



  void addtoStateMap(std::string message) {


    myState->myPidMap->insert(std::make_pair(
        getpid(), SharedMemoryMap::SharedString(message, SharedMemoryMap::CharAllocator(shmSrc.get_segment_manager()))));
  }

    V GetLevelBid( K lvl)
    {
      return 1;
    }

    // Helper function to convert a vector to a string
  template <typename T> std::string vectorToString(const std::vector<T> &vec) {
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

  template <typename mapType> std::string mapToString(const mapType &m) {
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
};

#endif // BOOK2_QUOTEBOOK_H

#ifndef BOOK2_QUOTEBOOK_H
#define BOOK2_QUOTEBOOK_H

#include "spdlog/spdlog.h"
//#include <map>
//#include <thread>

#include <iostream>
#include <stdexcept>




#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/unordered_map.hpp>


using namespace boost::interprocess;




struct SharedMemoryMap {
  typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager>CharAllocator;
  typedef boost::interprocess::basic_string<char, std::char_traits<char>,CharAllocator>SharedString;
  typedef std::pair<const int, SharedString> ValueType;
  typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager>Allocator;
  typedef boost::unordered_map<int, SharedString, std::hash<int>,std::equal_to<int>, Allocator>MapType;
  typedef vector<SharedString, allocator<SharedString, managed_shared_memory::segment_manager>> ShmemVector;
  typedef std::vector<int, allocator<int, managed_shared_memory::segment_manager>> MyVector;
};



template <typename K, typename V> class QuoteBook {
public:
  std::string Name = "TEST";
  std::string BookName = "";
  std::string SrcName = "";
  std::string StateName = "";

  std::string SessionType = "None";

  int NumLevels = 0;
  int myNumLevels = 5;
  std::vector<std::string> Srcs = std::vector<std::string>();
  std::thread bookPrintThread;

  ////////////////////////////////////////////////////////////

  struct SharedState {
    int rows;
    int cols;
    SharedMemoryMap::MapType *myPidMap;
    // Constructor
  };

  /////////////////////////////////////////////////////////////

  managed_shared_memory shmSrc;
  SharedMemoryMap::ShmemVector *mySrcMap;
  SharedState *myState;
  SharedMemoryMap::MyVector *myVector;

  QuoteBook(std::string s, bool b, std::string message = "No Message",
            std::vector<std::string> srcs = std::vector<std::string>(),
            int levels = 5) {
    spdlog::info("Welcome to QuoteBook! with {}", typeid(K).name());
    spdlog::info("Welcome to QuoteBook called with argments {} {}", s, b);
    Name = s;
    SrcName = Name + "_Srcs";
    BookName = Name + "_Book";
    StateName = Name + "_State";
    NumLevels = levels;
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
    print();
  }

  void QuoteBook_CLIENT() {
    spdlog::info("Attaching to existing Shared memory");
    SessionType = "Client";
    // managed_shared_memory
    spdlog::info("Client mySrcMap being attached. {}", SrcName);
    shmSrc = managed_shared_memory(open_only, SrcName.c_str());
    mySrcMap = shmSrc.find<SharedMemoryMap::ShmemVector>("mySrcMap").first;
    for (const auto &val : *mySrcMap) {
      std::string s = (std::string)val;
      Srcs.emplace_back(s);
    }

    spdlog::info("Client myBook being attached. {}", BookName);

    spdlog::info("Client myBook being attached. {}", BookName);
    myVector = shmSrc.find<SharedMemoryMap::MyVector>("MyVector").first;

    myState = shmSrc.find<SharedState>("myState").first;
    NumLevels = myState->rows;

    myState->myPidMap = shmSrc.find<SharedMemoryMap::MapType>("myPidMap").first;

    if (!myState) {
      std::cerr << "Could not find struct or map in shared memory.\n";
    }

    spdlog::info("Finished Client Setup for Shared Memory.");
  }

  void QuoteBook_SERVER() {
    SessionType = "Server";
    spdlog::info("Cleaning Shared memory");
    shared_memory_object::remove(Name.c_str());
    spdlog::info("Cleaned up shared memory");
    int totsize = 65536 + 10000 + NumLevels * Srcs.size() * sizeof(V);
    shared_memory_object::remove(SrcName.c_str());

    shmSrc = managed_shared_memory(create_only, SrcName.c_str(), totsize);

    mySrcMap = shmSrc.construct<SharedMemoryMap::ShmemVector>("mySrcMap")(shmSrc.get_segment_manager());

    std::vector<std::string>::iterator it;
    for (it = Srcs.begin(); it != Srcs.end(); ++it) {
      std::string s = (std::string)*it;
      mySrcMap->emplace_back(s, shmSrc.get_segment_manager());
    }

    spdlog::info("Creating Book Segment {}.", BookName);

    myVector = shmSrc.construct<SharedMemoryMap::MyVector>("MyVector")( allocator<int, managed_shared_memory::segment_manager>(shmSrc.get_segment_manager())); // first ctor parameter

    spdlog::info("Client myBook being attached and filled. {}", BookName);

    for (int i = 0; i < NumLevels * Srcs.size(); i++) {
      myVector->push_back(-1 * i * 0);
    }

    spdlog::info("Creating the State memory.", StateName);

    //////////////////////////////////////////////////////////

    myState = shmSrc.construct<SharedState>("myState")();

    myState->cols = Srcs.size();
    myState->rows = NumLevels;

    myState->myPidMap = shmSrc.find_or_construct<SharedMemoryMap::MapType>("myPidMap")(SharedMemoryMap::Allocator(shmSrc.get_segment_manager()));
    // myState->myPidMap=thatmap;

    spdlog::info("Finished Server Setup for Shared Memory.");

  }

  QuoteBook() {
    spdlog::info("Welcome to QuoteBook! null constructor.");
    QuoteBook(Name, true);
  }

  ~QuoteBook() {
    spdlog::info("Goodbye to QuoteBook! with {} {}", typeid(K).name(),
                 SessionType);
  }

  void print() {
    spdlog::info("Welcome to Print! with generic template{}", typeid(K).name());
    spdlog::info("This session is of type {}", SessionType);

    spdlog::info("SrcsMap pointer", (long)mySrcMap);

    spdlog::info("myState has the location {}", (long)myState);

    spdlog::info(" Srcs element = {}", vectorToString(Srcs));
    spdlog::info(" myState pidMap contents size {}", myState->myPidMap->size());

    spdlog::info(" myState pidMap contents {}",
                 mapToString(*myState->myPidMap));

    printbook();
  }

  void printbook() {
    spdlog::info(" Print Book {}", myVector->size());
    int cnt = 0;
    spdlog::info(" Srcs element = {}", vectorToString(Srcs));

    for (int i = 0; i < Srcs.size(); ++i) {
      for (int j = 0; j < NumLevels; ++j) {
        // spdlog::info(" Elelement ({},{})  is {}", i, j, (*myVector)[cnt]);

        std::cout << (*myVector)[cnt] << ", ";

        cnt = cnt + 1;
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;

    //}
  }

  void runPrintBook() {
    for (int i = 0; i < 1000; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      printbook();
    }
  }

  void BookPrint() {
    spdlog::info(" Starting to print book. ");
    bookPrintThread(&QuoteBook::runPrintBook, this);
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

  void BooKAdd(std::string Src, float price, K size) {
    // spdlog::info( " hello {} ", Src);
    int index = getsrcindex(Src);

    myVector->at((NumLevels * index) + (int)price) = size;
  }

  void addtoStateMap(std::string message) {


    myState->myPidMap->insert(std::make_pair(
        getpid(), SharedMemoryMap::SharedString(message, SharedMemoryMap::CharAllocator(shmSrc.get_segment_manager()))));
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

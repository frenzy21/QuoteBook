//
// Created by balmoral on 26/05/25.
//

#ifndef BOOK2_QUOTEBOOK_H
#define BOOK2_QUOTEBOOK_H
#include "spdlog/spdlog.h"
#include <map>
#include <array>
#include <random>
#include <thread>





#include <typeinfo>
#include <stdexcept>
#include <sstream>
#include <string>
#include <unistd.h>
#include <iostream>
#include <utility> // for std::pair




#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/unordered_map.hpp>

#include <iostream>

using namespace boost::interprocess;

typedef int KeyType;

// What i want is an Array in memory. ( or actually an Array of arrays. )

struct SharedMemoryMap
{
    typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
    typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> SharedString;
    typedef std::pair<const int, SharedString> ValueType;
    typedef boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager> Allocator;
    typedef boost::unordered_map<int, SharedString, std::hash<int>, std::equal_to<int>, Allocator> MapType;
};



template <typename K,typename V >
class QuoteBook {
public:
    std::string Name = "TEST";
    std::string BookName = "";
    std::string SrcName = "";
    std::string StateName = "";

    std::string SessionType = "None";

    int NumLevels=0;
    int  myNumLevels=5;
    std::vector<std::string> Srcs = std::vector<std::string>();
    std::thread bookPrintThread;





    //Items related to keeping Map in memory
    typedef managed_shared_memory::segment_manager SegmentManager;
    typedef allocator<std::pair<const K, V>, SegmentManager> ShmAllocator;
    typedef map<K, V, std::less<K>, ShmAllocator> ShmMap;

    typedef allocator<char, managed_shared_memory::segment_manager> StringAllocator;
    typedef basic_string<char, std::char_traits<char>, StringAllocator> ShmemString;
    typedef allocator<ShmemString, managed_shared_memory::segment_manager> VectorAllocator;
    typedef vector<ShmemString, VectorAllocator> ShmemVector;

    typedef allocator<int, managed_shared_memory::segment_manager> SharedAllocator;
    typedef std::vector<int, SharedAllocator> SharedMatrix;

    typedef allocator<int, managed_shared_memory::segment_manager>ShmemAllocator;
    typedef std::vector<int, ShmemAllocator> MyVector;

////////////////////////////////////////////////////////////

    typedef managed_shared_memory::segment_manager StateSegmentManager;
    typedef allocator<void, StateSegmentManager> VoidAllocator;

// Define key and value types
    typedef allocator<char, StateSegmentManager> CharAllocator;
    typedef basic_string<char, std::char_traits<char>, CharAllocator> SharedString;

    typedef std::pair<const int, SharedString> MapValueType;
    typedef allocator<MapValueType, StateSegmentManager> MapAllocator;
    typedef boost::interprocess::map<int,SharedString, std::less<int>, MapAllocator> SharedMap;

// Define struct with a map inside
    struct SharedState {
        int rows;
        int cols;
        SharedMemoryMap::MapType *myPidMap;
        // Constructor

    };


    struct QuoteState {
        int rows=0;
        int cols=0;


    };


/////////////////////////////////////////////////////////////


    //managed_shared_memory *shm;
    ShmMap *myMap;
    managed_shared_memory shm;

    ShmemVector *mySrcMap;
    managed_shared_memory shmSrc;

    SharedState *myState;
    managed_shared_memory shmState;
    SharedMemoryMap::MapType *thatmap;

    MyVector* myVector;
    managed_shared_memory shmVec;



    QuoteBook(std::string s, bool b, std::string message="No Message",std::vector<std::string> srcs = std::vector<std::string>(),int levels=5 ) {
        spdlog::info("Welcome to QuoteBook! with {}", typeid(K).name());
        spdlog::info("Welcome to QuoteBook called with argments {} {}", s, b);
        Name = s;
        SrcName = Name + "_Srcs";
        BookName = Name + "_Book";
        StateName = Name + "_State";
        NumLevels=levels;
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
        spdlog::info("QuoteBook session pid is {}",getpid());
        addtoStateMap(message);
        // spdlog::info("MyState list   {}",myState->attachList.size());
       //myState->myPidMap->insert_or_assign((int)getpid(),SharedMemoryMap::SharedString(message, stringAllocator));
       print();
    }

    void QuoteBook_CLIENT() {
        spdlog::info("Attaching to existing Shared memory");
        SessionType = "Client";
        //managed_shared_memory
        spdlog::info("Client mySrcMap being attached. {}", SrcName);
        shmSrc = managed_shared_memory(open_only, SrcName.c_str());
        mySrcMap = shmSrc.find<ShmemVector>("mySrcMap").first;
        for (const auto &val: *mySrcMap) {
            std::string s = (std::string) val;
            Srcs.emplace_back(s);
        }

        spdlog::info("Client myBook being attached. {}", BookName);

        shmVec=managed_shared_memory(open_only, BookName.c_str());

        spdlog::info("Client myBook being attached. {}", BookName);
        // Find the matrix
        //
        myVector= shmVec.find<MyVector>("MyVector").first;


        shmState = managed_shared_memory(open_only, StateName.c_str());
        myState = shmState.find<SharedState>("myState").first;
        NumLevels=myState->rows;
        thatmap = shmState.find<SharedMemoryMap::MapType>("thatMap").first;
        myState->myPidMap=thatmap;
        if (!myState ) {
            std::cerr << "Could not find struct or map in shared memory.\n";

        }


        spdlog::info("Finished Client Setup for Shared Memory.");



    }

    void QuoteBook_SERVER() {
        SessionType = "Server";
        spdlog::info("Cleaning Shared memory");
        shared_memory_object::remove(Name.c_str());
        spdlog::info("Cleaned up shared memory");

        shared_memory_object::remove(SrcName.c_str());

        shmSrc = managed_shared_memory(create_only, SrcName.c_str(), 65536);

        mySrcMap = shmSrc.construct<ShmemVector>("mySrcMap")(shmSrc.get_segment_manager());


        std::vector<std::string>::iterator it;
        for (it = Srcs.begin(); it != Srcs.end(); ++it) {
            std::string s = (std::string) *it;
            mySrcMap->emplace_back(s, shmSrc.get_segment_manager());
        }


        // Define matrix size at runtime

        spdlog::info("Creating Book Segment {}.", BookName);


        shared_memory_object::remove(BookName.c_str());
        spdlog::info("Allocated memory {}",10000+NumLevels * Srcs.size() * sizeof(V));

        shmVec=managed_shared_memory(create_only, BookName.c_str() ,10000+NumLevels * Srcs.size() * sizeof(V));
        spdlog::info("Allocated memory");
        const ShmemAllocator alloc_inst(shmVec.get_segment_manager());
        myVector =shmVec.construct<MyVector>("MyVector")(alloc_inst); //first ctor parameter

        spdlog::info("Client myBook being attached and filled. {}", BookName);

        //myState=shmState.construct<State>("myState")();
        for(int i=0;i<NumLevels * Srcs.size();i++)
        {
            myVector->push_back(-1*i*0);

        }

        spdlog::info("Creating the State memory.", StateName);

        //////////////////////////////////////////////////////////

        shared_memory_object::remove(StateName.c_str());

        shmState = managed_shared_memory(create_only, StateName.c_str(), 65536);
        //VoidAllocator voidAlloc(shmState.get_segment_manager());
        //MapAllocator mapAlloc(shmState.get_segment_manager());
        myState = shmState.construct<SharedState>("myState")();

        myState->cols=Srcs.size();
        myState->rows=NumLevels;

        SharedMemoryMap::Allocator allocator(shmState.get_segment_manager());
        //SharedMemoryMap::MapType *
        thatmap = shmState.find_or_construct<SharedMemoryMap::MapType>("thatMap")(allocator);
        myState->myPidMap=thatmap;

        for (const auto &pair : *thatmap)
        {
            std::cout << pair.first << " -> " << pair.second << std::endl;
        }
        /////////////////////////////////////////////////////////

        spdlog::info("Finished Server Setup for Shared Memory.");

    }

    QuoteBook() {
        spdlog::info("Welcome to QuoteBook! null constructor.");
        QuoteBook(Name, true);
    }

    ~QuoteBook() { spdlog::info("Goodbye to QuoteBook! with {} {}", typeid(K).name(), SessionType); }

    void print() {
        spdlog::info("Welcome to Print! with generic template{}", typeid(K).name());
        spdlog::info("This session is of type {}", SessionType);

        spdlog::info("SrcsMap pointer", (long)mySrcMap);

        //spdlog::info("This session has {} levels.", myBook->rows);
        //spdlog::info("This session has {} Levels  and {} Srcs.", myBook->rows, Srcs.size());

        spdlog::info("myState has the location {}", (long)myState);

        spdlog::info(" Srcs element = {}", vectorToString(Srcs));
        spdlog::info(" myState pidMap contents size {}", myState->myPidMap->size());



       spdlog::info(" myState pidMap contents {}", mapToString( *myState->myPidMap));

        printbook();

    }

    void printbook() {
        spdlog::info(" Print Book {}",myVector->size());
        int cnt=0;
        spdlog::info(" Srcs element = {}", vectorToString(Srcs));


        for (int i = 0; i < Srcs.size(); ++i) {
            for (int j = 0; j < NumLevels; ++j) {
                //spdlog::info(" Elelement ({},{})  is {}", i, j, (*myVector)[cnt]);

                std::cout << (*myVector)[cnt]<<", ";

                cnt=cnt+1;
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
    void BookPrint(){
        spdlog::info(" Starting to print book. ");
        bookPrintThread(&QuoteBook::runPrintBook, this);

    }

    int getsrcindex(std::string Src){
        auto it = std::find(Srcs.begin(), Srcs.end(), Src);
        int index=-1;
        if (it != Srcs.end()) {
            index = std::distance(Srcs.begin(), it);
          //  std::cout << "Index of " << Src << " is: " << index << std::endl;
        } else {
            std::cout << "Element not found!" << std::endl;
        }


    return index;
    }

    void BooKAdd(std::string Src,float price,K size){
        spdlog::info( " hello {} ", Src);
        int index= getsrcindex(Src);

        spdlog::info( " hello {} {} {} {}",Src,index,price,(NumLevels*index)+(int)price);
        myVector->at((NumLevels*index)+(int)price)=size;
        spdlog::info( " hello  finished index ");
    }

    void addtoStateMap(std::string message) {

        SharedMemoryMap::Allocator allocator(shmState.get_segment_manager());
        SharedMemoryMap::CharAllocator stringAllocator(shmState.get_segment_manager());


        myState->myPidMap->insert(std::make_pair(getpid(), SharedMemoryMap::SharedString(message, stringAllocator)));
        //myState->myMap->insert(std::make_pair(200,key2));



    }


    // Helper function to convert a vector to a string
    template <typename T>
    std::string vectorToString(const std::vector<T>& vec) {
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
    std::string mapToString(const mapType& m) {
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


#endif //BOOK2_QUOTEBOOK_H

//
// Created by balmoral on 26/05/25.
//

#ifndef BOOK2_QUOTEBOOK_H
#define BOOK2_QUOTEBOOK_H
#include "spdlog/spdlog.h"
#include <map>
#include <array>


#include <typeinfo>
#include <stdexcept>
#include <sstream>
#include <string>



#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>

#include <iostream>

using namespace boost::interprocess;

typedef int KeyType;

// What i want is an Array in memory. ( or actually an Array of arrays. )




template <typename K,typename V >
class QuoteBook {
public:
    std::string Name = "TEST";
    std::string BookName = "";
    std::string SrcName = "";
    std::string SessionType = "None";
    int NumLevels = 5;
    std::vector<std::string> Srcs = std::vector<std::string>();
    struct Book {
        int rows, cols;
    };
    //struct myBook {
    //   int rows, cols;
    //  int* data;  // Pointer to dynamically allocated matrix
    //};







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






    //managed_shared_memory *shm;
    ShmMap *myMap;
    managed_shared_memory shm;

    ShmemVector *mySrcMap;
    managed_shared_memory shmSrc;

    Book *myBook;
    managed_shared_memory shmBook;

    MyVector* myVector;
    managed_shared_memory shmVec;


    QuoteBook(std::string s, bool b, std::vector<std::string> srcs = std::vector<std::string>()) {
        spdlog::info("Welcome to QuoteBook! with {}", typeid(K).name());
        spdlog::info("Welcome to QuoteBook called with argments {} {}", s, b);
        Name = s;
        SrcName = Name + "_Srcs";
        BookName = Name + "_Book";
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

        shmVec=managed_shared_memory(open_only, "VectorMemory");

        spdlog::info("Client myBook being attached. {}", BookName);
        // Find the matrix
        //
        myVector= shmVec.find<MyVector>("MyVector").first;

        print();
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


        shared_memory_object::remove("VectorMemory");
        spdlog::info("Allocated memory {}",NumLevels * Srcs.size() * sizeof(V));
        shmVec=managed_shared_memory(create_only, "VectorMemory" ,1010000);
        spdlog::info("Allocated memory");
        const ShmemAllocator alloc_inst(shmVec.get_segment_manager());
        myVector =shmVec.construct<MyVector>("MyVector")(alloc_inst); //first ctor parameter

        for(int i=0;i<NumLevels * Srcs.size();i++)
        {
            spdlog::info("Adding");
            myVector->push_back(-1*i*0);
            //spdlog::info("Added {}",(*myVector)[i]);

        }
        print();
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

        spdlog::info("myBook has the location {}", (long)myBook);



        std::vector<std::string>::iterator it;
        for (it = Srcs.begin(); it != Srcs.end(); ++it) {

            std::string s = (std::string) *it;
           //spdlog::info(" Srcs element = {}", s);
           //spdlog::info(" Srcs element = {}", s);
        }
        spdlog::info(" Srcs element = {}", vectorToString(Srcs));

        printbook();

    }

    void printbook() {
        spdlog::info(" Print Book {}",myVector->size());
        int cnt=0;
        spdlog::info(" Srcs element = {}", vectorToString(Srcs));
        for (int i = 0; i < NumLevels; ++i) {
            for (int j = 0; j < Srcs.size(); ++j) {
                //spdlog::info(" Elelement ({},{})  is {}", i, j, (*myVector)[cnt]);

                std::cout << (*myVector)[cnt]<<", ";

                cnt=cnt+1;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        //}
    }

    int getsrcindex(std::string Src){
        auto it = std::find(Srcs.begin(), Srcs.end(), Src);
        int index=-1;
        if (it != Srcs.end()) {
            index = std::distance(Srcs.begin(), it);
            std::cout << "Index of " << Src << " is: " << index << std::endl;
        } else {
            std::cout << "Element not found!" << std::endl;
        }


    return index;
    }

    void BooKAdd(std::string Src,float price,K size){
        int index= getsrcindex(Src);

        myVector->at((NumLevels*index)+(int)price)=size;

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






};


#endif //BOOK2_QUOTEBOOK_H

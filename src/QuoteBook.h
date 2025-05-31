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
    std::string Name="TEST";
    std::string BookName="";
    std::string SrcName="";
    std::string SessionType="None";
    int NumLevels=10000;
    std::vector<std::string> Srcs=std::vector<std::string>();
    struct Book {
        int rows, cols;
        V* data;  // Pointer to dynamically allocated matrix
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





    QuoteBook(std::string s,bool b,std::vector<std::string> srcs = std::vector<std::string>()){
            spdlog::info("Welcome to QuoteBook! with {}",typeid(K).name());
            spdlog::info("Welcome to QuoteBook called with argments {} {}",s,b);
            Name=s;
            SrcName=Name+"_Srcs";
            BookName=Name+"_Book";
            if( b){spdlog::info("QuoteBook Session started as server");
            if(srcs.empty()){spdlog::info("Srcs empty in server start up.");throw std::invalid_argument("Srcs not defined.");};
               Srcs=srcs;
               QuoteBook_SERVER();

            };
            if( !b){spdlog::info("QuoteBook session started as a client"); QuoteBook_CLIENT();};


    }
    void QuoteBook_CLIENT() {
        spdlog::info("Attaching to existing Shared memory");
        SessionType="Client";
        //managed_shared_memory
        spdlog::info("Client mySrcMap being attached. {}",SrcName);
        shmSrc=managed_shared_memory(open_only, SrcName.c_str());
        mySrcMap = shmSrc.find<ShmemVector>("mySrcMap").first;
        //std::memcpy(&Srcs, mySrcMap, sizeof(mySrcMap));
        for (const auto &val : *mySrcMap) {
            std::string s=(std::string)val;
            Srcs.emplace_back(s);
        }

        spdlog::info("Client myBook being attached. {}",BookName);

        shmBook=managed_shared_memory(open_only, SrcName.c_str());

        // Find the matrix
        myBook = shmBook.find<Book>("Matrix").first;



        print();
        spdlog::info("Client set up finished. {}",(long)mySrcMap);
    }

    void QuoteBook_SERVER() {
        SessionType="Server";
        spdlog::info("Cleaning Shared memory");
        shared_memory_object::remove(Name.c_str());
        spdlog::info("Cleaned up shared memory");
        spdlog::info("Created a new segment.");




        spdlog::info("Created a new segment for the Book vector{}", (long) shm.get_address());



        shared_memory_object::remove(SrcName.c_str());

        shmSrc=managed_shared_memory(create_only, SrcName.c_str(), 65536);

        mySrcMap = shmSrc.construct<ShmemVector>("mySrcMap")(shmSrc.get_segment_manager());


        std::vector<std::string>::iterator it;
        for (it = Srcs.begin(); it != Srcs.end(); ++it) {
            std::string s=(std::string)*it;
            mySrcMap->emplace_back(s, shmSrc.get_segment_manager());
       }
        spdlog::info("Creating Book Segment {}.",BookName);



        shared_memory_object::remove(BookName.c_str());

        // Define matrix size at runtime

        spdlog::info("Creating Book Segment {}.",BookName);
        // Create shared memory segment
        shmBook=managed_shared_memory(create_only, BookName.c_str(), 2*NumLevels * Srcs.size() * sizeof(V));
        // Allocate memory for the matrix in shared memory
        myBook = shmBook.construct<Book>("Matrix")();
        myBook->rows = NumLevels;
        myBook->cols = Srcs.size();
        myBook->data = static_cast<V*>(shmBook.allocate(NumLevels * Srcs.size() * sizeof(V)));
        // Initialize myBook
        for (int i = 0; i < NumLevels * Srcs.size() ; ++i) {
            myBook->data[i] = (V)i;
        }
        print();
        spdlog::info("Finished Server Setup for Shared Memory.");

    }
    QuoteBook() {spdlog::info("Welcome to QuoteBook! null constructor.");QuoteBook(Name,true);}
    ~QuoteBook() {spdlog::info("Goodbye to QuoteBook! with {} {}",typeid(K).name(),SessionType);}

    void print(){
        spdlog::info("Welcome to Print! with generic template{}",typeid(K).name());
        spdlog::info("Thi session is of type {}",SessionType);


        for (const auto &val : *mySrcMap) {
            spdlog::info(" MySrcMap element = {}", val);
            //Srcs.emplace_back("weee");
        }
        spdlog::info(" Srcs element = {}", "");
        std::vector<std::string>::iterator it;
        for (it = Srcs.begin(); it != Srcs.end(); ++it) {

            std::string s=(std::string)*it;
            spdlog::info(" Srcs element = {}", s);

        }

        }

     void   printbook() {
            //for (int i = 0; i < myBook->rows; ++i) {
             //   for (int j = 0; j < myBook->cols; ++j) {
              //      std::cout << myBook->data[i * myBook->cols + j] << " ";
              //  }
                std::cout << std::endl;


            }
    }

    V MyMapGet(K key){
        //managed_shared_memory  shm3=managed_shared_memory (open_only, Name.c_str());
        //myMap = shm.find<ShmMap>("MyMap").first;

    }
   void MyMapSet(K key,V val){
       // spdlog::info("Setting Item {} {}",key,-10*val);
       //std::pair<K,V> myPair(key,val);


    }
public:
    //managed_shared_memory *shm;
    ShmMap *myMap;
    managed_shared_memory shm;

    ShmemVector *mySrcMap;
    managed_shared_memory shmSrc;

    Book *myBook;
    managed_shared_memory shmBook;


};


#endif //BOOK2_QUOTEBOOK_H

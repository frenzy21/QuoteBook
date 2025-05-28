//
// Created by balmoral on 26/05/25.
//

#ifndef BOOK2_QUOTEBOOK_H
#define BOOK2_QUOTEBOOK_H
#include "spdlog/spdlog.h"
#include <map>
#include <typeinfo>
#include <stdexcept>



#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <iostream>

using namespace boost::interprocess;

typedef int KeyType;






template <typename K,typename V >
class QuoteBook {
public:
    std::string Name="TEST";

    typedef managed_shared_memory::segment_manager SegmentManager;
    typedef allocator<std::pair<const K, V>, SegmentManager> ShmAllocator;
    typedef map<K, V, std::less<K>, ShmAllocator> ShmMap;
    QuoteBook(std::string s,bool b){
            spdlog::info("Welcome to QuoteBook! with {}",typeid(K).name());
            spdlog::info("Welcome to QuoteBook called with argments {} {}",s,b);
            Name=s;
            if( b){spdlog::info("QuoteBook Session started as server"); QuoteBook_SERVER();};
            if( !b){spdlog::info("QuoteBook session started as a client"); QuoteBook_CLIENT();};


    }
    void QuoteBook_CLIENT() {
        spdlog::info("Attaching to existing Shared memory");
        //managed_shared_memory
        shm=managed_shared_memory(open_only, Name.c_str());
        //shm=&myshm;
        //myMap = shm.find<ShmMap>("SharedMap");
        myMap = shm.find<ShmMap>("MyMap").first;
        spdlog::info("Client set up finished.");
    }

    void QuoteBook_SERVER() {
        spdlog::info("Cleaning Shared memory");
        shared_memory_object::remove(Name.c_str());
        spdlog::info("Cleaned up shared memory");
        shm=managed_shared_memory(create_only, Name.c_str(), 10000*(sizeof(K)+sizeof(V)));
        spdlog::info("Created a new segment.");

         myMap = shm.construct<ShmMap>("MyMap")(std::less<K>(), shm.get_segment_manager());
        //
        //
        spdlog::info("Created a new segment.{}", (long) shm.get_address());




    }
    QuoteBook() {spdlog::info("Welcome to QuoteBook! null constructor.");QuoteBook(Name,true);}
    ~QuoteBook() {spdlog::info("Goodbye to QuoteBook! with {}",typeid(K).name());}
    void print(){spdlog::info("Welcome to Print! with generic template{}",typeid(K).name()
        );};

    V MyMapGet(K key){
        //managed_shared_memory  shm3=managed_shared_memory (open_only, Name.c_str());
        //myMap = shm.find<ShmMap>("MyMap").first;
        auto me=myMap->find(key);
        if(me==myMap->end())
        {
            throw std::invalid_argument("Key not found.");

        }
        //spdlog::info("Retrieving Item here {} {}",me->first,me->second);
        return me->second;
    }
   void MyMapSet(K key,V val){
       // spdlog::info("Setting Item {} {}",key,-10*val);
       //std::pair<K,V> myPair(key,val);
       myMap->insert_or_assign(key,val);
       V n=MyMapGet(key);


    }
private:
    //managed_shared_memory *shm;
    ShmMap *myMap;
    managed_shared_memory shm;

};


#endif //BOOK2_QUOTEBOOK_H

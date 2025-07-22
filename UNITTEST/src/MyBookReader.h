//
// Created by balmoral on 21/07/25.
//

#ifndef QUOTEBOOK_MYBOOKREADER_H
#define QUOTEBOOK_MYBOOKREADER_H

#include "spdlog/spdlog.h"

#include <string>
#include <vector>
#include <fstream>
#include <list>

struct QuoteRecord {
    std::string sym;
    std::string src;
    std::string side;
    float price;
    float size;
    int id;
    void print()
    {
        spdlog::info( " sym = {}; src= {}; side={}; price={}; size={}; id={};",sym,src,side,price,size,id);
    }
};


class MyBookReader {
public:
    explicit MyBookReader(std::string filename);
    MyBookReader();
    std::vector<QuoteRecord> read();
    std::vector<QuoteRecord> read(std::string filename);
    std::vector<QuoteRecord> myData;
    // Reads the next row and fills the vector with fields
    bool readRow(std::vector<std::string>& fields);




private:
    std::ifstream file;
    std::string filename_;




};



#endif //QUOTEBOOK_MYBOOKREADER_H

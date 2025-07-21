//
// Created by balmoral on 21/07/25.
//

#include "MyBookReader.h"
#include "spdlog/spdlog.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <string>



MyBookReader::MyBookReader(const std::string& filename) : file(filename) {
    filename_=filename;
    spdlog::info("Grabbing book data from csv {}." , filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    MyBookReader::read();
}


std::vector<QuoteRecord> MyBookReader::read() {
    spdlog::info("Grabbing BBo data from csv {}.",filename_);
    QuoteRecord data;
    int id=0;
    //std::ifstream file(filename_);
    if (!file.is_open()) {
        spdlog::critical("Error opening csv file.");
        throw std::runtime_error("Failed to open file: " + filename_);
    }

    std::string line;
    //Ditch the header . . . .
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;



        if (std::getline(ss, cell, ',')) data.sym = cell;
        if (std::getline(ss, cell, ',')) data.src = cell;
        if (std::getline(ss, cell, ',')) data.side = cell;
        if (std::getline(ss, cell, ',')) data.price = std::stof(cell);
        if (std::getline(ss, cell, ',')) data.size = std::stof(cell);
        data.id=++id;
        myData.push_back(data);
        spdlog::info("Data read. Total rows is  {}", myData.size());
        data.print();
    }
    spdlog::info("Data read. Total rows is  {}", myData.size());
    return myData;
}


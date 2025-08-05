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



MyBookReader::MyBookReader( std::string filename) : file(filename) {

    filename_=filename;
    spdlog::info("Grabbing book data from csv {}." , filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    myData.clear();
    myData=read();
}
MyBookReader::MyBookReader( ) {
    spdlog::info("Default MyBookReader constructor called.");
    MyBookReader::SetDataPath();
}

void MyBookReader::SetDataPath(){
    try {
        DataRoot = std::string(std::getenv("UATDATAROOT"));
        spdlog::info("UAT Data Filepath set to {}",DataRoot);
    }
    catch  (const std::exception& ex) {
        spdlog::info("UAT Data Filepath not set.");
        DataRoot="UNITTEST/DATA/";
    }
    spdlog::info("data Filepath set to {}",DataRoot);
    filename_="filename";

}
std::vector<QuoteRecord> MyBookReader::readpath(std::string filename) {
    std::string thisfilename;
    thisfilename=DataRoot+ "/"+ filename;
    spdlog::info("Data file being read is {}",thisfilename);
    return read(filename);

}
std::vector<QuoteRecord> MyBookReader::read(std::string filename) {

    spdlog::info("Grabbing BBo data from csv. Custom read {}.",filename);
    filename_=filename;
    file= std::ifstream(filename_);
    std::vector<QuoteRecord> thisData;
    return read();
}
 std::vector<QuoteRecord> MyBookReader::read() {
    spdlog::info("Grabbing BBo data from csv {}.",filename_);
    QuoteRecord data;
     std::vector<QuoteRecord> thisData;
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
        if (std::getline(ss, cell, ',')) data.size = std::stoi(cell);
        data.id=++id;
        thisData.push_back(data);
        data.print();
    }
    spdlog::info("Data read. Total rows is  {}", thisData.size());
    return thisData;
}


cmake_minimum_required(VERSION 3.29)
project(QuoteBook)

set(CMAKE_CXX_STANDARD 20)

include_directories(external/spdlog/include)
add_link_options("-lrt")



add_executable(SERVER server.cpp src/QuoteBook.cpp src/QuoteBook.h)

add_executable(CLIENT client.cpp src/QuoteBook.cpp src/QuoteBook.h)

##c++ /home/balmoral/BOOK2/server.cpp -lrt -lpthread src/QuoteBook.cpp -Ic++ /home/balmoral/BOOK2/server.cpp -lrt -lpthread src/QuoteBook.cpp -Iexternal/spdlog/include -o SERVER -g2 -o SERVER -g2


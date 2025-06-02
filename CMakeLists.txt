cmake_minimum_required(VERSION 3.29)
project(QuoteBook)

set(CMAKE_CXX_STANDARD 20)

include_directories(external/spdlog/include)


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g2")


add_executable(SERVER server.cpp src/QuoteBook.cpp src/QuoteBook.h)

add_executable(CLIENT client.cpp src/QuoteBook.cpp src/QuoteBook.h
        src/QuoteBook.h)

target_link_libraries(SERVER PRIVATE rt pthread)
target_link_libraries(CLIENT PRIVATE rt pthread)

#target_link_options(SERVER PRIVATE -O3)
#target_compile_options(SERVER PRIVATE -O3)
target_compile_options(SERVER PRIVATE -g2)

#target_link_options(CLIENT PRIVATE -O3)
#target_compile_options(CLIENT PRIVATE -O3)
target_compile_options(SERVER PRIVATE -g2)





##c++ /home/balmoral/BOOK2/server.cpp -lrt -lpthread src/QuoteBook.cpp -Ic++ /home/balmoral/BOOK2/server.cpp -lrt -lpthread src/QuoteBook.cpp -Iexternal/spdlog/include -o SERVER -g2 -o SERVER -g2


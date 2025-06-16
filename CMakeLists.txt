cmake_minimum_required(VERSION 3.29)
project(QuoteBook)

set(CMAKE_CXX_STANDARD 20)



# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include FetchContent module
include(FetchContent)

# Download and make GoogleTest available
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
#FetchContent_MakeAvailable(googletest)

#This can be made recursive by adding the GLOB_RECURSIVE or something.
file(GLOB SOURCES CONFIGURE_DEPENDS "UNITTEST/*")

# Add your test executable
add_executable(QuoteBook_Tests ${SOURCES})

# Link GoogleTest libraries
target_link_libraries(QuoteBook_Tests PRIVATE gtest gtest_main $<TARGET_OBJECTS:QuoteBookObject> -lrt -lpthread)

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(QuoteBook_Tests)

include_directories(external/spdlog/include)
include_directories(".")


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3")



add_executable(SERVER server.cpp)
add_executable(CLIENT client.cpp)

target_link_libraries(SERVER PRIVATE rt pthread $<TARGET_OBJECTS:QuoteBookObject>)
target_link_libraries(CLIENT PRIVATE rt pthread $<TARGET_OBJECTS:QuoteBookObject>)

target_link_options(SERVER PRIVATE -O3)
target_compile_options(SERVER PRIVATE -O3)
#target_compile_options(SERVER PRIVATE -g)

target_link_options(CLIENT PRIVATE -O3)
target_compile_options(CLIENT PRIVATE -O3)
#target_compile_options(SERVER PRIVATE -g)



add_library(QuoteBookObject OBJECT src/QuoteBook.cpp src/QuoteBook.h)
target_link_libraries(QuoteBookObject PRIVATE rt pthread)

target_link_options(QuoteBookObject PRIVATE -O3)
target_compile_options(QuoteBookObject PRIVATE -O3)


##c++ /home/balmoral/BOOK2/server.cpp -lrt -lpthread src/QuoteBook.cpp -Ic++ /home/balmoral/BOOK2/server.cpp -lrt -lpthread src/QuoteBook.cpp -Iexternal/spdlog/include -o SERVER -g2 -o SERVER -g2


#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <iostream>
#include <chrono>
#include <thread>


using namespace boost::interprocess;

struct SharedData {
    interprocess_mutex mutex;
    int value;
};

int main() {
    // Create or open shared memory
   
    std::cout << "Starting " << std::endl;

    managed_shared_memory shm(open_or_create, "SharedMemory", 1024);

    // Construct shared data structure
    SharedData* data = shm.find_or_construct<SharedData>("SharedData")();

    // Lock the mutex before modifying shared data
    std::cout << "Locking: "  << std::endl;
    data->mutex.lock();
    std::cout << "Locked: "  << std::endl;
    data->value++;
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    std::cout << "Updated value: " << data->value << std::endl;
    data->mutex.unlock();

    return 0;
}

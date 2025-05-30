#include <condition_variable> // std::condition_variable
#include <iostream>           // std::cout
#include <mutex>              // std::mutex
#include <thread>             // std::thread

using namespace std::chrono_literals;

int main()
{
    std::mutex mutex;
    std::condition_variable cv;
    bool time_for_breakfast = false; // globally shared state
    // to be called by thread
    auto student = [&]() -> void {
        { // this is the scope of the lock
            std::unique_lock<std::mutex> unique_lock(mutex);
#if 0			
            // check the globally shared state
            while (!time_for_breakfast) 
                // lock is released during wait
                cv.wait(unique_lock);
#else
            // alternative
            cv.wait(unique_lock, [&]() { return time_for_breakfast; });
#endif
        } // lock is finally released
        std::cout << "Time to make coffee!" << std::endl;
    };

    // create the waiting thread and wait for 2s
    std::thread my_thread(student);
    std::this_thread::sleep_for(2s);
    {
        // prepare the alarm clock
        std::lock_guard<std::mutex> lock_guard(mutex);
        time_for_breakfast = true;
    } // here the lock is released
    // ring the alarm clock
    cv.notify_one();

    // wait until breakfast is finished
    my_thread.join();
}

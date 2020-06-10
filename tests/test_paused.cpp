#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <atomic>
#include "thrp.hpp"

int main(){
    const int num_thr = 5;
    const int num_task = 1000;
    std::atomic_int counter = 0;

    thrp::thread_pool tp;
    tp.set_workers(num_thr);
    std::vector<std::future<void>> result;
    for(size_t i=0; i<num_task; i++){
        auto ftr = tp.emplace([&counter](){ counter++; });
        result.emplace_back(std::move(ftr));
    }
    tp.start();
    std::this_thread::sleep_for(std::chrono::nanoseconds(300));
    tp.stop();//stop, hopefully untill not all tasks are done
    std::cout<<"tasks done before stop:"<<counter<<std::endl;
    assert(counter != num_task);
    tp.start();//start again
    for(auto &ft:result){
        ft.get();
    }
    std::cout<<"tasks done all:"<<counter<<std::endl;
    assert(counter == num_task);
}

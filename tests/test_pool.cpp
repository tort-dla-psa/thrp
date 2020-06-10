#include <future>
#include <iostream>
#include <vector>
#include "thrp.hpp"

int main(){
    const int num_thr = 5;
    const int num_task = 10;

    thrp::thread_pool tp;
    tp.set_workers(num_thr);
    std::vector<std::future<int>> result;
    for(size_t i=0; i<num_task; i++){
        auto ftr = tp.emplace([](auto arg){ return arg; }, 1337);
        result.emplace_back(std::move(ftr));
    }
    tp.start();
    for(auto &ft:result){
        std::cout<<ft.get()<<std::endl;
    }
}

# thrp
This is a simple and bloat-free header-only threadpool library. C++17.

It uses concurrent queue and as less blocking code as it's possible.

![CmakeCI](https://github.com/tort-dla-psa/thrp/workflows/CmakeCI/badge.svg)
## Usage
Basic usage would look like that:
```c++
thrp::thread_pool tp(5);
std::vector<std::future<int>> result;
//enqueue tasks and save futures
for(size_t i=0; i<20; i++){
    auto lbd = [](auto arg){ return arg; };
    auto ftr = tp.emplace(std::move(lbd), 1337);
    result.emplace_back(std::move(ftr));
}
tp.start();
//get results
for(auto &ft:result){
    std::cout<<ft.get()<<std::endl;
}
```

Others examples can be found in [tests](tests)

## Building
As a submodule in your project:
```bash
#in shell
mkdir -p submodules/
git submodule init
git submodule add https://github.com/tort-dla-psa/thrp.git submodules/thrp
```
```cmake
#in cmake
include_directories(submodules/thrp)
```
```c++
//in code
#include <thrp.hpp>
//...
thrp::thread_pool p; //and so on
```

## License 
MIT, of course

## Contributors 
Just me yet

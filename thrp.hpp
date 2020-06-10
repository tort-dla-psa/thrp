#pragma once
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <list>
#include <future>
#include <functional>

namespace thrp{

class thread_pool{
    std::list<std::thread> m_workers; /**< Dynamic container with workers */
    std::condition_variable m_cond_var; /**< Condition variable to communicate with workers */
    std::mutex m_queue_mt; /**< Mutex for condition variable */
    bool m_stop, /**< Bool for stopping workers */
         m_start; /**< Bool for starting workers */

    size_t m_workers_count; /** Cached count of workers */
    std::queue<std::function<void()>> m_tasks; /** Queue for sending tasks */
    
    void p_start(); /** Infinite function for workers */
public:
    /**
     * Constructor.
     *
     * @param size Number of workers to do stuff.
     *      Note: by default thread_pool is not started, you need to manualy do that
     */
    thread_pool(const size_t &size=0);

    /**
     * Destructor.
     * It will automatically stop all workers.
     */
    ~thread_pool();

    /**
     * Stop all workers but save tasks that were not done yet.
     */
    void stop();
    /**
     * Start workers and enqueued tasks.
     * You can do it after stopping pool too.
     */
    void start();
    /**
     * Set number of workers. This will stop thread pull if it's running.
     * Tasks will be preserved.
     * @param size Number of workers to do stuff.
     */
    void set_workers(const size_t &num);
    /**
     * Get number of workers.
     */
    size_t workers()const;

    /**
     * Enqueue task with N or NONE parameters.
     * @return future result of the task
     */
    template<class F, class... Args>
    auto emplace(F &&f, Args &&... args);
};

thread_pool::thread_pool(const size_t &size){
    set_workers(size);
}
thread_pool::~thread_pool(){
    stop();
}

inline void thread_pool::p_start(){
    while(1) {
        std::function<void()> task;
        {
            std::unique_lock lock(m_queue_mt);
            m_cond_var.wait(lock, [this]{
                return m_stop || (m_start && !m_tasks.empty());
            });
            if(m_stop){ return; }
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        task();
    }
}
inline void thread_pool::stop(){
    m_stop = true;
    m_cond_var.notify_all();
    for(auto &thr:m_workers){
        thr.join();
    }
    m_workers.clear();
}
inline void thread_pool::start(){
    set_workers(m_workers_count);
    this->m_start = true;
    m_cond_var.notify_all();
}
inline void thread_pool::set_workers(const size_t &num){
    stop();
    m_stop = m_start = false;
    this->m_workers_count = num;
    for(size_t i=0; i<num; i++){
        m_workers.emplace_back([this](){p_start();});
    }
}
inline size_t thread_pool::workers()const{
    return m_workers.size();
}

template<class F, class... Args>
inline auto thread_pool::emplace(F &&f, Args &&... args) {
    using return_type = std::invoke_result_t<F, Args...>;

    auto task = new std::packaged_task<return_type()>
        (std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        
    auto res = task->get_future();
    auto call = [task](){ (*task)(); delete task; };
    {
        std::unique_lock lock(m_queue_mt);
        m_tasks.emplace(std::move(call));
    }
    m_cond_var.notify_one();
    return res;
}

}

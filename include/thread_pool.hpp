// I just went to stackoverflow for this since I didn't feel like fully learning this
// much about multithreading yet, wanting to get back to another project but wanting to
// do one last optimization.

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <thread>

// Source - https://stackoverflow.com/a/32593825
// Posted by PhD AP EcE, modified by community. See post 'Timeline' for change history
// Retrieved 2026-06-15, License - CC BY-SA 4.0

class ThreadPool {
    public:
    void Start();
    void QueueJob(const std::function<void()>& job);
    void Stop();
    bool busy();
    
    private:
    void ThreadLoop();
    
    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
};

#endif
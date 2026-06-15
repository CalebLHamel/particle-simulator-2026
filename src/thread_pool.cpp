#include "thread_pool.hpp"

// Source - https://stackoverflow.com/a/32593825
// Posted by PhD AP EcE, modified by community. See post 'Timeline' for change history
// Retrieved 2026-06-15, License - CC BY-SA 4.0

void ThreadPool::Start() {
    const uint32_t num_threads = std::thread::hardware_concurrency(); // Max # of threads the system supports
    for (uint32_t ii = 0; ii < num_threads; ++ii) {
        threads.emplace_back(std::thread(&ThreadPool::ThreadLoop,this));
    }
}

// Source - https://stackoverflow.com/a/32593825
// Posted by PhD AP EcE, modified by community. See post 'Timeline' for change history
// Retrieved 2026-06-15, License - CC BY-SA 4.0

void ThreadPool::ThreadLoop() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            mutex_condition.wait(lock, [this] {
                return !jobs.empty() || should_terminate;
            });
            if (should_terminate) {
                return;
            }
            job = jobs.front();
            jobs.pop();
        }
        job();
    }
}

// Source - https://stackoverflow.com/a/32593825
// Posted by PhD AP EcE, modified by community. See post 'Timeline' for change history
// Retrieved 2026-06-15, License - CC BY-SA 4.0

void ThreadPool::QueueJob(const std::function<void()>& job) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        jobs.push(job);
    }
    mutex_condition.notify_one();
}

// Source - https://stackoverflow.com/a/32593825
// Posted by PhD AP EcE, modified by community. See post 'Timeline' for change history
// Retrieved 2026-06-15, License - CC BY-SA 4.0

bool ThreadPool::busy() {
    bool poolbusy;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        poolbusy = !jobs.empty();
    }
    return poolbusy;
}

#pragma once
#include <vector>
#include <thread>
#include <iostream>
#include <atomic>

class ThreadPool {
public:
    static const size_t POOL_SIZE = 1;
    static const size_t MIN_CHUNK_SIZE = 1000;

    // Runs map(f, range(n)) effectively.
    template <class F>
    void mapChunks(size_t n, F f)
    {
        pool_.clear();
        size_t chunkSize = std::max(MIN_CHUNK_SIZE, n / POOL_SIZE);
        size_t up = 0;
        for (size_t k = 0; up < n && k < POOL_SIZE; ++k ) {
            size_t down = up;
            up = std::min(n, up + chunkSize);
            pool_.push_back(std::thread(f, down, up));
        }
        for (auto& thread : pool_)
            thread.join();
    }

    // Returns any(map(f, tasks)).
    // f should support stopCondition as the second agrument.
    template <class B, class F>
    bool searchAny(const std::vector<B>& tasks, F f)
    {
        pool_.resize(POOL_SIZE);
        size_t threadId = 0;
        bool stopCondition = false;
        std::vector<std::atomic<bool>> finished(POOL_SIZE);
        for (auto& x : finished)
            x = true;
        for (size_t i = 0; !stopCondition && i < tasks.size(); ++i) {
            while(!stopCondition && !finished[threadId]) {
                threadId = (threadId + 1) % POOL_SIZE;
            }
            if (stopCondition)
                break;
            finished[threadId] = false;
            if (pool_[threadId].joinable())
                pool_[threadId].join();
            pool_[threadId] = std::thread(
                    [i, &tasks, &stopCondition, &finished, threadId, f] {
                f(tasks[i], stopCondition);
                finished[threadId] = true;
            });
        }
        for (auto& t: pool_) {
            if (t.joinable())
                t.join();
        }
        return stopCondition;
    }

private:
    std::vector<std::thread> pool_;
};

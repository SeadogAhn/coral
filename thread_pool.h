/*!
    \file       thread_pool.h
    \brief      Thread pool management using the Modern C++
    \details    Thread management using the queue and the thread in the modern C++
    \author     seadog.ahn@gmail.com
    \date       2007-2023
    \copyright  All right reserved by seadog.ahn
*/
#ifndef __CORAL_THREAD_POOL_H__
#define __CORAL_THREAD_POOL_H__

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

//! Core Library for Applications and Libraries
namespace coral {
    //! thread pool class using queue and modern thread
    class thread_pool {
    public:
        //! constructor
        explicit thread_pool(size_t thread_size) : thread_size_(thread_size), stop_all_(false)
        {
            worker_threads_.reserve(thread_size_);
            for (size_t i = 0; i < thread_size_; ++i) {
                worker_threads_.emplace_back([this]() { this->worker_thread(); });
            }
        }
        //! destructor
        virtual ~thread_pool() {
            stop_all_ = true;
            job_queue_.notify_all();

            for (auto& t : worker_threads_) {
                t.join();
            }
        }

        //! add job to queue
        template <class F, class... Args>
        std::future<typename std::result_of<F(Args...)>::type> enqueue_job(F&& f, Args&&... args)
        {
            if (stop_all_) {
                throw thread_error("All thread had been terminated.");
            }

            using return_type = typename std::result_of<F(Args...)>::type;
            auto job = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            std::future<return_type> job_result_future = job->get_future();
            {
                std::lock_guard<std::mutex> lock(mutex_job_queue_);
                queue_jobs_.push([job]() { (*job)(); });
            }
            job_queue_.notify_one();

            return job_result_future;
        }
    private:
        //! thread worker
        void worker_thread()
        {
            while (true) {
                std::function<void()> job;
                {
                    std::unique_lock<std::mutex> lock(mutex_job_queue_);
                    job_queue_.wait(lock, [this]() { return !this->queue_jobs_.empty() || stop_all_; });
                    if (stop_all_ && this->queue_jobs_.empty()) {
                        return;
                    }
                    //pop front a job
                    job = std::move(queue_jobs_.front());
                    queue_jobs_.pop();
                    // lock.unlock();
                    // execute job
                }
                job();
            }
        }

        size_t thread_size_;                            ///< the number of the total of the threads
        std::vector<std::thread> worker_threads_;       ///< conatiner of worker thread
        std::queue<std::function<void()>> queue_jobs_;  ///< job queue
        std::condition_variable job_queue_;             ///< conditon variable
        std::mutex mutex_job_queue_;                    ///< queue mutex
        bool stop_all_;                                 ///< is all thread has been terminated
    };
} // namespace coral
#endif // __CORAL_THREAD_POOL_H__

/**
 * @file        thread_pool.cpp
 * @author      Sergio Guerrero Blanco
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief Implementation of ThreadPool.
 *
 * @details
 * This file contains the execution logic for worker threads,
 * enqueue operations, and shutdown mechanisms.
 */

/*****************************************************************************/

/* Project libraries */

#include "thread_pool.h"

#include "i_job.h"
#include "logger.h"

/*****************************************************************************/

/* Public Methods */

/**
 * @brief Creates a non-running thread pool.
 */
ThreadPool::ThreadPool() : running(false) {}

/**
 * @brief Ensures all worker threads are stopped and joined.
 */
ThreadPool::~ThreadPool()
{
    shutdown();
}

/**
 * @brief Starts N threads.
 */
void ThreadPool::start(size_t number_threads)
{
    if (running)
        return;

    running = true;
    if (number_threads == 0)
        number_threads = 1;
    Logger::info("[Thread Pool] Starting " + std::to_string(number_threads) + " threads");
    for (size_t i = 0; i < number_threads; ++i)
    {
        threads.emplace_back([this, i]() { threadLoop("Thread " + std::to_string(i)); });
    }
}

/**
 * @brief Enqueues a job for later execution.
 */
void ThreadPool::enqueue(std::unique_ptr<IJob> job)
{
    queue.push(std::move(job));
}

/**
 * @brief Attempts to enqueue a job only if pool is running.
 */
bool ThreadPool::tryEnqueue(std::unique_ptr<IJob> job)
{
    if (!running.load(std::memory_order_acquire))
    {
        Logger::warn("[ThreadPool] Job rejected: pool not running.");
        return false;
    }

    if (queue.is_closed())
    {
        Logger::warn("[ThreadPool] Job rejected: queue is closed.");
        return false;
    }

    queue.push(std::move(job));
    return true;
}

/**
 * @brief Gracefully shuts down the pool.
 */
void ThreadPool::shutdown()
{
    if (!running)
    {
        return;
    }

    running = false;

    Logger::info("[Thread Pool] Shutdown requested...");

    auto start = std::chrono::steady_clock::now();
    while (!queue.empty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        if (std::chrono::steady_clock::now() - start > std::chrono::seconds(1))
        {
            Logger::warn("[Thread Pool] Timeout waiting for queue to drain.");
            break;
        }
    }

    queue.shutdown();

    join();
    Logger::info("[Thread Pool] All workers joined. Shutdown complete.");
}

/**
 * @brief Immediately stops all workers, discarding queued jobs.
 */
void ThreadPool::shutdownNow()
{
    if (!running)
    {
        return;
    }

    running = false;

    Logger::info("[Thread Pool] Shutdown requested...");

    queue.shutdown();

    join();
    Logger::info("[Thread Pool] All workers joined. Shutdown complete.");
}

/**
 * @brief Waits for all threads to finish.
 */
void ThreadPool::join()
{
    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    threads.clear();
}

/**
 * @brief Returns number of worker threads.
 */
size_t ThreadPool::size() const
{
    return threads.size();
}

/**
 * @brief Returns true if pool is still operational.
 */
bool ThreadPool::isRunning() const
{
    return running;
}

/*****************************************************************************/

/* Private Methods */

/**
 * @brief Worker execution loop.
 *
 * @details
 * Each worker:
 *  - Blocks on queue.pop()
 *  - Exits when pop() returns nullptr (queue closed)
 *  - Catches exceptions thrown by jobs to avoid worker death
 */
void ThreadPool::threadLoop(const std::string& worker_name)
{
    Logger::info("[" + worker_name + "] Started");

    while (true)
    {
        std::unique_ptr<IJob> job = queue.pop();

        if (!job)
            break;

        try
        {
            job->execute();
        }
        catch (const std::exception& e)
        {
            Logger::error("[Thread Pool][" + worker_name + "] Exception: " + e.what());
        }
    }
    Logger::info("[" + worker_name + "] Exiting");
}

/*****************************************************************************/
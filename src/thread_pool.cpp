/**
 * @file        thread_pool.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-09-26>
 * @version     1.0.0
 *
 * @brief
 *
 * @details
 *
 */

/*****************************************************************************/

/* Project libraries */

#include "thread_pool.h"

#include "i_job.h"
#include "logger.h"

/*****************************************************************************/

/* Public Methods */

/**
 * @brief
 *
 * @param
 *
 * @details
 * GIVEN
 * WHEN
 * THEN
 *
 *
 */
ThreadPool::ThreadPool() : running(false) {}

/**
 * @brief
 *
 * @details
 *
 *
 * @note
 *
 */
ThreadPool::~ThreadPool()
{
    shutdown();
}

/**
 * @brief
 *
 * @param
 *
 * @details
 * GIVEN
 * WHEN
 * THEN:
 *
 * Example:
 * ```cpp
 *
 * ```
 *
 * @note
 *
 */
void ThreadPool::start(size_t number_workers)
{
    if (running)
        return;

    running = true;
    if (number_workers == 0)
        number_workers = 1;
    Logger::info("[Thread Pool] Starting " + std::to_string(number_workers) + " workers");
    for (size_t i = 0; i < number_workers; ++i)
    {
        threads.emplace_back([this, i]() { threadLoop("Worker " + std::to_string(i)); });
    }
}

/**
 * @brief
 *
 * @param
 *
 * @details
 * GIVEN
 * WHEN
 * THEN
 *
 * Example:
 * ```cpp
 *
 * ```
 *
 * @note
 *
 */
void ThreadPool::enqueue(std::unique_ptr<IJob> job)
{
    queue.push(std::move(job));
}

/**
 * @brief
 *
 * @param
 *
 * @details
 * GIVEN
 * WHEN
 * THEN
 *
 * Example:
 * ```cpp
 *
 * ```
 *
 * @note
 *
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
 * @brief
 *
 * @details
 * GIVEN
 * WHEN
 * THEN:
 *
 *
 * @note
 *
 *
 * @warning
 *
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
 * @brief
 *
 * @details
 * GIVEN
 * WHEN
 * THEN:
 *
 *
 * @note
 *
 *
 * @warning
 *
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
 * @brief
 *
 * @details
 * GIVEN
 * WHEN
 * THEN:
 *
 *
 * @note
 *
 *
 * @warning
 *
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
 * @brief
 *
 * @details
 * GIVEN
 * WHEN
 * THEN:
 *
 *
 * @note
 *
 *
 * @warning
 *
 */
size_t ThreadPool::size() const
{
    return threads.size();
}

/**
 * @brief
 *
 * @details
 * GIVEN
 * WHEN
 * THEN:
 *
 *
 * @note
 *
 *
 * @warning
 *
 */
bool ThreadPool::isRunning() const
{
    return running;
}

/*****************************************************************************/

/* Private Methods */

/**
 * @brief
 *
 * @param
 *
 * @details
 *
 * GIVEN
 * WHEN
 * THEN:
 *
 *
 * @note
 *
 *
 * @exception std::exception
 *
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
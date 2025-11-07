/**
 * @file        queue_job.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-10-27>
 * @version     0.0.0
 *
 * @brief
 *
 * @details
 */

/*****************************************************************************/

/* Standard libraries */

/* Project libraries */

#include "job_queue.h"

#include "logger.h"

/*****************************************************************************/

/**
 * @brief
 *
 * @param
 *
 * @details
 *
 * @warning
 */
void JobQueue::push(std::unique_ptr<IJob> job)
{
    std::unique_lock<std::mutex> lock(mtx);
    buffer.emplace_back(std::move(job));
    cv.notify_one();
}

/**
 * @brief
 *
 * @param
 * @return
 *
 * @details
 */
std::unique_ptr<IJob> JobQueue::pop()
{
    std::unique_lock<std::mutex> lock(mtx);
    std::unique_ptr<IJob>        data;

    // Wait until new data is added
    cv.wait(lock, [this] { return closed || !buffer.empty(); });

    if (closed && buffer.empty())
        return data;

    Logger::info("[Queue Job] Job extracted successfully");
    data = std::move(buffer.front());
    buffer.pop_front();
    return data;
}

/**
 * @brief Checks whether the queue is currently empty.
 *
 * @return `true` if the queue is empty, `false` otherwise.
 *
 * @note
 * This method is thread-safe and can be called concurrently with other operations.
 */
bool JobQueue::empty() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.empty();
}

/**
 * @brief Returns the current number of elements in the queue.
 *
 * @return Number of elements stored in the internal buffer.
 *
 * @note
 * This method acquires a lock briefly to read the size safely.
 */
size_t JobQueue::size() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.size();
}

/**
 * @brief Clears all elements currently stored in the queue.
 *
 * @details
 * Removes all elements from the internal buffer.
 * Does not affect the `closed` state.
 *
 * @warning
 * Should be used carefully in concurrent environments to avoid discarding
 * data being produced/consumed simultaneously.
 */
void JobQueue::clear()
{
    std::lock_guard<std::mutex> lock(mtx);
    Logger::info("[Queue Job] Jobs cleaned");
    buffer.clear();
}

/**
 * @brief Shutdown the queue, unblocking all waiting threads.
 *
 * @details
 * Sets an internal flag (`closed = true`) and notifies all waiting threads
 * so they can exit gracefully.
 *
 * After calling this, subsequent calls to `pop()` will return `false`
 * once the queue is empty.
 */
void JobQueue::shutdown()
{
    std::lock_guard<std::mutex> lock(mtx);
    closed = true;
    Logger::info("[Queue Job] Queue job closed");
    cv.notify_all();
}

/**
 * @brief
 *
 * @details
 */
bool JobQueue::is_closed()
{
    return closed;
}
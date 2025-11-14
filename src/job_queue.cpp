/**
 * @file        job_queue.cpp
 * @author      Sergio Guerrero Blanco
 * @date        2025-09-26
 * @version     1.0.0
 *
 * @brief Implementation of the thread-safe job queue.
 *
 * @details
 * This file contains the full implementation of `JobQueue`, a multi-producer
 * multi-consumer FIFO queue used as the backbone of the thread pool.
 *
 * Key properties:
 * - Safe concurrent access through `std::mutex`.
 * - Blocking pop with condition variable.
 * - Deterministic shutdown: waiting consumers wake up and return nullptr.
 * - Ownership is transferred using `std::unique_ptr<IJob>`.
 */

/*****************************************************************************/

/* Standard libraries */

/* Project libraries */

#include "job_queue.h"

#include "logger.h"

/*****************************************************************************/

/**
 * @brief Inserts a job into the queue and wakes one waiting consumer.
 *
 * @param job A non-null unique_ptr owning the job to insert.
 *
 * @details
 * ### Concurrency:
 * - Acquires `mtx` exclusively.
 * - Appends the job to the FIFO buffer.
 * - Calls `notify_one()` to wake exactly one thread blocked in `pop()`.
 *
 * ### Notes:
 * - There is no rejection of jobs after shutdown; the thread pool
 *   is responsible for preventing enqueue after close.
 */
void JobQueue::push(std::unique_ptr<IJob> job)
{
    std::unique_lock<std::mutex> lock(mtx);
    buffer.emplace_back(std::move(job));
    cv.notify_one();
}

/**
 * @brief Retrieves the next available job, blocking if necessary.
 *
 * @return
 * - A `std::unique_ptr<IJob>` containing the next job.
 * - `nullptr` if the queue is closed AND empty.
 *
 * @details
 * ### Blocking behaviour:
 * - If the queue is empty and *not closed*, the caller blocks.
 * - The thread wakes when:
 *      1. A new job is pushed, or
 *      2. `shutdown()` is called.
 *
 * ### Shutdown semantics:
 * - If `closed == true` and there are no jobs remaining,
 *   this function returns `nullptr` immediately.
 *
 * ### Concurrency:
 * - `buffer` is protected by `mtx`.
 * - Uses a condition-variable predicate (`closed || !buffer.empty()`),
 *   which prevents spurious wakeups causing incorrect behaviour.
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
 * @brief Returns whether the queue is empty.
 *
 * @details
 * - Non-blocking.
 * - Requires acquiring `mtx` briefly.
 *
 * @warning
 * The result is only a snapshot; another thread may enqueue immediately after.
 */
bool JobQueue::empty() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.empty();
}

/**
 * @brief Returns the number of pending jobs currently stored.
 *
 * @details
 * - Thread-safe snapshot.
 * - Does not block waiting consumers or producers for long.
 */
size_t JobQueue::size() const
{
    std::lock_guard<std::mutex> lock(mtx);
    return buffer.size();
}

/**
 * @brief Removes all jobs currently stored in the queue.
 *
 * @details
 * Primarily used during manual cleanup or tests.
 *
 * ### Important:
 * - This does *not* wake consumers.
 * - Does *not* affect the closed state.
 *
 * ### Concurrency:
 * - Exclusive lock for the duration of the clear.
 */
void JobQueue::clear()
{
    std::lock_guard<std::mutex> lock(mtx);
    Logger::info("[Queue Job] Jobs cleaned");
    buffer.clear();
}

/**
 * @brief Closes the queue and wakes all waiting threads.
 *
 * @details
 * ### Effects:
 * - Sets `closed = true`.
 * - Wakes *all* threads waiting on `pop()`.
 * - Allows `pop()` to return `nullptr` when the queue becomes empty.
 *
 * ### Concurrency:
 * - Fully thread-safe; multiple calls are safe (idempotent).
 */
void JobQueue::shutdown()
{
    std::lock_guard<std::mutex> lock(mtx);
    closed = true;
    Logger::info("[Queue Job] Queue job closed");
    cv.notify_all();
}

/**
 * @brief Returns whether the queue has been closed.
 *
 * @details
 * - Thread-safe.
 * - Reads protected by mutex.
 */
bool JobQueue::is_closed()
{
    return closed;
}
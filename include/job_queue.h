/**
 * @file        job_queue.h
 * @author      Sergio Guerrero Blanco
 * @date        2025-09-26
 * @version     1.0.0
 *
 * @brief Thread-safe FIFO queue for `IJob` instances.
 *
 * @details
 * `JobQueue` implements a multi-producer / multi-consumer work queue.
 * It provides:
 *  - Blocking pop (`pop()`)
 *  - Non-blocking queries (`empty()`, `size()`)
 *  - Graceful shutdown (`shutdown()`)
 *
 * ### Concurrency guarantees:
 * - All operations are thread-safe.
 * - FIFO ordering is strictly preserved.
 * - `pop()` blocks until a job becomes available or the queue is closed.
 * - Once closed, consumers are awakened and eventually return `nullptr`.
 *
 * ### Design:
 * - Uses `std::unique_ptr<IJob>` for exclusive ownership.
 * - Protected internally by `std::mutex` and a `std::condition_variable`.
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>

/* Project libraries */

#include "i_job.h"

/*****************************************************************************/

/**
 * @class JobQueue
 *
 * @brief A thread-safe FIFO queue for job dispatching.
 *
 * @details
 * This queue is intended for use in thread pools and other concurrent
 * job-processing systems. It provides deterministic shutdown semantics
 * and safe synchronization between multiple producers and consumers.
 *
 * ### Queue states:
 * - **Open**: accepts new jobs; `pop()` may block.
 * - **Closed**: rejects no jobs explicitly, but `pop()` returns `nullptr`
 *   once the queue drains.
 */
class JobQueue
{
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Constructs an empty, open queue.
     */
    explicit JobQueue() = default;

    /**
     * @brief Default destructor.
     *
     * @details
     * If the queue was closed via `shutdown()`, any threads blocked on
     * `pop()` will already have been awakened.
     */
    ~JobQueue() = default;

    /**
     * @brief Disable copy constructor.
     *
     * JobQueue cannot be copied because it manages synchronization primitives
     * (`std::mutex`, `std::condition_variable`), which are inherently non-copyable.
     */
    JobQueue(const JobQueue&) = delete;

    /**
     * @brief Disable copy assignment operator.
     *
     * Copy assignment is forbidden for the same reason as the copy constructor:
     * synchronization primitives cannot be duplicated safely.
     */
    JobQueue& operator=(const JobQueue&) = delete;

    /**
     * @brief Disable move constructor.
     *
     * Moving synchronization primitives would invalidate any existing waiting threads.
     * To preserve strict ownership semantics, move operations are not supported.
     */
    JobQueue(JobQueue&&) = delete;

    /**
     * @brief Disable move assignment operator.
     *
     * Moving the queue could break thread synchronization guarantees, so it is disabled.
     */
    JobQueue& operator=(JobQueue&&) = delete;

    /**
     * @brief Pushes a job into the queue.
     *
     * @param job Exclusive pointer to the job to insert (must be non-null).
     *
     * @details
     * Wakes one waiting consumer if any are blocked on `pop()`.
     */
    void push(std::unique_ptr<IJob> job);

    /**
     * @brief Pops the next available job (blocking).
     *
     * @return A `std::unique_ptr<IJob>` containing the next job, or `nullptr`
     *         if the queue has been closed and no jobs remain.
     *
     * @details
     * - Blocks while the queue is empty and still open.
     * - If the queue is closed and empty, returns `nullptr` immediately.
     */
    std::unique_ptr<IJob> pop();

    /**
     * @brief Returns whether the queue is currently empty.
     *
     * @note Thread-safe.
     */
    bool empty() const;

    /**
     * @brief Returns the number of pending jobs.
     *
     * @note Thread-safe.
     */
    size_t size() const;

    /**
     * @brief Removes all pending jobs.
     *
     * @warning Does not affect the open/closed state of the queue.
     */
    void clear();

    /**
     * @brief Closes the queue and wakes all waiting threads.
     *
     * @details
     * Once closed:
     *  - Consumers eventually return `nullptr` from `pop()`.
     *  - The queue will not block again.
     */
    void shutdown();

    /**
     * @brief Returns whether the queue is closed.
     */
    bool is_closed();

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief FIFO storage.
     */
    std::deque<std::unique_ptr<IJob>> buffer;

    /**
     * @brief Synchronization primitive.
     */
    mutable std::mutex mtx;

    /**
     * @brief Wakes consumers on push/shutdown.
     */
    std::condition_variable cv;

    /**
     * @brief Indicates whether the queue is closed.
     */
    bool closed = false;

    /******************************************************************/
};
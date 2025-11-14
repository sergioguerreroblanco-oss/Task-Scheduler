/**
    * @file        job_queue.h
    * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
    * @date        <2025-09-26>
    * @version     0.0.0
    *
    * @brief
    *
    * @details
    */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <condition_variable>
#include <deque>
#include <mutex>
#include <memory> 

/* Project libraries */

#include "i_job.h"

    /*****************************************************************************/

    /**
     * @class ThreadSafeQueue
     * @brief Thread-safe FIFO queue supporting multiple producers and consumers.
     *
     * @tparam T Type of element stored in the queue.
     *
     * @note
     * This queue is designed for use in multi-threaded environments.
     * It provides blocking (`pop`) and non-blocking (`try_pop`) retrieval operations,
     * as well as a `close()` method for graceful termination of waiting consumers.
     */
    class JobQueue
{
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Default constructor.
     */
    explicit JobQueue() = default;

    /**
     * @brief Destructor.
     *
     * Automatically releases resources. If `close()` was not called, any threads waiting
     * on `pop()` may be unblocked during destruction.
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
     * @brief
     *
     * @param
     *
     * @details
     *
     * @warning
     */
    void push(std::unique_ptr<IJob> job);

    /**
     * @brief
     *
     * @param
     * @return
     *
     * @details
     */
    std::unique_ptr<IJob> pop();

    /**
     * @brief Checks whether the queue is currently empty.
     *
     * @return `true` if the queue is empty, `false` otherwise.
     *
     * @note
     * This method is thread-safe and can be called concurrently with other operations.
     */
    bool empty() const;

    /**
     * @brief Returns the current number of elements in the queue.
     *
     * @return Number of elements stored in the internal buffer.
     *
     * @note
     * This method acquires a lock briefly to read the size safely.
     */
    size_t size() const;

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
    void clear();

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
    void shutdown();

    /**
     * @brief
     *
     * @details
     */
    bool is_closed();

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief Internal FIFO buffer used to store queued elements.
     */
    std::deque<std::unique_ptr<IJob>> buffer;

    /**
     * @brief Mutex protecting access to the buffer and synchronization state.
     */
    mutable std::mutex mtx;

    /**
     * @brief Condition variable used to signal availability of data.
     */
    std::condition_variable cv;

    /**
     * @brief Indicates whether the queue has been closed (graceful shutdown flag).
     */
    bool closed = false;

    /******************************************************************/
};
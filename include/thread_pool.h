/**
 * @file        thread_pool.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-11-10>
 * @version     1.0.0
 *
 * @brief ThreadPool manages a group of worker threads consuming jobs from a shared JobQueue.
 *
 * @details
 * The ThreadPool provides:
 *  - A configurable number of worker threads (default = hardware concurrency).
 *  - FIFO job submission through `enqueue()` and `tryEnqueue()`.
 *  - Graceful shutdown (`shutdown()`): waits for queued jobs to finish.
 *  - Immediate shutdown (`shutdownNow()`): stops accepting jobs, interrupts waiting threads.
 *  - Automatic thread joining and safe cleanup.
 *
 * Jobs must inherit from `IJob` and override `execute()`.
 *
 * The pool guarantees:
 *  - No job is lost after being accepted.
 *  - threads survive job exceptions.
 *  - Shutdown always joins all threads safely.
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

/* Project libraries */

#include "job_queue.h"

/*****************************************************************************/

/**
 * @class ThreadPool
 * @brief A fixed-size pool of worker threads executing asynchronous jobs.
 *
 * @details
 * The pool starts N worker threads that repeatedly:
 *   1. Block on the shared JobQueue.
 *   2. Retrieve the next job.
 *   3. Execute it inside a try/catch to prevent worker termination.
 *
 * ThreadPool is non-copyable and non-movable to avoid transferring thread ownership.
 * Lifetime is strictly controlled: destruction forces a complete shutdown.
 */
class ThreadPool
{
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Constructs an empty thread pool (not running).
     *
     * @details
     * threads are NOT created until `start()` is called.
     */
    explicit ThreadPool();

    /**
     * @brief Destructs the thread pool.
     *
     * @details
     * Automatically performs `shutdown()` to ensure all threads are joined.
     */
    ~ThreadPool();

    /**
     * @brief Deleted copy constructor.
     *
     * @details
     * ThreadPool cannot be copied because it manages active threads and synchronization primitives.
     * Copying would imply duplicating ownership of threads, which is undefined behavior.
     */
    ThreadPool(const ThreadPool&) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * @details
     * Prevents assignment between ThreadPool instances for the same reason as the copy constructor:
     * thread ownership cannot be shared safely.
     */
    ThreadPool& operator=(const ThreadPool&) = delete;

    /**
     * @brief Deleted move constructor.
     *
     * @details
     * Although `std::thread` supports move semantics, allowing ThreadPool to be movable would
     * transfer ownership of active threads and invalidate the source object. Movement is disabled
     * to enforce strict lifetime control.
     */
    ThreadPool(ThreadPool&&) = delete;

    /**
     * @brief Deleted move assignment operator.
     *
     * @details
     * Move assignment is disabled to avoid undefined behavior related to thread ownership transfer.
     */
    ThreadPool& operator=(ThreadPool&&) = delete;

    /**
     * @brief Starts the threads.
     *
     * @param number_threads Number of threads to create.
     * @details
     * If called multiple times, only the first one creates threads.
     *
     * If number_threads == 0, the pool forces 1 thread.
     */
    void start(size_t number_threads = std::thread::hardware_concurrency());

    /**
     * @brief Enqueues a job for execution.
     *
     * @param job Unique pointer to an `IJob` instance.
     *
     * @details
     * The job is ALWAYS accepted as long as:
     *  - the pool is running, and
     *  - the queue is not closed.
     *
     * @warning This function throws if `job` is nullptr.
     */
    void enqueue(std::unique_ptr<IJob> job);

    /**
     * @brief Attempts to enqueue a job without guaranteeing acceptance.
     *
     * @param job Unique pointer to an `IJob` instance.
     * @return `true` if the job was accepted, `false` otherwise.
     *
     * @details
     * Useful when external systems must avoid blocking or must not enqueue
     * tasks during shutdown.
     */
    bool tryEnqueue(std::unique_ptr<IJob> job);

    /**
     * @brief Gracefully shuts down the pool.
     *
     * @details
     * - Stops accepting new jobs.
     * - Waits up to 1 second for the queue to drain.
     * - Closes the queue.
     * - Joins all threads.
     *
     * threads finish any job already in progress.
     */
    void shutdown();

    /**
     * @brief Immediately shuts down the pool.
     *
     * @details
     * - Stops accepting new jobs.
     * - Immediately closes the queue.
     * - threads unblock and exit even if jobs remain unprocessed.
     *
     * This is the “fail-fast” version of shutdown().
     */
    void shutdownNow();

    /**
     * @brief Waits for all worker threads to finish.
     *
     * @details
     * Called automatically by shutdown() and shutdownNow().
     */
    void join();

    /**
     * @brief Returns the number of active worker threads.
     */
    size_t size() const;

    /**
     * @brief Indicates if the pool is running.
     *
     * @return true if threads are still active.
     */
    bool isRunning() const;

    /******************************************************************/

    /* Private Methods */

   private:
    /**
     * @brief Main loop executed by each worker thread.
     *
     * @details
     * Each worker:
     *  - Blocks on JobQueue::pop()
     *  - Exits when `nullptr` is returned (queue closed)
     *  - Catches exceptions thrown by jobs
     */
    void threadLoop(const std::string& worker_name);

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief Shared job queue.
     */
    JobQueue queue;

    /**
     * @brief Indicates whether the pool is in running state.
     */
    std::atomic<bool> running;

    /**
     * @brief Threads.
     */
    std::vector<std::thread> threads;

    /******************************************************************/
};
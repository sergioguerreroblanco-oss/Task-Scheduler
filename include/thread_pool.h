/**
    * @file        thread_pool.h
    * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
    * @date        <2025-11-10>
    * @version     1.0.0
    *
    * @brief       Thread pool manages multiple worker threads consuming tasks from a shared queue.
    *
    * @details
    *
    */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <memory>

/* Project libraries */

#include "job_queue.h"

    /*****************************************************************************/

    /**
     * @class ThreadPool
     * @brief
     *
     * @details
     *
     * @note
     *
     */
    class ThreadPool
{
    /******************************************************************/

    /* Private Constants */

   private:
    /**
     * @brief Default prefix for worker thread names.
     */
    static constexpr const char* DEFAULT_WORKER_NAME = "Worker ";

    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Constructs a ThreadPool
     *
     * @param
     *
     * @details
     * GIVEN
     * WHEN
     * THEN
     *
     * @note
     *
     */
    explicit ThreadPool();

    /**
     * @brief Destructor.
     *
     * @details
     * Ensures that all threads are stopped and joined before destruction.
     *
     * @note
     * Calls `stop()` automatically for safety, even if the user forgets.
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
     * @brief Starts
     *
     * @param
     *
     * @details
     * GIVEN
     * WHEN
     * THEN
     *
     * @note
     *
     */
    void start(size_t number_workers = std::thread::hardware_concurrency());

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
     * @note
     *
     */
    void enqueue(std::unique_ptr<IJob> job);

    /**
     * @brief shutdown
     *
     * @details
     * GIVEN
     * WHEN
     * THEN:
     *
     * @note
     *
     */
    void shutdown();

    /**
     * @brief shutdown
     *
     * @details
     * GIVEN
     * WHEN
     * THEN:
     *
     * @note
     *
     */
    void shutdownNow();

    /**
     * @brief join
     *
     * @details
     * GIVEN
     * WHEN
     * THEN:
     *
     * @note
     *
     */
    void join();

    /**
     * @brief join
     *
     * @details
     * GIVEN
     * WHEN
     * THEN:
     *
     * @note
     *
     */
    size_t size() const;

        /**
     * @brief join
     *
     * @details
     * GIVEN
     * WHEN
     * THEN:
     *
     * @note
     *
     */
    bool isRunning() const;

    /******************************************************************/

    /* Private Methods */

   private:
    /**
     * @brief
     *
     * @param
     *
     * @details
     *
     * @note
     *
     */
    void threadLoop(const std::string& worker_name);

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief Jobs queue
     *
     * @details
     *
     */
    JobQueue queue;

    /**
     * @brief
     *
     * @details
     *
     */
    std::atomic<bool> running;

    /**
     * @brief
     *
     * @details
     *
     */
    std::vector<std::thread> threads;

    /******************************************************************/
};
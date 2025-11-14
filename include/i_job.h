/**
 * @file        i_job.h
 * @author      Sergio Guerrero Blanco
 * @date        2025-10-27
 * @version     1.0.0
 *
 * @brief       Interface definition for executable job units.
 *
 * @details
 * This file declares the abstract interface that all job types must implement
 * in order to be processed by the JobQueue and executed by the ThreadPool.
 *
 * Each job represents a unit of work that is executed asynchronously by a
 * worker thread. The interface enforces a minimal contract: every job must
 * provide an `execute()` method containing its logic.
 *
 * The interface is intentionally lightweight, allowing flexibility in defining
 * different types of jobs (logging tasks, computation tasks, delayed actions,
 * etc.). Job objects are typically owned by `std::unique_ptr<IJob>` and
 * enqueued into a thread-safe JobQueue.
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

/*****************************************************************************/

/**
 * @class IJob
 * @brief Abstract interface representing a unit of executable work.
 *
 * @details
 * Any task that can be executed by the ThreadPool must inherit from this class.
 * The `execute()` method is called by worker threads inside a safe loop that
 * catches exceptions to avoid crashing the worker.
 *
 * Example:
 * @code
 * class MyJob : public IJob {
 * public:
 *     void execute() override {
 *         // Do some work...
 *     }
 * };
 * @endcode
 *
 * @note Jobs must be heap-allocated and owned via std::unique_ptr<IJob>.
 */
class IJob
{
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Virtual destructor for safe polymorphic cleanup.
     *
     * @details
     * Ensures derived job objects are properly destroyed when accessed through a
     * base `IJob*` pointer.
     */
    virtual ~IJob() = default;

    /**
     * @brief Executes the job's logic.
     *
     * @details
     * This function is called by worker threads. Derived classes should provide
     * the actual operation that needs to be performed. The function must not
     * throw exceptions out of the implementation — if it does, the ThreadPool
     * catches them and continues running.
     */
    virtual void execute() = 0;

    /******************************************************************/
};
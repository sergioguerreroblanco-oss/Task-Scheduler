/**
 * @file        print_job.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        2025-10-27
 * @version     1.0.0
 *
 * @brief Definition of PrintJob, a simple job that logs a message when executed.
 *
 * @details
 * PrintJob is a concrete implementation of the IJob interface.
 * Its purpose is to provide a lightweight job that writes a message
 * to the logging subsystem when executed. This makes it useful for
 * debugging, demonstrations, and unit-testing the thread pool behavior.
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

#include <string>

/* Project libraries */

#include "i_job.h"

/*****************************************************************************/

/**
 * @class PrintJob
 * @brief A job that prints a message to the logger when executed.
 *
 * @details
 * PrintJob encapsulates a string message provided at construction time.
 * When the job is executed, the message is forwarded to the Logger at
 * INFO level.
 *
 * This job is intentionally simple and is often used as:
 *  - a demonstration for how jobs behave in the system,
 *  - a placeholder job during development,
 *  - a unit-test target to verify that the thread pool executes tasks.
 */
class PrintJob : public IJob
{
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @brief Constructs a PrintJob with the given message.
     *
     * @param msg The message that will be printed when the job executes.
     *
     * @note The message is copied internally and stored as part of the job.
     */
    explicit PrintJob(const std::string& msg);

    /**
     * @brief Executes the job by printing the stored message.
     *
     * @details
     * Logs the message at INFO level using the Logger subsystem.
     *
     * Example output:
     * @code
     * [INFO] PrintJob executed: Hello world
     * @endcode
     */
    void execute() override;

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @brief Message to be printed upon execution.
     */
    std::string msg;

    /******************************************************************/
};
/**
 * @file        print_job.cpp
 * @author      Sergio Guerrero Blanco
 * @date        2025-10-27
 * @version     1.0.0
 *
 * @brief Implementation of PrintJob.
 *
 * @details
 * This file contains the implementation of the PrintJob class, a simple
 * job that logs a predefined message when executed. It acts as a minimal
 * IJob example used throughout the project and in unit tests.
 */

/*****************************************************************************/

/* Standard libraries */

/* Project libraries */

#include "print_job.h"

#include "logger.h"

/*****************************************************************************/

/**
 * @brief Constructs a PrintJob with the specified message.
 *
 * @param msg The message that will be logged when the job is executed.
 */
PrintJob::PrintJob(const std::string& msg) : msg(msg) {}

/**
 * @brief Executes the job by logging its message.
 *
 * @details
 * Logs the following template message:
 * @code
 * PrintJob executed: <message>
 * @endcode
 */
void PrintJob::execute()
{
    Logger::info("PrintJob executed: " + msg);
}
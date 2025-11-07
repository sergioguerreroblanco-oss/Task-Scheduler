/**
 * @file        print_job.cpp
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

#include "print_job.h"

#include "logger.h"

/*****************************************************************************/

/**
 * @brief
 * @param
 *
 * @details
 */
PrintJob::PrintJob(const std::string& msg) : msg(msg) {}

/**
 * @brief
 * @param
 *
 * @details
 */
void PrintJob::execute()
{
    Logger::info("PrintJob executed: " + msg);
}
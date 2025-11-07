/**
 * @file        print_job.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-10-27>
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

#include <string>

/* Project libraries */

#include "i_job.h"

/*****************************************************************************/

/**
 * @class PrintJob
 * @brief
 * @tparam
 */
class PrintJob : public IJob
{
    /******************************************************************/

    /* Public Methods */

   public:
    /**
     * @details
     */
    explicit PrintJob(const std::string& msg);

    /**
     * @details
     */
    void execute() override;

    /******************************************************************/

    /* Private Attributes */

   private:
    /**
     * @details
     */
    std::string msg;

    /******************************************************************/
};
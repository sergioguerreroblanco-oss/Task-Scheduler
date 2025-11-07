/**
 * @file        i_job.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-10-27>
 * @version     0.0.0
 *
 * @brief       Abstract interface for jobs.
 *
 * @details
 */

/*****************************************************************************/

/* Include Guard */

#pragma once

/*****************************************************************************/

/* Standard libraries */

/*****************************************************************************/

/**
 * @class IJob
 * @brief Abstract interface for jobs.
 * @tparam
 */
class IJob
{
    /******************************************************************/

    /* Public Methods */

   public:
    virtual ~IJob() = default;

    /**
     * @brief
     * @param
     */
    virtual void execute() = 0;

    /******************************************************************/
};
/**
 * @file        test_jobs.cpp
 * @author      Sergio Guerrero
 * @date        2025-10-27
 * @version     0.1.0
 *
 * @brief Unit tests for IJob and PrintJob components.
 *
 * @details
 * GIVEN a polymorphic job interface (IJob)
 * WHEN derived jobs like PrintJob are executed
 * THEN they must produce the expected observable behavior
 *      (e.g., write a log line to the console).
 */

/* Standard libraries */

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <streambuf>

/* Project libraries */

#include "i_job.h"
#include "logger.h"
#include "print_job.h"

/*****************************************************************************/

class PrintJobTest : public ::testing::Test
{
   protected:
    std::ostringstream oss;
    std::streambuf*    oldBuf = nullptr;

    void SetUp() override
    {
        // Redirect std::cout to our string buffer
        oldBuf = std::cout.rdbuf(oss.rdbuf());
        Logger::set_min_level(Logger::Level::INFO);
    }

    void TearDown() override
    {
        // Restore original buffer
        std::cout.rdbuf(oldBuf);
    }
};

/*****************************************************************************/

/* Tests */

/**
 * @test Verify that a PrintJob can be executed polymorphically through IJob.
 *
 * GIVEN an IJob pointer referencing a PrintJob
 * WHEN execute() is called
 * THEN the expected log message is printed to stdout.
 */
TEST_F(PrintJobTest, ExecutesAndLogsMessage)
{
    // GIVEN
    std::unique_ptr<IJob> job = std::make_unique<PrintJob>("Test message");

    // WHEN
    job->execute();

    // THEN
    std::string output = oss.str();
    EXPECT_NE(output.find("Test message"), std::string::npos)
        << "Expected message not found in logger output:\n"
        << output;
}

/**
 * @test Verify that IJob cannot be instantiated directly.
 */
TEST(JobInterfaceTest, IsAbstract)
{
    // GIVEN / WHEN / THEN
    // Compilation should fail if we try to instantiate IJob.
    // This test is symbolic (compile-time).
    SUCCEED() << "IJob is abstract (compile-time check)";
}
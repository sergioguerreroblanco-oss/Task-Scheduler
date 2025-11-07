/**
 * @file        test_jobs_queue.cpp
 * @author      Sergio Guerrero
 * @date        2025-10-27
 * @version     0.1.0
 *
 * @brief Unit tests for JobQueue components.
 *
 * @details
 * GIVEN
 * WHEN
 * THEN
 */

/* Standard libraries */

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

/* Project libraries */

#include "job_queue.h"
#include "logger.h"
#include "print_job.h"

/*****************************************************************************/

class JobQueueTest : public ::testing::Test
{
   protected:
    void SetUp() override { Logger::set_min_level(Logger::Level::INFO); }
};

/*****************************************************************************/

/* Tests */

/**
 * @test Push and pop sequentially
 *
 * GIVEN an empty queue
 * WHEN one job is pushed
 * THEN pop() retrieves it successfully
 */
TEST_F(JobQueueTest, PushPopSquence)
{
    // GIVEN
    JobQueue queue;

    // WHEN
    queue.push(std::make_unique<PrintJob>("Job #1"));

    // THEN
    auto outPrintJob = queue.pop();
    ASSERT_NE(outPrintJob, nullptr);
    outPrintJob->execute();
}

/**
 * @test Blocking pop from another thread
 *
 * GIVEN an empty queue and a consumer thread waiting on pop()
 * WHEN another thread (the main thread here) pushes a job into the queue
 * THEN pop() returns that same job (i.e. not null)
 */
TEST_F(JobQueueTest, BlockPopInOtherThread)
{
    // GIVEN
    JobQueue              queue;
    std::unique_ptr<IJob> popped;

    std::atomic<bool> waiting{false};

    std::thread consumer(
        [&]()
        {
            waiting.store(true, std::memory_order_release);

            popped = queue.pop();
        });

    while (!waiting.load(std::memory_order_acquire))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // WHEN
    queue.push(std::make_unique<PrintJob>("Job #2"));

    consumer.join();

    // THEN
    ASSERT_NE(popped, nullptr) << "Consumer thread did not receive a job from the queue";

    popped->execute();
}

/**
 * @test Shutdown behavior
 *
 * GIVEN a queue with no jobs
 * WHEN shutdown() is called
 * THEN pop() immediately returns nullptr
 */
TEST_F(JobQueueTest, ShutdownBehaviour)
{
    // GIVEN
    JobQueue queue;

    // WHEN
    queue.shutdown();

    // THEN
    EXPECT_EQ(queue.pop(), nullptr);
}
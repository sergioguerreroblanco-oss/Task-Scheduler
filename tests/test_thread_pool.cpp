/*
 * @file        test_thread_pool.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        2025-11-10
 * @version     0.0.0
 *
 * @brief Unit tests for ... components.
 *
 * @details

 *
 * The tests follow the GIVEN / WHEN / THEN documentation pattern:
 *  - GIVEN:
 *  - WHEN:
 *  - THEN:
 */

/* Standard libraries */

#include <gtest/gtest.h>
#include <chrono>

/* Project libraries */

#include "fake_job.h"
#include "fake_slow_job.h"
#include "fake_throwing_job.h"
#include "thread_pool.h"
#include "logger.h"

/*****************************************************************************/

class ThreadPoolTest : public ::testing::Test
{
   protected:
    void SetUp() override { Logger::set_min_level(Logger::Level::INFO); }
};

/*****************************************************************************/

/* Tests */

/**
 * @test
 * @brief
 *
 * @details
 * GIVEN a pool
 * WHEN start(3)
 * THEN threads.size() == 3
 */
TEST_F(ThreadPoolTest, StartsCorrectNumberOfThreads)
{
    // GIVEN
    ThreadPool tPool;

    // WHEN
    tPool.start(3);

    // THEN
    EXPECT_EQ(tPool.size(), 3);
}

/**
 * @test
 * @brief
 *
 * @details
 * GIVEN a pool started with 1 thread
 * WHEN enqueue a fake job
 * THEN the fake job was executed by the thread
 */
TEST_F(ThreadPoolTest, EnqueueExecutesAJob)
{
    // GIVEN
    ThreadPool tPool;
    tPool.start(1);

    auto job = std::make_unique<FakeJob>();
    FakeJob* rawPtr = job.get();

    // WHEN
    tPool.enqueue(std::move(job));

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    tPool.shutdown();

    // THEN
    ASSERT_TRUE(rawPtr->wasExecuted())
        << "Expected job to be executed by the thread, but it was not.";
}

/**
 * @test
 * @brief
 *
 * @details
 * GIVEN a thread pool started with 3 workers
 * WHEN enqueue a FakeJob
 * THEN the job must be executed (wasExecuted == true)
 */
TEST_F(ThreadPoolTest, EnqueueExecutesJob)
{
    // GIVEN
    ThreadPool tPool;
    tPool.start(3);

    // WHEN
    auto job = std::make_unique<FakeJob>();
    FakeJob* jobPtr = job.get();
    tPool.enqueue(std::move(job));
    const auto timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    while (!jobPtr->wasExecuted() && std::chrono::steady_clock::now() < timeout)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // THEN
    EXPECT_TRUE(jobPtr->wasExecuted()) << "The job was never executed by any worker thread";
    tPool.shutdown();
}

/**
 * @test
 * @brief
 *
 * @details
 * GIVEN a pool
 * WHEN shutdown()
 * THEN workers.size() == 0
 */
TEST_F(ThreadPoolTest, ShutdownCorrectNumberOfThreads)
{
    // GIVEN
    ThreadPool tPool;
    tPool.start(3);

    // WHEN
    tPool.shutdown();

    // THEN
    EXPECT_EQ(tPool.size(), 0) << "Workers vector should be empty after shutdown()";
}

/**
 * @test
 * @brief
 *
 * @details
 * GIVEN un pool con 1 worker
 * AND un FakeSlowJob encolado
 * WHEN shutdownNow() es llamado
 * THEN the job must finish execution but do not accept more jobs
 */
TEST_F(ThreadPoolTest, ShutdownNowStopsImmediately)
{
    // GIVEN
    ThreadPool tPool;
    tPool.start(1);

    // WHEN
    auto job = std::make_unique<FakeSlowJob>();
    FakeSlowJob* jobPtr = job.get();
    tPool.enqueue(std::move(job));

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // AND
    tPool.shutdownNow();

    // THEN
    EXPECT_TRUE(jobPtr->hasStarted());
    EXPECT_EQ(tPool.size(), 0);
}

/**
 * @test
 * @brief Workers must survive exceptions thrown by jobs.
 *
 * @details
 * GIVEN a thread pool with 1 worker
 * AND a FakeThrowingJob followed by a normal FakeJob
 * WHEN both are enqueued ,executed and after that the thread pool is shutted down
 * THEN:
 *  - The throwing job must mark executed == true
 *  - The next job must still be executed (worker is still alive)
 *  - No crash must occur
 */
TEST_F(ThreadPoolTest, WorkerSurvivesExceptionAndContinues)
{
    // GIVEN
    ThreadPool tPool;
    tPool.start(1);

    auto throwingJob = std::make_unique<FakeThrowingJob>();
    FakeThrowingJob* throwingPtr = throwingJob.get();

    auto normalJob = std::make_unique<FakeJob>();
    FakeJob* normalPtr = normalJob.get();

    // WHEN
    tPool.enqueue(std::move(throwingJob));
    tPool.enqueue(std::move(normalJob));

    const auto timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
    while ((!throwingPtr->wasExecuted() || !normalPtr->wasExecuted()) &&
           std::chrono::steady_clock::now() < timeout)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    tPool.shutdown();

    // THEN
    ASSERT_TRUE(throwingPtr->wasExecuted()) << "The FakeThrowingJob was executed and stopped due to the exception - continue with the next job";

    ASSERT_TRUE(normalPtr->wasExecuted())
        << "The thread stopped after the previous exception — it continued and executed the next job";
}
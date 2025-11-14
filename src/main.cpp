/**
 * @file        main.cpp
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        <2025-11-14>
 * @version     1.0.0
 *
 * @brief Entry point for the Task Scheduler demo application.
 *
 * @details
 * This program demonstrates:
 *  - Starting a ThreadPool with configurable number of threads
 *  - Enqueuing jobs (PrintJob)
 *  - Graceful shutdown()
 *  - Immediate shutdownNow()
 *
 * CLI options:
 *   --threads N              Number of threads (default: HW concurrency)
 *   --demo                   Enqueue several PrintJobs
 *   --slow                   Enqueue slow jobs (requires FakeSlowJob)
 *   --immediate-shutdown     Stop immediately (shutdownNow)
 */

#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "../tests/fake_job.h"
#include "../tests/fake_slow_job.h"
#include "logger.h"
#include "print_job.h"
#include "thread_pool.h"

int main(int argc, char** argv)
{
    Logger::set_min_level(Logger::Level::INFO);

    size_t nThreads          = std::thread::hardware_concurrency();
    bool   runDemo           = false;
    bool   runSlow           = false;
    bool   immediateShutdown = false;

    // --------------------------
    // Parse CLI arguments
    // --------------------------
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--threads" && i + 1 < argc)
        {
            nThreads = std::stoul(argv[++i]);
        }
        else if (arg == "--demo")
        {
            runDemo = true;
        }
        else if (arg == "--slow")
        {
            runSlow = true;
        }
        else if (arg == "--immediate-shutdown")
        {
            immediateShutdown = true;
        }
        else if (arg == "--help")
        {
            std::cout << "TaskScheduler usage:\n"
                      << "  --threads N            Number of threads\n"
                      << "  --demo                 Run simple PrintJob demo\n"
                      << "  --slow                 Run FakeSlowJob demo\n"
                      << "  --immediate-shutdown   Demonstrate shutdownNow()\n";
            return 0;
        }
        else
        {
            std::cerr << "Unknown argument: " << arg << "\n";
            return 1;
        }
    }

    // --------------------------
    // Create and start ThreadPool
    // --------------------------
    Logger::info("[Main] Creating thread pool with " + std::to_string(nThreads) + " threads");

    ThreadPool pool;
    pool.start(nThreads);

    // --------------------------
    // Run desired mode
    // --------------------------
    if (runDemo)
    {
        Logger::info("[Main] Running PrintJob demo...");

        for (int i = 0; i < 5; ++i)
        {
            pool.enqueue(std::make_unique<PrintJob>("Task " + std::to_string(i)));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    if (runSlow)
    {
        Logger::info("[Main] Running FakeSlowJob demo...");

        for (int i = 0; i < 3; ++i)
        {
            pool.enqueue(std::make_unique<FakeSlowJob>());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // --------------------------
    // Shutdown mode
    // --------------------------
    if (immediateShutdown)
    {
        Logger::warn("[Main] Calling shutdownNow()");
        pool.shutdownNow();
    }
    else
    {
        Logger::info("[Main] Calling graceful shutdown()");
        pool.shutdown();
    }

    Logger::info("[Main] Exiting program.");
    return 0;
}

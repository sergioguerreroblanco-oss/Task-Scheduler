/*
 * @file        fake_slow_job.h
 * @author      Sergio Guerrero Blanco <sergioguerreroblanco@hotmail.com>
 * @date        2025-11-13
 * @version     0.0.0
 *
 * @brief
 *
 * @details
 */
#pragma once
#include <atomic>
#include <thread>

#include "i_job.h"

class FakeSlowJob : public IJob
{
   public:
    FakeSlowJob() : started(false), finished(false) {}

    void execute() override
    {
        started.store(true, std::memory_order_release);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        finished.store(true, std::memory_order_release);
    }

    bool hasStarted() const { return started.load(std::memory_order_acquire); }
    bool hasFinished() const { return finished.load(std::memory_order_acquire); }

   private:
    std::atomic<bool> started;
    std::atomic<bool> finished;
};
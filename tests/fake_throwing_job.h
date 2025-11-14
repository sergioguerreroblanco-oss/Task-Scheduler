/*
 * @file        fake_throwing_job.h
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
#include <stdexcept>

#include "i_job.h"

class FakeThrowingJob : public IJob
{
   public:
    FakeThrowingJob() : executed(false) {}

    void execute() override
    {
        executed.store(true, std::memory_order_release);
        throw std::runtime_error("Fake job error");
    }

    bool wasExecuted() const { return executed.load(std::memory_order_acquire); }

   private:
    std::atomic<bool> executed;
};
#pragma once
#include <iostream>
#include <thread>
#include <condition_variable>
#include "LoggerFactory.h"
#include "util/ThreadSafeQueue.h"
#include "content-writer/BaseContentWriter.h"
#include <cstdlib>

class Logger
{
private:
    std::thread logThread;
    std::condition_variable logCondition;
    bool stopThread;
    LogStrategy *lf;
    mutable std::mutex mutex_;

    // Private constructor to prevent instantiation
    Logger() : stopThread(false)
    {
        // TODO: Read this from a configuration file
        std::string loggerType = "";
        if (std::getenv("LOGGER_TYPE")) {
            loggerType = std::string(std::getenv("LOGGER_TYPE"));
        }
        /**
         * Creating via LoggerFactory, creates instance of BaseContentWriter,
         * which will write to output, when the queue size becomes 10
         */
        lf = LoggerFactory::CreateLogger(loggerType);
        if (!loggerType.empty())
            logThread = std::thread(&Logger::logWorker, this);
    }

    ~Logger()
    {
        stopThread = true;
        logCondition.notify_one();
        logThread.join();
    }

    // Private copy constructor and assignment operator to prevent copying
    Logger(const Logger &);
    Logger &operator=(const Logger &);

    // Static instance of the class
    inline static Logger *instance = nullptr;

    void logWorker()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            
            // Wait until there is a log message or the thread is stopping
            logCondition.wait(lock, [this]
                              { return !lf->Empty() || stopThread; });

            // Check if the thread is stopping
            if (stopThread && lf->Empty())
            {
                break;
            }

            lf->Flush();
        }
    }

public:
    // Public method to get the instance of the class
    static Logger *getInstance()
    {
        if (!instance)
        {
            instance = new Logger();
        }
        return instance;
    }

    void Log(std::string app, std::string key, std::string cause)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        lf->Log(app, key, cause);
        lock.unlock();
        logCondition.notify_one();
    }
};

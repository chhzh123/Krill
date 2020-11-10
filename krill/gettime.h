// This code is part of the project "Krill"
// Copyright (c) 2019 Hongzheng Chen

#ifndef GETTIME_H
#define GETTIME_H

#include <iomanip>
#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>

using Clock = std::chrono::high_resolution_clock;

class Timer
{
public:
    inline void start()
    {
        _start_time = Clock::now();
        flag = true;
    }
    inline void stop()
    {
        _end_time = Clock::now();
        flag = false;
    }
    inline double elapsedMilliseconds()
    {
        std::chrono::time_point<Clock> endTime;
        if(flag)
        {
            endTime = Clock::now();
        }
        else
        {
            endTime = _end_time;
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - _start_time).count();
    }
    void reportT(double time)
    {
        std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(4) << time << "s" << std::endl;
    }
    inline double elapsedSeconds()
    {
        return elapsedMilliseconds() / 1000.0;
    }
    inline double reportNext(std::string str)
    {
        std::cout << str << ": ";
        reportT(elapsedSeconds());
    }
    inline double reportTotal(std::string str)
    {
        stop();
        std::cout << str << ": ";
        reportT(elapsedSeconds());
    }

private:
    std::chrono::time_point<Clock> _start_time;
    std::chrono::time_point<Clock> _end_time;
    bool flag = false;
};

static Timer _tm;
#define startTime() _tm.start();
#define reportTime(_str) _tm.reportTotal(_str);
#define reportNs() _tm.elapsedMilliseconds();
#define reportSec() _tm.elapsedSeconds();
#define nextTime(_str) _tm.reportNext(_str);

#endif // GETTIME_H
#pragma once

#include <vector>
#include <thread>

#include <range/v3/all.hpp>

namespace par{

template<class Result, class... Args>
class SubCalc{
public:
    SubCalc(std::function<Result(Args...)> const& f, Args... args)
        : func_(f)
        , args_(std::forward<Args>(args)...)
    {}

    void calc()
    {
        result_ = func_(args_)
    }

private:
    std::function<Result(Args...)> func_;
    Result result_;
    std::tuple<Args...> args_;
}

class CalcStep {
    CalcStep(std::vector<SubCalc> subCalcs)
    :  subCalcs_(subCalcs)
    {
        // put shared_ptrs of sub calcultions onto the queue in reverse order
        for(auto& subCalc : subCalcs_ | ranges::views::reverse)
        {
            std::shared_ptr<SubCalc> ptr = nullptr;
            ptr.reset(&subCalc);
            queue_.push(ptr);
        }
    }

    std::shared_ptr<SubCalc> pop() noexcept
    {
        std::unique_lock lock(mutex_);
        if(queue_.empty())
            return nullptr;
        auto subCalc = queue_.top();
        queue_.pop();
        return subCalc;
    }

    void calc()
    {
        std::shared_ptr<SubCalc> subCalc = nullptr;
        while( subCalc = pop(); subCalc)
            subCalc->calc();
    }

private:
    std::mutex mutex_;
    // independent sub calculations
    std::vector<SubCalc> subCalcs_;
    std::stack<std::shared_ptr<SubCalc>> queue_;
}


class Calculation{
public:
    Calculation(size_t numThreads) 
        : numThreads_(numThreads)
    {}

    // fill calculations
    virtual void init() = 0;

    void calculate()
    {
        for(auto& calculation : calculations_)
        {
            std::vector<std::thread> ts{numThreads_};
            std::generate(ts.begin(), ts.end(), [&](){ return std::thread{[&](){ calculation.calc(); }}});
            for(auto& t : ts)
                t.join();
        }
    }
private:
    size_t numThreads_ = 1;
    std::vector<CalcStep> calculations_;
}

}
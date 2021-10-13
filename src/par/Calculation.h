#pragma once

#include <vector>
#include <thread>

#include <range/v3/all.hpp>

namespace par{

template<class Result, class... Args>
class SubCalculation{
public:
    SubCalculation(std::function<Result(Args...)> const& f, Args... args)
        : func_(f)
        , args_(std::forward<Args>(args)...)
    {}
    SubCalculation() = default;
    SubCalculation(SubCalculation const&) = default; 
    SubCalculation& operator=(SubCalculation const&) = default;

    void calc()
    {
        result_ = func_(args_);
    }

    Result& result() { return result_; }
    std::tuple<Args...>& args() { return args_; }

private:
    std::function<Result(Args...)> func_;
    Result result_;
    std::tuple<Args...> args_;
};


template<class SubCalc>
class CalcStep {
public:
    CalcStep(std::vector<SubCalc> const& subCalcs)
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
        for(;;)
        {
            auto subCalc = pop();
            if(not subCalc)
                break;
            subCalc->calc();
        }
    }

    std::vector<SubCalc>& subCalcs() { return subCalcs_; }

private:
    std::mutex mutex_;
    // independent sub calculations
    std::vector<SubCalc> subCalcs_;
    std::stack<std::shared_ptr<SubCalc>> queue_;
};


template<class SubCalc>
class Calculation{
public:
    Calculation(size_t numThreads) 
        : numThreads_(numThreads)
    {}

    virtual void initCalculations() = 0;
    virtual void initTransformations() = 0;

    // fill calculations
    void init()
    {
        initCalculations();
        initTransformations();
    }

    void calculate()
    {
        size_t i = 0;
        for(auto& calculation : calculations_)
        {
            std::vector<std::thread> ts{numThreads_};
            std::generate(ts.begin(), ts.end(), [&](){ return std::thread{[&](){ calculation.calc();}}; });
            for(auto& t : ts)
                t.join();
            if(i < calculations_.size() - 1)
                transformations_[i++]();
        }
    }
protected:
    size_t numThreads_ = 1;
    std::vector<CalcStep<SubCalc>> calculations_;
    // index 0 handles transformation from calculations_[0] to calculations_[1]
    std::vector<std::function<void()>> transformations_;
};

}
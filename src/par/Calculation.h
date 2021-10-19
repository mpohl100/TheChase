#pragma once

#include <vector>
#include <map>
#include <thread>
#include <tuple>

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
    SubCalculation(SubCalculation&&) = default; 
    SubCalculation& operator=(SubCalculation&&) = default;

    Result calc() const
    {
        return std::apply(func_, args_);
    }

    std::tuple<Args...>& args() { return args_; }
    using result_t = Result;
private:
    std::function<Result(Args...)> func_;
    std::tuple<Args...> args_;
};


template<class SubCalc>
class CalcStep {
public:
    CalcStep(std::vector<SubCalc> const& subCalcs)
    {
        subCalcs_ = subCalcs;
        // put shared_ptrs of sub calcultions onto the queue in reverse order
        for(const auto& subCalc : subCalcs_ | ranges::views::reverse)
        {
            queue_.push(&subCalc);
        }
    }

    CalcStep() = default;
    CalcStep(CalcStep const&) = default;
    CalcStep& operator=(CalcStep const&) = default;

    std::pair<const SubCalc*,SubCalc> pop() noexcept
    {
        std::lock_guard<std::mutex> lock(*mutex_);
        if(queue_.empty())
            return {nullptr, {}};
        auto subCalc = queue_.top();
        queue_.pop();
        return {subCalc, *subCalc};
    }

    void setResult(const SubCalc* subCalc, typename SubCalc::result_t const& result)
    {
        std::lock_guard<std::mutex> lock(*mutex_);
        results_[subCalc] = result;
    }

    void calc()
    {
        for(;;)
        {
            std::pair<const SubCalc*, SubCalc> subCalc = pop();
            if(not subCalc.first)
                break;
            auto result = subCalc.second.calc();
            setResult(subCalc.first, result);
        }
    }

    std::vector<SubCalc>& subCalcs() { return subCalcs_; }
    std::vector<SubCalc> const& subCalcs() const { return subCalcs_; }
    typename SubCalc::result_t result(const SubCalc* subCalc) const 
    { 
        auto it = results_.find(subCalc);
        if(it != results_.end())
            return it->second;
        return {};  
    }

private:
    std::shared_ptr<std::mutex> mutex_ = std::make_shared<std::mutex>();
    // independent sub calculations
    std::vector<SubCalc> subCalcs_;
    std::stack<const SubCalc*> queue_;
    // index in subCalcs_, Result
    std::map<const SubCalc*, typename SubCalc::result_t> results_;
};


template<class SubCalc>
class Calculation{
public:
    Calculation(size_t numThreads) 
        : numThreads_(numThreads)
    {}

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

    virtual void initCalculations() = 0;
    virtual void initTransformations() = 0;

    size_t numThreads_ = 1;
    std::vector<CalcStep<SubCalc>> calculations_;
    // index 0 handles transformation from calculations_[0] to calculations_[1]
    std::vector<std::function<void()>> transformations_;
};

}
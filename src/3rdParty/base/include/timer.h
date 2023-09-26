#ifndef TIMER_H_
#define TIMER_H_

#include<chrono>
#include<string>
namespace base
{

class timer
{
    typedef std::chrono::steady_clock::time_point   tp;
    typedef std::chrono::duration<double>           dd;
    typedef std::chrono::steady_clock               sc;
private:
	auto _begin;
    dd _span;
public:
    timer()
        : _begin(tp()), _span(dd(0)){}
    void start()
    {
        _begin = sc::now();
		auto a = sc::now();
    }

    void pause()
    {
        tp _end = sc::now();
        _span += std::chrono::duration_cast<dd>(_end - _begin);
    }

    void stop(std::string head = std::string(),
            std::string tail = std::string())
    {
        tp _end = sc::now();
        _span += std::chrono::duration_cast<dd>(_end - _begin);
        //std::cout << head << _span.count() << " seconds" << tail << std::endl;
        _span = dd(0);
    }
    ~timer()
    {}
};

} // namespace

#endif // TIMER_H_
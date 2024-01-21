#include <thread>

#ifndef C8TIMER_H
#define C8TIMER_H

class C8Timer
{
public:
    void setTimerval(unsigned char newval);
    unsigned char getTimerval() const;
    C8Timer();
private:
    static constexpr unsigned int MUS_INTERVAL = 16670;
    void decrement();
    unsigned char timerval = 0;
    std::thread decrementthread;
};

#endif // C8TIMER_H

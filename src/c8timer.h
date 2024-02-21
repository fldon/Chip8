#ifndef C8TIMER_H
#define C8TIMER_H

class C8Timer
{
public:
    void setTimerval(unsigned char newval);
    unsigned char getTimerval() const;
    void decrement(unsigned int cycles);
private:
    static constexpr unsigned int CYCLES_PER_SECOND = 500; //TODO: Find out a reasonable value for this based on the COMPAC VP

    unsigned char timervalSecs = 0;
    unsigned int cyclesSinceLastSec{0};
};

#endif // C8TIMER_H

#include "c8timer.h"
/*
 * Passes the given amount of cycles in time
 * If cycles are enough to pass a second, decrement the actual timer value by 1
 * Do not "throw away" cycles when that happens
 * */
void C8Timer::decrement(unsigned int cycles)
{//every 1/60 second interval, decrement the timerval (if above 0), then sleep for 1/60 of a second
    cyclesSinceLastSec += cycles;
    if(cyclesSinceLastSec > CYCLES_PER_TIMER_DECREASE)
    {
       if(timervalSecs > 0)
       {
           timervalSecs--;
       }
       cyclesSinceLastSec = cyclesSinceLastSec - CYCLES_PER_TIMER_DECREASE; //keep remainder
    }
}

unsigned char C8Timer::getTimerval() const
{
   return timervalSecs;
}

void C8Timer::setTimerval(unsigned char newval)
{
   timervalSecs = newval;
}

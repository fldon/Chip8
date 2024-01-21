#include "c8timer.h"

//spawn new thread for timer decrement
C8Timer::C8Timer()
   :decrementthread(&C8Timer::decrement, this)
{
}

void C8Timer::decrement()
{//every 1/60 second interval, decrement the timerval (if above 0), then sleep for 1/60 of a second
   while(true)
   {
       if(timerval > 0)
       {
           timerval--;
       }
       std::this_thread::sleep_for(std::chrono::microseconds(MUS_INTERVAL));
   }
}


unsigned char C8Timer::getTimerval() const
{
   return timerval;
}

void C8Timer::setTimerval(unsigned char newval)
{
   timerval = newval;
}

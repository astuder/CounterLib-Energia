/*
CounterLib_t.h - Simple counter library for Energia with MSP430 microcontrollers.
Created by Adrian Studer, August 2015.

Distributed under MIT License, see license.txt for details.
*/

#ifndef CounterLib_T_h
#define CounterLib_T_h

#include <inttypes.h>
#include <Energia.h>

enum CL_TIMER_t
{
  CL_TimerA0,    // G2533 P1.0, F5529 P1.0
#if defined(__MSP430F5529)
  CL_TimerA1,    // F5529 P1.6
  CL_TimerA2,    // FF529 P2.2
  CL_TimerB0     // F5529 P7.7 (note: P7.7 not broken out on LP, also available via port mapping as PM_TB0CLK)
#endif
};

template <CL_TIMER_t timer = CL_TimerA0>
struct Counter {

public:
  void start(void)
  {
#if defined(__MSP430G2553)
    if(timer == CL_TimerA0)
    {
      // set pin 1.0 as input, select function TA0CLK
      P1DIR &= ~BIT0;
      P1SEL |= BIT0;
      P2SEL &= BIT0;
      
      // configure timer A0 to count upwards based on TA0CLK input
      // halt timer, disable interrupts, TACLK clock source, reset counter
      TACTL = TACLR;  // clears MCx, IDx, TAIE, TASSEL
 
      // start timer in continuous mode
      TACTL |= MC1;
    }
#elif defined(__MSP430F5529)
    if(timer == CL_TimerA0)
    {
      // set pin 1.0 as input, select function TA0CLK
      P1DIR &= ~BIT0;
      P1SEL |= BIT0;
      
      // configure timer A0 to count upwards based on TA0CLK input
      // halt timer, disable interrupts, TACLK clock source, reset counter
      TA0CTL = TACLR;  // clears MCx, IDx, TAIE, TASSEL
 
      // start timer in continuous mode
      TA0CTL |= MC1;
    }
#else
  #error This microcontroller is not supported by CounterLib.
#endif
  }

  void stop(void)
  {
#if defined(__MSP430G2553)
    if(timer == CL_TimerA0)
    {
      // halt timer, don't reset counter
      TACTL &= ~(MC0+MC1);  // clears MCx
    }
#elif defined(__MSP430F5529)
    if(timer == CL_TimerA0)
    {
      // halt timer, don't reset counter
      TA0CTL &= ~(MC0+MC1);  // clears MCx
    }
#else
  #error This microcontroller is not supported by CounterLib.
#endif
  }

  void reset(void)
  {
#if defined(__MSP430G2553)
    if(timer == CL_TimerA0)
    {
      // reset counter
      TACTL |= TACLR;
    }
#elif defined(__MSP430F5529)
    if(timer == CL_TimerA0)
    {
      // reset counter
      TA0CTL |= TACLR;
    }
#else
  #error This microcontroller is not supported by CounterLib.
#endif
  }

  uint16_t read(void)
  {
#if defined(__MSP430G2553)
    if(timer == CL_TimerA0)
    {
      return TAR;
    }
#elif defined(__MSP430F5529)
    if(timer == CL_TimerA0)
    {
      return TA0R;
    }
#else
  #error This microcontroller is not supported by CounterLib.
#endif
  }

  uint16_t readAndReset(void)
  {
    uint16_t counter_value;
#if defined(__MSP430G2553)
    if(timer == CL_TimerA0)
    {      
      counter_value = TAR;    // store counter value
      TACTL |= TACLR;         // reset counter
      return counter_value;   // return counter value
    }
#elif defined(__MSP430F5529)
    if(timer == CL_TimerA0)
    {      
      counter_value = TA0R;   // store counter value
      TA0CTL |= TACLR;        // reset counter
      return counter_value;   // return counter value
    }
#else
  #error This microcontroller is not supported by CounterLib.
#endif
  }

private:

};  // end of struct/template Counter

#endif  // CounterLib_T_h

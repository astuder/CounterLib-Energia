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

enum CL_DIVIDER_t
{
  CL_Div1 = 0,        // divide by 1
  CL_Div2 = ID0,      // divide by 2
  CL_Div4 = ID1,      // divide by 4
  CL_Div8 = ID0 | ID1 // divide by 8 
};

template <CL_TIMER_t timer = CL_TimerA0>
struct Counter {

public:
#if defined(__MSP430G2553)
  void start(CL_DIVIDER_t divider1 = CL_Div1)
#elif defined(__MSP430F5529)
  void start(CL_DIVIDER_t divider1 = CL_Div1, uint8_t divider2 = 0)
#else
  #error This microcontroller is not supported by CounterLib.
#endif
  {
    uint16_t divider1_bits = divider1;    // basic divider can be 1, 2, 4 or 8
#if defined(__MSP430F5529)
    uint16_t divider2_bits = (divider2 - 1) & 0x07;  // extended divider can be 1 to 8
#endif

#if defined(__MSP430G2553)
    if(timer == CL_TimerA0)
    {
      // set pin 1.0 as input, select function TA0CLK
      P1DIR &= ~BIT0;
      P1SEL |= BIT0;
      P2SEL &= BIT0;
      
      // configure timer A0 to count upwards based on TA0CLK input
      // halt timer, disable interrupts, TACLK clock source, reset counter
      TACTL = TACLR | divider1_bits;  // clears MCx, TAIE, TASSEL
 
      // start timer in continuous mode
      TACTL |= MC1;
    }
#elif defined(__MSP430F5529)
    switch(timer)
    {
      // configure timer to count upwards using timer CLK input from pin
      // 1. setup pin
      // 2. halt timer, disable interrupts, TA0CLK clock source, reset counter
      // 3. start timer in continuous mode
      case CL_TimerA0:    // timer A0
      default:
        P1DIR &= ~BIT0;   // set pin 1.0 as input
        P1SEL |= BIT0;    // select function TA0CLK
        TA0CTL = divider1_bits;   // set main divider, clears MCx, TAIE, TASSEL
        TA0EX0 = (TA0EX0 & 0xfff8) | divider2_bits;  // set extended divider
        TA0CTL |= TACLR | MC1;    // reset counter and start timer in continuous mode
        break;

      case CL_TimerA1:    // timer A1
        P1DIR &= ~BIT6;   // set pin 1.6 as input
        P1SEL |= BIT6;    // select function TA1CLK
        TA1CTL = divider1_bits;   // set main divider, clears MCx, TAIE, TASSEL
        TA1EX0 = (TA1EX0 & 0xfff8) | divider2_bits;  // set extended divider
        TA1CTL |= TACLR | MC1;    // reset counter and start timer in continuous mode
        break;

      case CL_TimerA2:    // timer A2
        P2DIR &= ~BIT2;   // set pin 2.2 as input
        P2SEL |= BIT2;    // select function TA2CLK
        TA2CTL = divider1_bits;   // set main divider, clears MCx, TAIE, TASSEL
        TA2EX0 = (TA2EX0 & 0xfff8) | divider2_bits;  // set extended divider
        TA2CTL |= TACLR | MC1;    // reset counter and start timer in continuous mode
        break;
        
      case CL_TimerB0:    // timer B0
        P7DIR &= ~BIT7;   // set pin 7.7 as input
        P7SEL |= BIT7;    // select function TB0CLK
        TB0CTL = divider1_bits;   // set main divider, clears MCx, TAIE, TASSEL
        TB0EX0 = (TB0EX0 & 0xfff8) | divider2_bits;  // set extended divider
        TB0CTL |= TBCLR | MC1;    // reset counter and start timer in continuous mode
        break;
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
      // halt timer, but don't reset counter
      TACTL &= ~(MC0+MC1);  // clears MCx
    }
#elif defined(__MSP430F5529)
    switch(timer)
    {
      // halt timer, but don't reset counter
      case CL_TimerA0:      // timer A0
      default:
        TA0CTL &= ~(MC0+MC1);
        break;
      case CL_TimerA1:      // timer A1
        TA1CTL &= ~(MC0+MC1);
        break;
      case CL_TimerA2:      // timer A2
        TA2CTL &= ~(MC0+MC1);
        break;
      case CL_TimerB0:      // timer B0
        TB0CTL &= ~(MC0+MC1);
        break;
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
      // reset counter to zero, counter keeps running
      TACTL |= TACLR;
    }
#elif defined(__MSP430F5529)
    switch(timer)
    {
      // reset counter to zero, counter keeps running
      case CL_TimerA0:      // timer A0
      default:
        TA0CTL |= TACLR;
        break;
      case CL_TimerA1:      // timer A1
        TA1CTL |= TACLR;
        break;
      case CL_TimerA2:      // timer A2
        TA2CTL |= TACLR;
        break;
      case CL_TimerB0:      // timer B0
        TB0CTL |= TBCLR;
        break;
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
      // read current value in counter
      return TAR;
    }
#elif defined(__MSP430F5529)
    switch(timer)
    {
      // read current value in counter
      case CL_TimerA0:      // timer A0
      default:
        return TA0R;
        break;
      case CL_TimerA1:      // timer A1
        return TA1R;
        break;
      case CL_TimerA2:      // timer A2
        return TA2R;
        break;
      case CL_TimerB0:      // timer B0
        return TB0R;
        break;
    }
#else
  #error This microcontroller is not supported by CounterLib.
#endif
  }

  uint16_t readAndReset(void)
  {
    uint16_t counter_value = 0;
#if defined(__MSP430G2553)
    if(timer == CL_TimerA0)
    {      
      counter_value = TAR;    // store counter value
      TACTL |= TACLR;         // reset counter
    }
#elif defined(__MSP430F5529)
    switch(timer)
    {
      case CL_TimerA0:        // timer A0
      default:
        counter_value = TA0R; // store counter value
        TA0CTL |= TACLR;      // reset counter
        break;
      case CL_TimerA1:        // timer A1
        counter_value = TA1R; // store counter value
        TA1CTL |= TACLR;      // reset counter
        break;
      case CL_TimerA2:        // timer A2
        counter_value = TA2R; // store counter value
        TA2CTL |= TACLR;      // reset counter
        break;
      case CL_TimerB0:        // timer B0
        counter_value = TB0R; // store counter value
        TB0CTL |= TBCLR;      // reset counter
        break;
    }
#else
  #error This microcontroller is not supported by CounterLib.
#endif
      return counter_value;   // return counter value
  }

private:

};  // end of struct/template Counter

#endif  // CounterLib_T_h

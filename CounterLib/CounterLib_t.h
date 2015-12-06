/*
CounterLib_t.h - Simple counter library for Energia with MSP430 microcontrollers.
Created by Adrian Studer, August 2015.

Distributed under MIT License, see license.txt for details.

Modified by Frank Milburn, September 2015 to include the MSP430FR5969 and bug fixes
Modified by Frank Milburn, December 2015 to include the MSP430FR6989
*/

#ifndef CounterLib_T_h
#define CounterLib_T_h

#include <inttypes.h>
#include <Energia.h>

// definition of timer and its pin for MSP430G2553 and MSP430G2452
#if defined(__MSP430G2553__) || defined(__MSP430G2452__)

  enum CL_TIMER_t
  {
    CL_TimerA0     // G2553, G2452 P1.0
  };
  
  // clk pin initalization for each supported timer
  #define CL_TACLK_PIN_SETUP  { P1DIR &= ~BIT0; P1SEL |= BIT0; P1SEL2 &= ~BIT0; }

// definition of timer and its pin for MSP430G2231
#elif defined(__MSP430G2231__)

  enum CL_TIMER_t
  {
    CL_TimerA0     // G2231 P1.0
  };
  
  // clk pin setup for each supported timer
  #define CL_TACLK_PIN_SETUP  { P1DIR &= ~BIT0; P1SEL |= BIT0; }

// definition of timers and their pins for MSP430F5529
#elif defined(__MSP430F5529__)

  enum CL_TIMER_t
  {
    CL_TimerA0,    // F5529 P1.0
    CL_TimerA1,    // F5529 P1.6
    CL_TimerA2,    // FF529 P2.2
    CL_TimerB0     // F5529 P7.7 (note: pin not easily accessible on LP, supports port mapping as PM_TB0CLK)
  };
  
  // clk pin setup for each supported timer
  #define CL_TA0CLK_PIN_SETUP  { P1DIR &= ~BIT0; P1SEL |= BIT0; }
  #define CL_TA1CLK_PIN_SETUP  { P1DIR &= ~BIT6; P1SEL |= BIT6; }
  #define CL_TA2CLK_PIN_SETUP  { P2DIR &= ~BIT2; P2SEL |= BIT2; }
  #define CL_TB0CLK_PIN_SETUP  { P7DIR &= ~BIT7; P7SEL |= BIT7; }

// definition of timers and their pins for MSP430FR5969 and MSP430FR6989
#elif defined(__MSP430FR5969__) || defined(__MSP430FR6989__)

  enum CL_TIMER_t
  {
    CL_TimerA0,    // FR5969 P1.2                 FR6989 P1.2*, P6.7*, P7.0* 
    CL_TimerA1,    // FR5969 P1.1*                FR6989 P1.1*, P4.4*, P5.2*
    CL_TimerB0     // FR5969 P2.0*                FR6989 P2.0, P3.3*, P5.7*
    // Note: Pins with * are not easily accessible on LaunchPads
  };
  
  // clk pin setup for each supported timer
  #define CL_TA0CLK_PIN_SETUP  { P1DIR &= ~BIT2; P1SEL0 &= ~BIT2; P1SEL1 |= BIT2; }
  #define CL_TA1CLK_PIN_SETUP  { P1DIR &= ~BIT1; P1SEL0 &= ~BIT1; P1SEL1 |= BIT1; }
  #define CL_TB0CLK_PIN_SETUP  { P2DIR &= ~BIT0; P2SEL0 |= BIT0; P2SEL1 |= BIT0; }

#else
  #error 1) This microcontroller is not supported by CounterLib
#endif

enum CL_DIVIDER_t
{
  CL_Div1 = ID_0,     // divide by 1
  CL_Div2 = ID_1,     // divide by 2
  CL_Div4 = ID_2,     // divide by 4
  CL_Div8 = ID_3      // divide by 8 
};

#if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_TA3__)
  // define anything that's special about TA2 and TA3
#elif defined(__MSP430_HAS_T0A3__) || defined(__MSP430_HAS_T0A5__)
  #define CL_DIVIDER_EX  // these timers support a 2nd divider
#else
  #error 2) This microcontroller's timer peripheral is not supported by CounterLib
  // to support this microcontroller, you'll need to add timer specific code below
#endif

template <CL_TIMER_t timer = CL_TimerA0>
struct Counter {

public:

#if defined(CL_DIVIDER_EX)
  void start(CL_DIVIDER_t divider1 = CL_Div1, uint8_t divider2 = 1)
  {
    uint16_t divider1_bits = divider1;               // basic divider can be 1, 2, 4 or 8
    uint16_t divider2_bits = (divider2 - 1) & 0x07;  // extended divider can be 1 to 8
#else
  void start(CL_DIVIDER_t divider1 = CL_Div1)
  {
    uint16_t divider1_bits = divider1;    // basic divider can be 1, 2, 4 or 8
#endif

  #if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_TA3__)
    if(timer == CL_TimerA0)
    {
      // set pin as input, select function TA0CLK
      CL_TACLK_PIN_SETUP;

      // configure timer A0 to count upwards based on TA0CLK input
      // halt timer, disable interrupts, TACLK clock source, reset counter
      TACTL = TACLR | divider1_bits;  // clears MCx, TAIE, TASSEL
 
      // start timer in continuous mode
      TACTL |= MC_2;
    }
  #else
    switch(timer)
    {
      // configure timer to count upwards using timer CLK input from pin
      // 1. setup pin
      // 2. halt timer, disable interrupts, TA0CLK clock source, reset counter
      // 3. start timer in continuous mode
      default:
    #if defined(__MSP430_HAS_T0A3__) || defined(__MSP430_HAS_T0A5__)
      case CL_TimerA0:    // timer A0
        CL_TA0CLK_PIN_SETUP;      // set pin as input, select function TA0CLK
        TA0CTL = divider1_bits;   // set main divider, clears MCx, TAIE, TASSEL
        TA0EX0 = (TA0EX0 & 0xfff8) | divider2_bits;  // set extended divider
        TA0CTL |= TACLR | MC_2;   // reset counter and start timer in continuous mode
        break;
    #endif
    #if defined(__MSP430_HAS_T1A3__)
      case CL_TimerA1:    // timer A1
        CL_TA1CLK_PIN_SETUP;      // set pin as input, select function TA1CLK
        TA1CTL = divider1_bits;   // set main divider, clears MCx, TAIE, TASSEL
        TA1EX0 = (TA1EX0 & 0xfff8) | divider2_bits;  // set extended divider
        TA1CTL |= TACLR | MC_2;   // reset counter and start timer in continuous mode
        break;
    #endif
    #if defined(__MSP430_HAS_T2A3__)
      case CL_TimerA2:    // timer A2
        CL_TA2CLK_PIN_SETUP;      // set pin as input, select function TA2CLK
        TA2CTL = divider1_bits;   // set main divider, clears MCx, TAIE, TASSEL
        TA2EX0 = (TA2EX0 & 0xfff8) | divider2_bits;  // set extended divider
        TA2CTL |= TACLR | MC_2;   // reset counter and start timer in continuous mode
        break;
    #endif
    #if defined(__MSP430_HAS_T0B7__)
      case CL_TimerB0:    // timer B0
        CL_TB0CLK_PIN_SETUP;      // set pin as input, select function TB0CLK
        TB0CTL = divider1_bits;   // set main divider, clears MCx, TAIE, TASSEL
        TB0EX0 = (TB0EX0 & 0xfff8) | divider2_bits;  // set extended divider
        TB0CTL |= TBCLR | MC_2;   // reset counter and start timer in continuous mode
        break;
    #endif
    }
  #endif 
  }

  void stop(void)
  {
  #if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_TA3__)
    if(timer == CL_TimerA0)
    {
      // halt timer, but don't reset counter
      TACTL &= ~(MC_3);  // clears MCx
    }
  #else
    switch(timer)
    {
      // halt timer, but don't reset counter
      default:
    #if defined(__MSP430_HAS_T0A3__) || defined(__MSP430_HAS_T0A5__)
      case CL_TimerA0:      // timer A0
        TA0CTL &= ~(MC_3);
        break;
    #endif
    #if defined(__MSP430_HAS_T1A3__)
      case CL_TimerA1:      // timer A1
        TA1CTL &= ~(MC_3);
        break;
    #endif
    #if defined(__MSP430_HAS_T2A3__)
      case CL_TimerA2:      // timer A2
        TA2CTL &= ~(MC_3);
        break;
    #endif
    #if defined(__MSP430_HAS_T0B7__)
      case CL_TimerB0:      // timer B0
        TB0CTL &= ~(MC_3);
        break;
    #endif
    }
  #endif
  }

  void reset(void)
  {
  #if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_TA3__)
    if(timer == CL_TimerA0)
    {
      // reset counter to zero, counter keeps running
      TACTL |= TACLR;
    }
  #else
    switch(timer)
    {
      // reset counter to zero, counter keeps running
      default:
    #if defined(__MSP430_HAS_T0A3__) || defined(__MSP430_HAS_T0A5__)
      case CL_TimerA0:      // timer A0
        TA0CTL |= TACLR;
        break;
    #endif
    #if defined(__MSP430_HAS_T1A3__)
      case CL_TimerA1:      // timer A1
        TA1CTL |= TACLR;
        break;
    #endif
    #if defined(__MSP430_HAS_T2A3__)
      case CL_TimerA2:      // timer A2
        TA2CTL |= TACLR;
        break;
    #endif
    #if defined(__MSP430_HAS_T0B7__)
      case CL_TimerB0:      // timer B0
        TB0CTL |= TBCLR;
        break;
    #endif
    }
  #endif
  }

  uint16_t read(void)
  {
  #if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_TA3__)
    if(timer == CL_TimerA0)
    {
      // read current value in counter
      return TAR;
    }
  #else
    switch(timer)
    {
      // read current value in counter
      default:
    #if defined(__MSP430_HAS_T0A3__) || defined(__MSP430_HAS_T0A5__)
      case CL_TimerA0:      // timer A0
        return TA0R;
        break;
    #endif
    #if defined(__MSP430_HAS_T1A3__)
      case CL_TimerA1:      // timer A1
        return TA1R;
        break;
    #endif
    #if defined(__MSP430_HAS_T2A3__)
      case CL_TimerA2:      // timer A2
        return TA2R;
        break;
    #endif
    #if defined(__MSP430_HAS_T0B7__)
      case CL_TimerB0:      // timer B0
        return TB0R;
        break;
    #endif
    }
  #endif
  }

  uint16_t readAndReset(void)
  {
    uint16_t counter_value = 0;
    
  #if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_TA3__)
    if(timer == CL_TimerA0)
    {      
      counter_value = TAR;    // store counter value
      TACTL |= TACLR;         // reset counter
    }
  #else
    switch(timer)
    {
      default:
    #if defined(__MSP430_HAS_T0A3__) || defined(__MSP430_HAS_T0A5__)
      case CL_TimerA0:        // timer A0
        counter_value = TA0R; // store counter value
        TA0CTL |= TACLR;      // reset counter
        break;
    #endif
    #if defined(__MSP430_HAS_T1A3__)
      case CL_TimerA1:        // timer A1
        counter_value = TA1R; // store counter value
        TA1CTL |= TACLR;      // reset counter
        break;    
    #endif
    #if defined(__MSP430_HAS_T2A3__)
      case CL_TimerA2:        // timer A2
        counter_value = TA2R; // store counter value
        TA2CTL |= TACLR;      // reset counter
        break;
    #endif
    #if defined(__MSP430_HAS_T0B7__)
      case CL_TimerB0:        // timer B0
        counter_value = TB0R; // store counter value
        TB0CTL |= TBCLR;      // reset counter
        break;
    #endif
    }
  #endif
  
      return counter_value;   // return counter value
  }

private:

};  // end of struct/template Counter

#endif  // CounterLib_T_h

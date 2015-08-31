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
  CL_TA0,    // G2533 P1.0, F5529 P1.0
  CL_TA1,    // F5529 P1.6
  CL_TA2,    // FF529 P2.2
  CL_TB0     // F5529 P7.7
};

template <CL_TIMER_t timer = CL_TA0>
struct Counter {

public:
  void start(void)
  {
    
  };

  void stop(void);
  void reset(void);
  uint16_t read(void);
  uint16_t readAndReset(void);

private:

};  // end of struct/template Counter

#endif  // CounterLib_T_h

#include <CounterLib_t.h>

Counter<> MyCounter;     // create counter that counts pulses on pin P1.0

void setup()
{
  Serial.begin(9600);
  MyCounter.start(CL_Div8);    // start counter, divide clock by 8
}

void loop()
{
  MyCounter.reset();           // reset counter to zero
  delay(100);                  // wait 100 milliseconds
  Serial.print((MyCounter.read() + 5) / 100 * 8);   // read counter, calculate kHz
  Serial.println(" kHz");
  delay(1000);                 // wait one second
}

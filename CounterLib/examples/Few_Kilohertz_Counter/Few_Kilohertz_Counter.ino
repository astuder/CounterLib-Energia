#include <CounterLib_t.h>

Counter<> MyCounter;     // create counter that counts pulses on pin P1.0

void setup()
{
  Serial.begin(9600);
  MyCounter.start();     // start counter
}

void loop()
{
  MyCounter.reset();           // reset counter to zero
  delay(100);                  // wait 100 milliseconds
  Serial.print(MyCounter.read() * 10);   // read counter, calculate Hz
  Serial.println(" Hz");
  delay(1000);                 // wait one second
}

#include <CounterLib_t.h>

Counter<> MyCounter;

void setup()
{
  Serial.begin(9600);
  MyCounter.start();
}

void loop()
{
  MyCounter.reset();
  delay(10);
  Serial.print((MyCounter.read() + 5) / 10);
  Serial.println(" kHz");
  delay(1000);
}

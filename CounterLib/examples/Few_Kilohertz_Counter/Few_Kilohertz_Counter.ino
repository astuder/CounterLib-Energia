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
  delay(100);
  Serial.print(MyCounter.read() * 10);
  Serial.println(" Hz");
  delay(1000);
}

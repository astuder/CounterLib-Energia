# CounterLib-Energia
A library for Energia that counts signal pulses received from an external source, like for example a clock.

This library currently only supports MSP430G2533 and MSP430F5529.

CounterLib leverages the Timer_A peripheral and uses pin P1.0 as external clock input for a 16 bit counter. 
Using a Timer instead of interrupts allows to count very fast pulses.

## Installation

To install this libray in Energia, copy the folder "CounterLib" into "<user home>/Documents/Energia/libraries" and restart Energia.

## Usage

To create an instance of the counter, simply declare it as a global variable like this:

	Counter<> MyCounter

Once created, the counter has 5 functions:
* start() initializes the timer peripheral and I/O pin and starts the counter
* stop() stops the counter, but does not reset the counter value
* read() reads the current value of the counter
* reset() resets the counter to 0, the counter keeps running
* readAndReset() reads the current value and resets the counter to 0

Here is a simple example that resets the counter, waits one second, reads and displays the value of the counter,
and waits another second before restarting the loop.

    Counter<> MyCounter;
    
    void setup()
    {
      Serial.begin(9600);
      MyCounter.start();
    }
    
    void loop()
    {
      MyCounter.reset();
      delay(1000);
      Serial.println(MyCounter.read());
	  delay(1000);
    }

Connected to the 1KHz calibration signal output of my oscilloscope (Rigol DS1054Z) I get the following output:

    1005
	1005
	1006
	1005

The difference to the theoretical number of 1000 can have different causes. The most likely reasons are 
* The clock of the LaunchPad and the oscilloscope differ by 0.5%
* There is some small overhead in each function call

Because the counter is only 16 bits, it will restart at 0 after 65536 pulse. To count faster signals the measurement
period needs to be adjusted downwards. E.g. when measuring a 1 MHz clock, the counter will reach 10000 in 10ms.

To measure signals in that range, you could adjust the example as follows:

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

With the shorter measurement period for fast signals, the inaccuracy due to overhead becomes more significant. 
For example, with a signal at 740 KHz my test setup reports 795 KHz. Because this overhead is fairly constant,
we can compensate that error by measuring a known signal and then add/subtract the measured difference, e.g.
by changing the line above to:

    Serial.print(((MyCounter.read() + 5) / 10) - 55);

## Caveats
* P1.0 is also used for LED1, you may need to remove the LED1 jumper if the signal source is not strong enough to drive the LED
* On the MSP430F5529LP, you have to use the upper pin of the LED1 jumper to input the signal
* Make sure to verify, that the signal source does not exceed 3.3V or you may damage the MSP430
* This library may not play nicely with other libraries or functionality that uses Timer A (PWM / analogWrite?)

## Todo
* Configurable /2, /4 and /8 clock dividers, currently the counter is increased on each clock
* Support for other MCUs and LaunchPads

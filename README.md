# CounterLib-Energia
A library for Energia that counts signal pulses received from an external source, like for example a clock.

This library currently only supports MSP430G2533, MSP430F5529 and MSP430FR5969

CounterLib leverages timer peripherals and uses I/O as external clock input for a 16 bit counter. 
Using a Timer instead of interrupts allows to count very fast pulses.

## Installation

To install this libray in Energia, copy the folder "CounterLib" into "<user home>/Documents/Energia/libraries"
and restart Energia.

## Usage

To create an instance of the counter, simply declare it as a global variable like this:

	Counter<> MyCounter;	// create a counter that counts pulses on pin P1.0

Once created, the counter has 5 functions:
* start() initializes the timer peripheral and I/O pin and starts the counter
* stop() stops the counter, but does not reset the counter value
* read() reads the current value of the counter
* reset() resets the counter to 0, the counter keeps running
* readAndReset() reads the current value and resets the counter to 0

### Timers and Pins

By default, the counter will use pin P1.0 as input. Some MCUs have multiple timer peripherals linked to different
input pins. The timer peripheral, and therefore indirectly the input pin, can be selected with an optional
parameter when declaring the counter. For example:

	Counter<CL_TimerA1> MyCounter; // use timer A1, which uses pin P1.6 as input
	
Below a list of supported timers and their pins. Note that not all MCUs support all timers.

| Timer      	| G2533 	| F5529 	| FR5969 	|
|------------	|-------	|-------	|--------	|
| CL_TimerA0 	|  P1.0 	|  P1.0  	|  P1.2  	|
| CL_TimerA1 	|  n/a  	|  P1.6  	|  P1.1  	|
| CL_TimerA2 	|  n/a  	|  P2.2  	|  n/a   	|
| CL_TimerB0 	|  n/a  	|  P7.7 	|  P2.0  	|

### Dividers

For accurate counting you want a long measurement period, for example 100ms. But fast signals will overflow the
16 bit counter. For example a 1 MHz signal will count to 65535 (the maximum for 16 bit) in just 65 ms. This is when
clock dividers come in handy.

Depending on the MCU, the timers have one (G2533) or two (F5529, FR5969) dividers. The dividers can be set as optional
parameter of the start() function.

	MyCounter.start(divider1, divider2);
	
divider1 can have the fixed values of CL_Div1 (divide by 1, the default), CL_Div2 (divide by 2), CL_Div4 (divide by 4)
or CL_Div8 (divide by 8). For example, to increase the counter on every 4th tick of a clock, start the counter like this:

	MyCounter.start(CL_Div4);	// only count every 4th pulse
	
divider2 can be added if you need to slow things down even more. Valid values for divider2 are between 1 and 8, with 1
the default if divider2 is not specified. Note that the dividers are applied after each other, so the total divisor will
be divider1 * divider2. For example, the following counter will only count every 10th pulse:

	MyCounter.start(CL_Div2, 5);	// divider set to 2*5=10

##Examples

Here is a simple example that resets the counter, waits one second, reads and displays the value of the counter,
and waits another second before restarting the loop.

    Counter<> MyCounter;	// create counter that counts pulses on pin P1.0
    
    void setup()
    {
      Serial.begin(9600);
      MyCounter.start();	// start counter
    }
    
    void loop()
    {
      MyCounter.reset();				// reset counter to zero
      delay(1000);						// wait one second
      Serial.println(MyCounter.read());	// read number of pulses during the last second
	  delay(1000);						// wait another second
    }

Connected to the 1KHz calibration signal output of my oscilloscope (Rigol DS1054Z) I get the following output:

    1005
	1005
	1006
	1005

The difference to the theoretical number of 1000 can have different causes. The most likely reasons are 
* The clock of the LaunchPad and the oscilloscope differ by 0.5%
* There is some small overhead in each function call

Because the counter is only 16 bits, it will restart at 0 after 65536 pulses. To count faster signals the measurement
period needs to be adjusted downwards. E.g. when measuring a 1 MHz clock, the counter will reach 10000 in 10ms.

To measure signals in that range, you could adjust the example as follows:

    Counter<> MyCounter;	// create counter that counts pulses on pin P1.0
    
    void setup()
    {
      Serial.begin(9600);
      MyCounter.start();	// start counter
    }
    
    void loop()
    {
      MyCounter.reset();				// reset counter to zero
      delay(10);						// wait 10 milliseconds
      Serial.print((MyCounter.read() + 5) / 10);	// read counter, calculate KHz
	  Serial.println(" kHz");
	  delay(1000);						// wait one second
    }

With a signal that reads 746 kHz on the oscilloscope, the sketch above reports 800 kHz. That's more than 7.5% off!
The reason for the inaccuracy is the overhead of Energia and the sketch itself. The overhead stays always the same, 
no matter how long the measurement period, so with shorter periods it becomes more significant.

This is where dividers are useful. By measuring a for a longer period, the effect of the overhead is reduced. When
dividing 1 MHz by 8, the counter will only reach 12500 in 100ms, a value that fits well within the 16 bit counter.

Here I start the timer with the divider set to 8, extended the measurement period to 100 ms, and adjusted the math
accordingly to get back to kHz.

    Counter<> MyCounter;	// create counter that counts pulses on pin P1.0
    
    void setup()
    {
      Serial.begin(9600);
      MyCounter.start(CL_Div8);		// start counter, divide clock by 8
    }

    void loop()
    {
      MyCounter.reset();			// reset counter to zero
      delay(100);					// wait 100 milliseconds
      Serial.print((MyCounter.read() + 5) / 100 * 8);	// read counter, calculate kHz
	  Serial.println(" kHz");
	  delay(1000);					// wait one second
    }

	MyCounter.reset();
	delay(100);
	Serial.print((MyCounter.read() + 5) / 100 * 8);

With these minor changes above, my test setup now reports 744 kHz, which is only 0.2% different from what the 
oscilloscope measured.

## Caveats
* P1.0 is also used for LED1, you may need to remove the LED1 jumper if the signal source is not strong enough to drive the LED
* On the MSP430F5529LP, you have to use the upper pin of the LED1 jumper to input the signal
* Make sure to verify, that the signal source does not exceed 3.3V or you may damage the MSP430
* This library may not play nicely with other libraries or functionality that uses Timer A (PWM / analogWrite?)

## Todo
* Support for other MCUs and LaunchPads


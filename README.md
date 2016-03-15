SYMAX Receiver
==========

Thanks to

 * execuc for the original code (https://github.com/execuc/v202-receiver)
 * Deviationtx team for hacking the symax protocol (https://bitbucket.org/deviationtx/deviation/src/1194044d116b9611a015837226729e26de7e8365/src/protocol/symax_nrf24l01.c)
 * Special thanks to hexfet from deviationtx forum for the invaluable support
 
Goal
--------------------

This code decodes frames from the X5C-1, X11, X11C, X12... transmitter (blue or green led) with an arduino and a nrf24L01+ chip. NOT the old x5c!
This code has not been tested enough and it is not super reliable. So don't use it with dangerous rc model as planes, helicopters, cars...

[![DEMO VIDEO](http://img.youtube.com/vi/zw69m9TvEHo/0.jpg)](http://www.youtube.com/watch?v=zw69m9TvEHo)

Hardware
--------------------

Arduino, nrf24l01+ chip
Connect SCK, MISO, MOSI on pins D13, D12 and D11. Then connect CE and CS on digital pins you have chosen in the code (see the `wireless.setPins()` method below).
Finally connect nrf24l01 VCC and GND to arduino 3.3V (if you plan to NOT use the chip in low power mode, get a proper 3.3v power supply or you may destroy the Arduino) and GND pins.

Use
--------------------

There are two classes : 

 * nrf24l01p : handle the spi protocol to communicate with the nrf24l01p chip
 * symax_protocol : handle the symax protocol
	
symax_rx.ino is an example showing how use theses classes.

These two classes are instantiated :
    
	nrf24l01p wireless; 
    symaxProtocol protocol;

The nrf24l01 pins must be defined in arduino setup function. `setPins` method arguments define the nrf24L01 CE (chip enable) and CS (SPI chip select) pins in this order. In my example, I did not use SS arduino pin as CS but SS pin must be set to output to activate the SPI mode to master.

    void setup() {
	  ...
	  // SS pin must be set as output to set SPI to master !
	  pinMode(SS, OUTPUT);

	  // Set CE pin to 10 and CS pin to 9
	  wireless.setPins(10,9);
	  
	  // Set power (PWRLOW,PWRMEDIUM,PWRHIGH,PWRMAX)
	  wireless.setPwr(PWRLOW);
	  
	  protocol.init(&wireless);
      ...
    }
	
SPI wrapper (nrf24l01p class) are linked to the protocol in the setup function

    protocol.init(&wireless);

In the loop function, symaxProtocol Run() method must be called AT MOST every 4ms with & `rx_values_t` structure:

    uint8_t value = protocol.run(&rxValues);
	
This function has several kind of returns from an enum :

    enum rxReturn
    {
       BOUND_NEW_VALUES = 0,   // Bound state, frame received with new TX values
       BOUND_NO_VALUES,        // Bound state, no new frame received
       NOT_BOUND,              // Not bound, initial state
       BIND_IN_PROGRESS,       // Bind in progress, first frame has been received with TX id, wait no bind frame.
       UNKNOWN                 // Not used for moment
    };
	
When a frame is received (`BOUND_NEW_VALUES`), `rx_values_t` structure can be read :

    typedef struct __attribute__((__packed__)) {
	  uint8_t throttle; // 0...255
	  int8_t yaw; // 127...-127
	  int8_t pitch; // 127...-127
	  int8_t roll; // 127...-127
	  int8_t trim_yaw; // 31...-31
	  int8_t trim_pitch; // 31...-31
	  int8_t trim_roll; // 31...-31
	  bool video; // true means button is down
	  bool picture; // true means button is down
	  bool highspeed; // true means high speed mode selected
	  bool flip; // true means button is down
    } rx_values_t;

Four axis, 3 trims values and buttons are available.

Improvements 
--------------------
	
 Some improvements : 
  * Handle missing frame : for each frequency, transmitter send two frames in 8 ms. If there are lost, received will wait 4 frequency switching * 8ms to get frame. It would be interesting to jump some frequencies to retrieve the channel switching faster.

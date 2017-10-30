/* symax_rx.ino -- An arduino sketch to test the protocol symax
 *
 */

#include <SPI.h>
#include "symax_protocol.h"
#include <Servo.h>

#define LED_PIN 2
#define LED_ON digitalWrite(LED_PIN, HIGH)
#define LED_OFF digitalWrite(LED_PIN, LOW)

Servo myservo_gaz, myservo_rul;  // create servo object to control a servo

nrf24l01p wireless; 
symaxProtocol protocol;

unsigned long time = 0;

void setup() {

	pinMode(LED_PIN, OUTPUT);
	LED_ON;

  Serial.begin(115200);

  myservo_gaz.attach(6);  
  myservo_rul.attach(5);  

  // SS pin must be set as output to set SPI to master !
  pinMode(SS, OUTPUT);

  // Set CE pin to 10 and CS pin to 9
  wireless.setPins(8,7);
  
  // Set power (PWRLOW,PWRMEDIUM,PWRHIGH,PWRMAX)
  wireless.setPwr(PWRLOW);
  
  protocol.init(&wireless);

  time = micros();
  Serial.println("Start");
  
  LED_OFF;
  
}

rx_values_t rxValues;

unsigned long newTime;

bool led;

void loop() 
{	

  time = micros();
  uint8_t value = protocol.run(&rxValues); 
  newTime = micros();
   
  switch( value )
  {
    case  NOT_BOUND:
        Serial.println("Not bound");
		    led = (millis() % 1000 > 500) ? HIGH : LOW;
        break;

    case  BIND_IN_PROGRESS:
      led = (millis() % 400 > 200) ? HIGH : LOW;
      Serial.println("Bind in progress");
      break;
    
    case BOUND_NEW_VALUES:
		  led = HIGH;
      Serial.print(newTime - time);
      Serial.print(" :\t");Serial.print(rxValues.throttle);
      Serial.print("\t"); Serial.print(rxValues.yaw);
      Serial.print("\t"); Serial.print(rxValues.pitch);
      Serial.print("\t"); Serial.print(rxValues.roll);
      Serial.print("\t"); Serial.print(rxValues.trim_yaw);
      Serial.print("\t"); Serial.print(rxValues.trim_pitch);
      Serial.print("\t"); Serial.print(rxValues.trim_roll);
      Serial.print("\t"); Serial.print(rxValues.video);
      Serial.print("\t"); Serial.print(rxValues.picture);
      Serial.print("\t"); Serial.print(rxValues.highspeed);
      Serial.print("\t"); Serial.println(rxValues.flip);
      //time = newTime;

      myservo_rul.write(map(rxValues.roll, -127, 127, 0, 180));              // tell servo to go to position
	    myservo_gaz.write(map(rxValues.pitch, -127, 127, 0, 180));              // tell servo to go to position
      
      break;

    case BOUND_NO_VALUES:
      //Serial.print(newTime - time); Serial.println(" : ----");
      led = LOW;
      break;

    default:
      break;

  }
	if (led) LED_ON; else LED_OFF;

}

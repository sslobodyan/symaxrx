/* symax_rx.ino -- An arduino sketch to test the protocol symax
 *
 */

#include <SPI.h>
#include <symax_protocol.h>
#include <Servo.h>

Servo myservo_gaz, myservo_rul;  // create servo object to control a servo

nrf24l01p wireless; 
symaxProtocol protocol;

unsigned long time = 0;

void setup() {

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
  
}

rx_values_t rxValues;

unsigned long newTime;

void loop() 
{
  time = micros();
  uint8_t value = protocol.run(&rxValues); 
  newTime = micros();
   
  switch( value )
  {
    case  NOT_BOUND:
        Serial.println("Not bound");
    break;

    case  BIND_IN_PROGRESS:
        Serial.println("Bind in progress");
    break;
    
    case BOUND_NEW_VALUES:
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
    break;

    default:
    break;

  }

}

/* Encoder Library - Basic Example
   http://www.pjrc.com/teensy/td_libs_Encoder.html

   This example code is in the public domain.
*/

//#define USEDELTAP
#define USEDELTAT
#define TDIV 40 //how many time slots to pass before speed measure
#define MINPWM 40

#include <Encoder360.h>

#define POTPIN A5
#define MOTPIN 5

Encoder360 myEnc(2, 3);

void setup() {
  Serial.begin(9600);
  Serial.println("Motor w Encoder Control");
  digitalWrite(MOTPIN, LOW);
  pinMode(MOTPIN, OUTPUT);
}

long oldPosition  = -999;
long posDiff = 0;

float slPotPos = 0;
float we = 0.3;
int motDir = 0;
uint8_t oldpwm = 0;
long lmill = millis(); //last millis for full turn
long plmill = millis(); // last millis for partial turn
float dmill = 0;
float spd = 1;
float ws = 0.4;

long pofs = 5;
float ffac = 60000.0f;
float tfac = ffac * pofs / 360;



void loop() {
  long mill = millis();
  float curPotPos = analogRead(POTPIN);
  slPotPos = slPotPos * (1 - we) + curPotPos * we;
  int speedDial = slPotPos;



  // Serial.print("POT ");
  // Serial.println(speedDial);
  uint8_t pwm = map(speedDial, 0, 1023, MINPWM, 255);
  if (oldpwm != pwm) {
    oldpwm = pwm;
    if (pwm > MINPWM + 1) {
      if (pwm > 252) {
        pwm = 255;
      }
      analogWrite(MOTPIN, pwm);
      //Serial.print("MOTSPD "); Serial.println(pwm);
      if (motDir != 1) {
        motDir = 1;
      }
    } else {
      analogWrite(MOTPIN, 0);
      motDir = 0;
      //Serial.println("MOTOFF");
    }

  }
  /// Position tracking


#ifdef USEDELTAP
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    if (newPosition % pofs == 4) {
      float pdmill = mill - plmill;
      float newspd2 = tfac / pdmill;
      spd = spd * (1 - ws) + newspd2 * ws;
      plmill = mill;
      Serial.print(pwm);
      Serial.print(",");
      Serial.println(spd);
    }
    oldPosition = newPosition;
  }
#endif
#ifdef USEDELTAT
  if (mill % TDIV == 0) {
    long newPosition = myEnc.read();
    dmill = mill - lmill;
    if (newPosition != oldPosition) {
      float pdiff = newPosition - oldPosition;
      if (newPosition < oldPosition) {
        //pdiff = oldPosition - ( newPosition + 360 );
        pdiff += 360;
      }
      oldPosition = newPosition;
      Serial.print(dmill);
      Serial.print(",");
      float newSpd=pdiff*60/dmill;
      if(newSpd!=INFINITY){
        spd=spd*(1-ws)+newSpd*ws;      
      }
      Serial.println(spd);      
    }
    lmill = mill;
  }
#endif

}

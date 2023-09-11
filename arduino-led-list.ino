#include <Adafruit_NeoPixel.h>
#define PIN 2   // input pin Neopixel is attached to
#define NUMPIXELS 6 // number of neopixels in Ring

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

const int pingPin = 13;

int timer = 1000; // timing delay
int buttonState;
int buttonState2;

unsigned int counting;
unsigned int counting2;

byte b[NUMPIXELS];
byte rstlst[NUMPIXELS];

struct RGB {
  byte r = 0;
  byte g = 0;
  byte b = 0;
};

void setup()
{
  for (int i = 0; i < NUMPIXELS;i++) {
  	b[i] = i;
  }
  memcpy(rstlst, b, NUMPIXELS);
  
  
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  
  
  Serial.begin(9600);
  pixels.begin();
}

// bitshift for for enklere tall og jobbe med
byte distanceToByte(long microsecond) {
  return microsecond >> 10; // * 13
}

int blinkLed(int i) {
  if (i < NUMPIXELS) {
  	i++;
  }
}

void resetList() {
  memcpy(b, rstlst, NUMPIXELS);
}

void shutDownList() {
  for (int i = 0; i < NUMPIXELS; i++) {
  	b[i] = 0;
  }
}

void shiftLEDforward(){
  byte temp[sizeof(b)];
  memcpy(temp, b, sizeof(b));
  
  for (int i = 0; i < sizeof(b);i++) {
  	b[(i+1) % sizeof(b)] = temp[i];
  }
  
  pinMode(5, INPUT);
  
}

void loop() 
{
  static int i = 0;
  static int prevTime = 0;
  
  buttonState = digitalRead(5);
  buttonState2 = digitalRead(4);

  long duration;   
  byte bt;  
  if (millis() - prevTime > timer){
    prevTime = millis();


    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pingPin, LOW);
    pinMode(pingPin, INPUT);
    duration = pulseIn(pingPin, HIGH);
    bt = distanceToByte(duration);

  }
  // Serial.print(cm);
  // Serial.print("cm");
  //Serial.println();

  //delay(100);
  
  /*
    
  if (millis() - prevTime > timer){
    prevTime = millis();
    shiftLEDforward();
    if (b[0] == 3) {
    	resetList();
    }
  	i = bt * 13;
    //Serial.println(i);
    if (i > 250) {
   	  i = 0;
    }
  }
  
  pixels.setBrightness(i);
  pixels.setPixelColor(b[0], 10, 255, 10);
  pixels.setPixelColor(b[1], 0, 0, 0);
  pixels.setPixelColor(b[2], 0, 255, 255);
  pixels.setPixelColor(b[3], 255, 255, 0);
  pixels.setPixelColor(b[4], 255, 0, 0);
  pixels.setPixelColor(b[5], 255, 0, 255);

  pixels.show();
  //delay(timer);
  
  */
}


long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}



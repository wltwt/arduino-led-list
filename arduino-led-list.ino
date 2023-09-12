#include <Adafruit_NeoPixel.h>

#define PIN 2   // input pin Neopixel is attached to

#define NUMPIXELS 8 // number of neopixels in Ring

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

typedef struct
{
  byte r;
  byte g;
  byte b;
} RGB;

RGB rgbValues[NUMPIXELS];				// array som holder RGB-verdiene til hver LED
byte ledPosition[NUMPIXELS];			// array som holder posisjonene til hver LED


const byte modeBtnPin  = 6;				// mode/speed
const byte colorBtnPin = 5;				// color/brightness pin
const byte powerBtnPin = 4;				// av/på-knapp pin

unsigned long prevDebounceValue = 0; 	// the last time the output pin was toggled
unsigned long debounceDelay = 10;   



const int timer = 100;
const int powerCheckTimer = 500;

int buttonState;
int buttonState2;
int sensorValue;


int powerBtnToggle = HIGH;			     	// the current state of the output pin
int btnState;      	  			// the current reading from the input pin
int prevBtnState = LOW;



unsigned int counting;


byte rstlst[NUMPIXELS];


int sensorPin = A1;
int sensorMax = 0;
int sensorMin = 1023;


void initArrays(){
  for (int i = 0; i < NUMPIXELS;i++) {
  	ledPosition[i] = i;
  }
  
  for (int i = 0; i < NUMPIXELS; i++) {
  	rgbValues[i].r = 255;
    rgbValues[i].g = 255;
    rgbValues[i].b = 0;
  }
  
  memcpy(rstlst, ledPosition, NUMPIXELS);
}


// bitshift for for enklere tall og jobbe med
byte lightConstrain(long microsecond) {
  return microsecond >> 10; // * 13
}


int blinkLed(int i) {
  if (i < NUMPIXELS) {
  	i++;
  }
}

void resetList() {
  memcpy(ledPosition, rstlst, NUMPIXELS);
}

void shutDownList() {
  for (int i = 0; i < NUMPIXELS; i++) {
  	ledPosition[i] = 0;
  }
}

void shiftLEDforward(){
  byte temp[sizeof(ledPosition)];
  memcpy(temp, ledPosition, sizeof(ledPosition));
  
  for (int i = 0; i < sizeof(ledPosition);i++) {
  	ledPosition[(i+1) % sizeof(ledPosition)] = temp[i];
  }
}


bool lastBtnCheck(int readState) {
  return readState != prevBtnState;					// button press result in 1
}

void debounceControl(int btnPin, int &toggle) {		// trengte referanse til toggle for å funke
  int readState = digitalRead(btnPin);
  
  //Serial.println(lastBtnCheck(readState));
  
  if (lastBtnCheck(readState)) {
  	prevDebounceValue = millis();					// then this is true
  }
  
  if ((millis() - prevDebounceValue) > debounceDelay) {
  	
    /*
    Serial.print(millis() - prevDebounceValue);
    Serial.print(" and ");
    Serial.print(millis());
    Serial.println("\n");
    //delay(1000);
    */
    
    if (readState != btnState) {
        btnState = readState;
          
      if (btnState == HIGH) {
    	toggle = !toggle;
        //Serial.println(toggle);
      }
    
    }
        //Serial.println(btnState);
    	//delay(100);


  }
  prevBtnState = readState;
}



void powerCheck(int prevTime) {
  if (millis() - prevTime > powerCheckTimer) {
    prevTime = millis();
  }
}




void setup()
{
  initArrays();

  
  pinMode(sensorPin, INPUT);
  pinMode(4, INPUT);
  
  
  Serial.begin(9600);
  pixels.begin();
}




void loop() 
{
  static int i = 0;
  static int prevTime = 0;
  sensorValue = analogRead(sensorPin);

  
  debounceControl(powerBtnPin, powerBtnToggle);
  //btnState = digitalRead(powerBtnPin);
  
  
  Serial.println(powerBtnToggle);
  
  
  //Serial.println(btnState);

  

  
    
   
  //Serial.print(bt);
  //Serial.println(", ");
  //Serial.println("counted presses: ");
  //Serial.println(counting);  

  // Serial.print(cm);
  // Serial.print("cm");
  //Serial.println();

  //delay(100);
  
  
    
  if (millis() - prevTime > timer){
    prevTime = millis();
    shiftLEDforward();
    //if (b[0] == 3) {
    //	resetList();
    //}
  	//i = bt * 13;
    //Serial.println(i);
    if (i == 0) {
    	i = 100;
    } else {
    	i = 0;
    }
    if (i > 250) {
   	  i = 0;
    }
  }
  
  pixels.setBrightness(i);

  //pixels.setPixelColor(ledPosition[0], rgbValues[0].r, rgbValues[0].g, rgbValues[0].r);
  //pixels.setPixelColor(ledPosition[1], rgbValues[1].r, rgbValues[1].g, rgbValues[1].r);

  for (int i = 0; i < 2; i++) {
    pixels.setPixelColor(ledPosition[i], rgbValues[i].r, rgbValues[i].g, rgbValues[i].b);
  }
  
  
  
  
  pixels.show();
  
  /*
  
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
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}



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
byte rstlst[NUMPIXELS];

RGB rgbValuesTemp[NUMPIXELS];


const byte modeBtnPin  = 6;				// mode/speed
const byte colorBtnPin = 5;				// color/brightness pin
const byte powerBtnPin = 4;				// av/på-knapp pin

unsigned long prevDebounceValue = 0; 		// the last time the output pin was toggled
unsigned long debounceDelay = 10;
unsigned long powerButtonHeld = 0;

const int timer = 100;						
const int powerCheckTimer = 500;			// burde vært en liste

int buttonState;
int buttonState2;
int sensorValue;
int buttonPress;


int powerBtnToggle = LOW;			     	// the current state of the output pin
int btnState;      	  						// the current reading from the input pin
int prevBtnState = LOW;						

// Burde laget struct for mye av disse verdiene men ikke nok tid.


unsigned int counting;




int sensorPin = A1;
int sensorMax = 0;
int sensorMin = 1023;


void initArrays(){
  for (int i = 0; i < NUMPIXELS;i++) {
  	ledPosition[i] = i;
  }
  
  for (int i = 0; i < NUMPIXELS; i++) {
  	rgbValues[i].r = 0;
    rgbValues[i].g = 0;
    rgbValues[i].b = 0;
  }
  
  memcpy(rgbValuesTemp, rgbValues, NUMPIXELS);
  
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

void setLedColor(int pos, int red, int green, int blue) {
  rgbValues[pos].r = red;
  rgbValues[pos].g = green;
  rgbValues[pos].b = blue;
}

void bootUpSequence() {
  for (int i = 0; i < NUMPIXELS; i++) {
  	setLedColor(i, 0, 0, 255);
  }
  int delTime = 250;
  while (millis() < 1000) {
    startList(255);    
    delay(delTime);
    startList(0);  
    delay(delTime);
  }      
}





void resetList() {
  memcpy(ledPosition, rstlst, NUMPIXELS);
}


void shiftLEDforward(){
  byte temp[sizeof(ledPosition)];
  memcpy(temp, ledPosition, sizeof(ledPosition));
  
  for (int i = 0; i < sizeof(ledPosition);i++) {
  	ledPosition[(i+1) % sizeof(ledPosition)] = temp[i];
  }
}

bool lastBtnCheck(int readState) {
  return readState != prevBtnState;					
}

void debounceControl(int btnPin, int &toggle) {		// trengte referanse til toggle for å funke
  int readState = digitalRead(btnPin);
  
  //Serial.println(lastBtnCheck(readState));
  if (lastBtnCheck(readState)) {
    //Serial.println(millis());
    //Serial.println(powerButtonHeld);
    if (prevDebounceValue < powerButtonHeld) {
      powerButtonHeld = millis() - powerButtonHeld;
      Serial.print("BUTTON HELD FOR: ");
      Serial.print(powerButtonHeld);
      Serial.println(" MILLISECONDS");
        buttonPress++;
   		Serial.println(buttonPress);
    }
    
    
    
  	prevDebounceValue = millis();


  }
  
  if ((millis() - prevDebounceValue) > debounceDelay) {
    
    if (readState != btnState) {
      	//Serial.println("2");

        btnState = readState;
      //Serial.println("Readstater");

      if (btnState == HIGH) {
        toggle = !toggle;
        powerButtonHeld = millis();
        //Serial.println("3");
        //Serial.println(prevDebounceValue);
		//Serial.println(powerButtonHeld);
      } 
    
    }
        //Serial.println(btnState);
    	//delay(100);

  }
  prevBtnState = readState;
}

void powerOffLed() {
  for (int i = 0; i < NUMPIXELS; i++) {
    rgbValues[i].r = 0;
    rgbValues[i].g = 0;
    rgbValues[i].b = 0;
  }
  startList(0);
}

void saveLedState() {
  memcpy(rgbValuesTemp, rgbValues, NUMPIXELS);
  powerOffLed();
}

void restartLed() {
  memcpy(rgbValues, rgbValuesTemp, NUMPIXELS);
}

void powerCheck() {
  if (powerBtnToggle) {
  	saveLedState();
    powerOffLed();
    while (powerBtnToggle) {
  	  debounceControl(powerBtnPin, powerBtnToggle);
  	}
    restartLed();
  }
}

byte readLightSensor(int analogPin) {
  return 1; 
}

void startList(int brightness) {
  pixels.setBrightness(brightness);
  
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(ledPosition[i], rgbValues[i].r, rgbValues[i].g, rgbValues[i].b);
  }
  
  pixels.show();
}


void setup()
{
  initArrays();
  pinMode(sensorPin, INPUT);
  pinMode(4, INPUT);
  Serial.begin(9600);
  pixels.begin();
  bootUpSequence();
}

void loop() 
{
  static int i = 0;
  static int prevTime = 0;
  sensorValue = analogRead(sensorPin);

  
  debounceControl(powerBtnPin, powerBtnToggle);			
  //btnState = digitalRead(powerBtnPin);
  
  
  //Serial.println(powerButtonHeld);
  
  
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
  
  //pixels.setBrightness(i);

  //pixels.setPixelColor(ledPosition[0], rgbValues[0].r, rgbValues[0].g, rgbValues[0].r);
  //pixels.setPixelColor(ledPosition[1], rgbValues[1].r, rgbValues[1].g, rgbValues[1].r);

  
  startList(100);
  
  
  //pixels.show();
  
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
  powerCheck();
  
}


/*
  Dato: 12.09.23
*/

#include <Adafruit_NeoPixel.h>			// importerer bibliotek for funksjonalitet av led-listen
#define PIN 2   						// inn-pinne til led-listen
#define NUMPIXELS 8 					// antall led i listen

// oppretter pixels-objekt som kaller sin egen kode
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


typedef struct								// oppretter en struct med verdiene til en farge
{
  byte r;
  byte g;
  byte b;
} RGB;

RGB rgbValues[NUMPIXELS];					// array som holder RGB-verdiene til hver LED
byte ledPosition[NUMPIXELS];				// array som holder posisjonene til hver LED
byte rstlst[NUMPIXELS];						// temp liste som holder posisjoner i av-posisjon
RGB rgbValuesTemp[NUMPIXELS];				// temp liste som holder posisjoner i av-posisjon


const byte modeBtnPin  = 6;					// mode/speed
const byte colorBtnPin = 5;					// color/brightness pin
const byte powerBtnPin = 4;					// av/på-knapp pin
const int sensorPin = A1;					

unsigned long prevDebounceValue = 0; 		// the last time the output pin was toggled
unsigned long debounceDelay = 10;
unsigned long powerButtonHeld = 0;

const int timer = 40;						
const int powerCheckTimer = 500;			// burde vært en liste

int buttonState;
int buttonState2;
int sensorValue;
int buttonPress;


int powerBtnToggle = LOW;			     	// the current state of the output pin
int btnState;      	  						// the current reading from the input pin
int prevBtnState = LOW;						

// Burde laget struct eller liste for mye av disse verdiene men ikke nok tid.

unsigned int counting;

bool lightMode;


//int sensorMax = 0;						// bruk om tid til
//int sensorMin = 1023;						// bruk om tid til


void initArrays()
{
  for (int i = 0; i < NUMPIXELS; i++) {
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

int blinkLed(int i) 
{
  if (i < NUMPIXELS) {
  	i++;
  }
}

void setLedColor(int pos, int red, int green, int blue)
{
  rgbValues[pos].r = red;
  rgbValues[pos].g = green;
  rgbValues[pos].b = blue;
}

void bootUpSequence() 
{
  pixels.begin();
  
  for (int i = 0; i < NUMPIXELS; i++) {
  	setLedColor(i, 255, 0, 255);
  }
  
  int delTime = 250;
  
  while (millis() < 1000) {
    startList(255);    
    delay(delTime);
    startList(0);  
    delay(delTime);
  }
}

void resetList() 
{
  memcpy(ledPosition, rstlst, NUMPIXELS);
}

void shiftLEDforward()
{
  byte temp[sizeof(ledPosition)];
  memcpy(temp, ledPosition, sizeof(ledPosition));
  
  for (int i = 0; i < sizeof(ledPosition);i++) {
  	ledPosition[(i+1) % sizeof(ledPosition)] = temp[i];
  }
}

bool lastBtnCheck(int readState) 
{
  return readState != prevBtnState;					
}

void debounceControl(int btnPin, int &toggle) 
{																	// trengte referanse til toggle for å funke
  int readState = digitalRead(btnPin);

  if (lastBtnCheck(readState)) {
    if (prevDebounceValue < powerButtonHeld) {
      powerButtonHeld = millis() - powerButtonHeld;
      Serial.print("Button held for: ");
      Serial.print(powerButtonHeld);
      Serial.println(" ms");
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
      } 
    }        
  }
  prevBtnState = readState;
}

void powerOffLed()
{
  for (int i = 0; i < NUMPIXELS; i++) {
    rgbValues[i].r = 0;
    rgbValues[i].g = 0;
    rgbValues[i].b = 0;
  }
  startList(0);
}

void saveLedState() 
{
  memcpy(rgbValuesTemp, rgbValues, NUMPIXELS);
}

void restartLed() 
{
  memcpy(rgbValues, rgbValuesTemp, NUMPIXELS);
}

void powerCheck() 
{
  if (powerBtnToggle) {
  	saveLedState();
    powerOffLed();
    while (powerBtnToggle) {
  	  debounceControl(powerBtnPin, powerBtnToggle);
  	}
    restartLed();
  }
}

byte lightConstrain(int lightVal)			// bitshift for bedre tall
{
  return lightVal >> 3;
}

byte lightBrightnessControl(int analogPin)
{
  int readBrightness = analogRead(analogPin);
  return lightConstrain(readBrightness);
}

void checkLightMode()
{
  if (powerButtonHeld > 300) {
    startList(lightBrightnessControl(sensorPin));
    lightMode = true;
  } else {
  	lightMode = false;
  }
}

void startList(int brightness) 
{
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
  bootUpSequence();
}

void loop() 
{
  static int i = 0;
  static int prevTime = 0;
  sensorValue = analogRead(sensorPin);
  checkLightMode();

  debounceControl(powerBtnPin, powerBtnToggle);
  //devounceControl(colorBtnPin, colorBtnToggle);
  //btnState = digitalRead(powerBtnPin);
  
  
  //Serial.println(powerButtonHeld);
  
  
  //Serial.println(btnState);
  
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

  if (!lightMode) {
    startList(255);
  }
  powerCheck();
  
}


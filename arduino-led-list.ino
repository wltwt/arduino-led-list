/*
  Dato: 12.09.23
*/

#include <Adafruit_NeoPixel.h>			          // importerer bibliotek for funksjonalitet av led-listen
#define PIN 2   						                  // inn-pinne til led-listen
#define NUMPIXELS 8 				                	// antall led i listen

// oppretter pixels-objekt som kaller sin egen kode
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


struct RGB						// oppretter en struct med verdiene til en farge
{
  byte r;
  byte g;
  byte b;
};

struct Button
{
  byte btnPin;
  int state;
  int prevState;
  int prevDebounce;
  int toggle;
  int presses;       
  unsigned long held;
  unsigned long heldInstant;
};

RGB rgbValues[NUMPIXELS];					// array som holder RGB-verdiene til hver LED
byte ledPosition[NUMPIXELS];				// array som holder posisjonene til hver LED
byte rstlst[NUMPIXELS];						// temp liste som holder posisjoner i av-posisjon
RGB rgbValuesTemp[NUMPIXELS];				// temp liste som holder posisjoner i av-posisjon

const int sensorPin = A1;					
unsigned long debounceDelay = 10;
const int timer = 40;						
const int powerCheckTimer = 500;			// burde vært en liste

int sensorValue;
unsigned int counting;
bool lightMode;
//int sensorMax = 0;						// bruk om tid til
//int sensorMin = 1023;						// bruk om tid til

Button powerBtn, colorBtn, modeBtn;

void initButtons() {
 powerBtn = {4, LOW, 0, LOW, 0, 0, 0, 0};
 colorBtn = {5, LOW, 0, LOW, 0, 0, 0, 0};
 modeBtn = {6, LOW, 0, LOW, 0, 0, 0, 0};
}

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

void bootUpSequence() 
{
  pixels.begin();
  
  for (int i = 0; i < NUMPIXELS; i++) {
  	setLedColor(i, random(255), random(255), random(255));
  }
  
  int delTime = 300;
  
  while (millis() < 1200) {
    startList(255);    
    delay(delTime);
    startList(0);  
    delay(delTime);
  }
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

void debounceControl(struct Button &button)
{															
  int readState = digitalRead(button.btnPin);
  if (readState != button.prevState) {
    if (button.prevDebounce < button.held) {
      button.held = millis() - button.held;
      button.heldInstant = button.held;
      Serial.print("Button held for: ");
      Serial.print(button.heldInstant);
      Serial.println(" ms");
      button.presses++;
    }
  	button.prevDebounce = millis();
  }
  
  if ((millis() - button.prevDebounce) > debounceDelay) {
    
    if (readState != button.state) {
        button.state = readState;
      if (button.state == HIGH) {
        button.toggle = !button.toggle;
        button.held = millis();
      } 
    }        
  }
  button.prevState = readState;
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

void powerCheck(struct Button &button) 
{
  //Serial.println(newPowerBtnHeld);
  if (button.heldInstant > 300 && button.heldInstant != 0) {
  	saveLedState();
    powerOffLed();
    button.heldInstant = 1;
    while (button.heldInstant < 300 && button.heldInstant != 0) {
  	  debounceControl(button);
  	}
    button.heldInstant = 0;
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

void checkLightMode(struct Button &button)
{
  int temp = button.presses;

  if (button.heldInstant > 500 || lightMode) {
    startList(lightBrightnessControl(sensorPin));
    if (button.heldInstant < 500) {
      button.heldInstant = 0;
      lightMode = true;
    } else if (button.heldInstant > 500 && lightMode) {
      lightMode = false;
      button.heldInstant = 0;
    } else {
      lightMode = true;
      button.heldInstant = 0;
    }
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
  randomSeed(analogRead(0));

  initButtons();
  
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
  //sensorValue = analogRead(sensorPin);

  debounceControl(powerBtn);
  delay(10);
  debounceControl(colorBtn);
  delay(10);                                              // Usikker hvorfor men ser ut som koden kjører mye bedre med delay her
  debounceControl(modeBtn);
  delay(10);
  checkLightMode(colorBtn);                               // når det er mørkt lyser den mindre

  
  
  if (millis() - prevTime > timer){
    prevTime = millis();
    shiftLEDforward();
    //Serial.println(powerBtnPresses);
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
  powerCheck(powerBtn);
  
}


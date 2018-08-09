/* 
 * @file    main.ino
 * @author  Nazareno Lorenzo (nazarenol)
 * @date    09/08/2017
 * @version 0.1
 * 
 * @brief Arduino Midi controller footswitch made for fun
 *
 * @section DESCRIPTION
 */

#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

/*
 * We assume that each PEDAL_BUTTON has a LED.
 */
const byte PORTS_PEDAL_BUTTON[] = {0, 1, 2, 3, 4};
const byte PORTS_PEDAL_BUTTON_LEDS[] = {8, 9, 10, 11, 12}; // TODO: The led 12 does not really exist ATM
const byte PEDAL_BUTTON_COUNT = 5;

/*
 * Other buttons
 */
const byte PORT_LOOPER_BUTTON = 7;
const byte PORT_PRESET_UP = 5;
const byte PORT_PRESET_DOWN = 6;

/*
 * SCREEN
 */
const byte SCREEN_BACKLIGHT_PIN = 3;
const byte SCREEN_ADDRESS = 0x27;
LiquidCrystal_I2C lcd(SCREEN_ADDRESS, 2, 1, 0, 4, 5, 6, 7, SCREEN_BACKLIGHT_PIN, POSITIVE);

// Creat a set of new characters
const uint8_t CHAR_BITMAP[][8] = {
  {0xc, 0x12, 0x12, 0xc, 0, 0, 0, 0},
  {0x6, 0x9, 0x9, 0x6, 0, 0, 0, 0},
  {0x0, 0x6, 0x9, 0x9, 0x6, 0, 0, 0x0},
  {0x0, 0xc, 0x12, 0x12, 0xc, 0, 0, 0x0},
  {0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0, 0x0},
  {0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0, 0x0},
  {0x0, 0x0, 0x0, 0x6, 0x9, 0x9, 0x6, 0x0},
  {0x0, 0x0, 0x0, 0xc, 0x12, 0x12, 0xc, 0x0}
};

void setup() {
  doBoardSetup();
  doScreenSetup();
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
	digitalWrite(PORTS_PEDAL_BUTTON_LEDS[0], digitalRead(PORTS_PEDAL_BUTTON[0]));
	digitalWrite(PORTS_PEDAL_BUTTON_LEDS[1], digitalRead(PORTS_PEDAL_BUTTON[1]));
	digitalWrite(PORTS_PEDAL_BUTTON_LEDS[2], digitalRead(PORTS_PEDAL_BUTTON[2]));
	digitalWrite(PORTS_PEDAL_BUTTON_LEDS[3], digitalRead(PORTS_PEDAL_BUTTON[3]));
	// digitalWrite(led2, digitalRead(but2));
	// digitalWrite(led3, digitalRead(but3));
	// digitalWrite(led4, digitalRead(but4));
	screenLoop();
	delay(200);
}

void doBoardSetup()
{
  for (byte portIndex = 0; portIndex < PEDAL_BUTTON_COUNT; portIndex++) {
    pinMode(PORTS_PEDAL_BUTTON_LEDS[portIndex], OUTPUT);
    initializeButton(PORTS_PEDAL_BUTTON[portIndex]);
  }

  initializeButton(PORT_LOOPER_BUTTON);
  initializeButton(PORT_PRESET_UP);
  initializeButton(PORT_PRESET_DOWN);
}

void doScreenSetup()
{
  int charBitmapSize = (sizeof(CHAR_BITMAP) / sizeof(CHAR_BITMAP[0]));

  // Switch on the backlight
  pinMode(SCREEN_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(SCREEN_BACKLIGHT_PIN, HIGH);

  lcd.begin(20,4);

  for (int i=0;i < charBitmapSize;i++) {
    lcd.createChar(i, (uint8_t *) CHAR_BITMAP[i]);
  }

  lcd.home();
  lcd.print("LaPedalera");
  lcd.setCursor(0,2);
  lcd.print("@nazarenol");
  delay(3000);
}

void initializeButton(byte port)
{
  // Initialize as input and init the pullup resistor
  pinMode(port, INPUT_PULLUP);
  digitalWrite(port, HIGH);
}

void screenLoop()
{
  lcd.home ();
  // Do a little animation by writing to the same location
  for (int i=0; i<2; i++) {
  	for (int j=0; j<16; j++) {
  	 lcd.print(char(random(7)));
  	}

  	lcd.setCursor(0, 1);
  }
  // delay (200);
}

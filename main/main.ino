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
#include <MIDI.h>
#include <Button.h>

/*
 * We assume that each PEDAL_BUTTON has a LED.
 */
const Button BUTTONS[] = {
  Button(0, PULLUP),
  Button(1, PULLUP),
  Button(2, PULLUP),
  Button(3, PULLUP),
  Button(4, PULLUP)
};
const byte LEDS_PORTS[] = {8, 9, 10, 11, 12}; // TODO: The led 12 does not really exist ATM
const byte BUTTON_COUNT = 5;

/*
 * Other buttons
 */
const Button BUTTON_MODE_UP = Button(5, PULLUP);
const Button BUTTON_MODE_DOWN = Button(6, PULLUP);
const Button BUTTON_LOOPER = Button(7, PULLUP);

/*
 * SCREEN
 */
const byte SCREEN_BACKLIGHT_PIN = 3;
const byte SCREEN_ADDRESS = 0x27;
LiquidCrystal_I2C lcd(SCREEN_ADDRESS, 2, 1, 0, 4, 5, 6, 7, SCREEN_BACKLIGHT_PIN, POSITIVE);

/*
 * MIDI
 */
MIDI_CREATE_DEFAULT_INSTANCE();

/*
 * MODE
 */ 
const String MODES[] = {
  "Bajo1",
  "Bajo2",
  "Bajo3",
  "Bajo4",
};
const byte MODES_COUNT = 4;
byte selectedMode = 0;

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
  MIDI.begin();
  doLedsSetup();
  doScreenSetup();

  delay(200);
}

// the loop function runs over and over again forever
void loop() {
	digitalWrite(LEDS_PORTS[0], BUTTONS[0].isPressed());
	digitalWrite(LEDS_PORTS[1], BUTTONS[1].isPressed());
	digitalWrite(LEDS_PORTS[2], BUTTONS[2].isPressed());
	digitalWrite(LEDS_PORTS[3], BUTTONS[3].isPressed());
  
	screenLoop();
  modeChangeLoop();

  delay(100);
}

void doLedsSetup()
{
  for (byte portIndex = 0; portIndex < BUTTON_COUNT; portIndex++) {
    pinMode(LEDS_PORTS[portIndex], OUTPUT);
  }
}

void doScreenSetup()
{
  int charBitmapSize = (sizeof(CHAR_BITMAP) / sizeof(CHAR_BITMAP[0]));

  lcd.begin(20,4);

  for (int i=0;i < charBitmapSize;i++) {
    lcd.createChar(i, (uint8_t *) CHAR_BITMAP[i]);
  }

  lcd.home();
  lcd.print("LaPedalera");
  lcd.setCursor(0,2);
  lcd.print("@nazarenol");
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
}

void modeChangeLoop() {
  if (BUTTON_MODE_UP.uniquePress()) {
    if (selectedMode +1 >= BUTTON_COUNT) {
      selectedMode = 0;
    } else {
      selectedMode++;
    }
  } else if (BUTTON_MODE_DOWN.uniquePress()) {
    if (selectedMode == 0) {
      selectedMode = BUTTON_COUNT -1;
    } else {
      selectedMode--;
    }
  } else {
    return;
  }

  lcd.setCursor(0,2);
  lcd.print(MODES[selectedMode]); //Padded to 20 (screen width)
}

void printModeName();



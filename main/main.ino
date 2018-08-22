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
  Button(A0, PULLUP),
  Button(2, PULLUP),
  Button(3, PULLUP),
  Button(4, PULLUP)
};
const byte LEDS_PORTS[] = {8, 9, 10, 11, 12}; // TODO: The led 12 does not really exist ATM
const byte BUTTON_COUNT = 5;
boolean buttonToggled[] = {false, false, false, false, false};

/*
 * Other buttons
 */
const Button BUTTON_MODE_DOWN = Button(5, PULLUP);
const Button BUTTON_MODE_UP = Button(6, PULLUP);
const Button BUTTON_LOOPER = Button(7, PULLUP);

/*
 * SCREEN
 */
const byte SCREEN_BACKLIGHT_PIN = 3;
const byte SCREEN_ADDRESS = 0x27;
const String SCREEN_CLEAR_LINE = "                    ";
LiquidCrystal_I2C lcd(SCREEN_ADDRESS, 2, 1, 0, 4, 5, 6, 7, SCREEN_BACKLIGHT_PIN, POSITIVE);
unsigned long lastRefreshTimeMs = millis();

/*
 * MIDI
 */
MIDI_CREATE_DEFAULT_INSTANCE();
const byte MIDI_CHANNEL = 1;
const byte STOMP_BUTTON_CC_NUMBER = 20;


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

  delay(800);

  printModeName();
}

// the loop function runs over and over again forever
void loop() {
  screenLoop();
  modeChangeLoop();
  stompSwitchesLoop();
  delay(50);
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
  lcd.print("PEDALCITO!");
  lcd.setCursor(0, 2);
  lcd.print("@nazarenol");
}

void screenLoop()
{
  lcd.home ();
  // Do a little animation by writing to the same location
  if (millis() - lastRefreshTimeMs > 200) {
    lastRefreshTimeMs = millis();

    for (int i=0; i<2; i++) {
      for (int j=0; j<16; j++) {
       lcd.print(char(random(7)));
      }

      lcd.setCursor(0, 1);
    }
  }
}

void modeChangeLoop() {
  if (BUTTON_MODE_UP.uniquePress()) {
    if (selectedMode +1 >= MODES_COUNT) {
      selectedMode = 0;
    } else {
      selectedMode++;
    }
  } else if (BUTTON_MODE_DOWN.uniquePress()) {
    if (selectedMode == 0) {
      selectedMode = MODES_COUNT -1;
    } else {
      selectedMode--;
    }
  } else {
    return;
  }

  printModeName();
  sendProgramChange(selectedMode);
}

void stompSwitchesLoop() {
  // Turn on the LED accordingly
  for (int i=0; i <= 4; i++){
    if (BUTTONS[i].uniquePress()) {
      buttonToggled[i] = !buttonToggled[i];
      digitalWrite(LEDS_PORTS[i], buttonToggled[i]);

      MIDI.sendControlChange(STOMP_BUTTON_CC_NUMBER + i, buttonToggled[i] ? 0 : 127, MIDI_CHANNEL);
    }
  }


}

void printModeName() {
  lcd.setCursor(0, 2);
  lcd.print(SCREEN_CLEAR_LINE);
  lcd.setCursor(0, 2);
  lcd.print(MODES[selectedMode]);
}

void sendProgramChange(byte program) {
  MIDI.sendProgramChange(program, MIDI_CHANNEL);
}


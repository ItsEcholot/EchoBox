#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define LCD_I2C_ADDR 0x3F
#define LCD_CHAR_BAT (uint8_t) 5
#define LCD_CHAR_RELAY (uint8_t) 6
#define LCD_CHAR_SPEAKER (uint8_t) 7
#define BAT_MAX_V 12.6

LiquidCrystal_PCF8574 lcd(LCD_I2C_ADDR);
bool lcd_layout_setup = false;
bool lcd_v_low_screen = false;
bool lcd_backlight = false;

const int wave0[8] = {0b00000, 0b00000, 0b00000, 0b00100, 0b00100, 0b00100, 0b00100, 0b00100};
const int wave1[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b00100, 0b00100, 0b00100};
const int wave2[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b00100, 0b00100};
const int wave3[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00100, 0b00100};
const int wave4[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00100};
const int battery100[8] = {
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
const int battery80[8] = {
  0b01110,
  0b11111,
  0b10001,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
const int battery60[8] = {
  0b01110,
  0b11111,
  0b10001,
  0b10001,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
const int battery40[8] = {
  0b01110,
  0b11111,
  0b10001,
  0b10001,
  0b10001,
  0b11111,
  0b11111,
  0b11111
};
const int battery20[8] = {
  0b01110,
  0b11111,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b11111,
  0b11111
};
const int relay[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00001,
  0b00010,
  0b00100,
  0b00100,
  0b00100
};
const int speaker[8] = {
  0b00000,
  0b00100,
  0b10010,
  0b01001,
  0b01001,
  0b10010,
  0b00100,
  0b00000
};
const int (*selected_battery_icon)[8] = &battery100;

void setupLCD() {
  lcd.begin(20,4);
  lcd.createChar(0, (int*)wave0);
  lcd.createChar(1, (int*)wave1);
  lcd.createChar(2, (int*)wave2);
  lcd.createChar(3, (int*)wave3);
  lcd.createChar(4, (int*)wave4);
  lcd.createChar(LCD_CHAR_BAT, (int*)battery20);
  lcd.createChar(LCD_CHAR_RELAY, (int*)relay);
  lcd.createChar(LCD_CHAR_SPEAKER, (int*)speaker);
  
  turnOnBacklightLCD();
  lcd.clear();
  printTitleLCD();
  printBootingLCD();
}

void setupLayoutLCD() {
  lcd.noBlink();
  lcd.clear();
  printTitleLCD();
  lcd.setCursor(0, 2);
  lcd.write(LCD_CHAR_BAT);
  lcd.setCursor(0, 3);
  lcd.write(LCD_CHAR_SPEAKER);
  lcd.setCursor(9, 3);
  lcd.write(LCD_CHAR_RELAY);
}

void printTitleLCD() {
  lcd.setCursor(6, 0);
  lcd.print(F("EchoBox"));
}

void printTitleWaveLCD() {
  if (interrupt_count % 2 != 0) return;
  lcd.setCursor(0, 0);
  for (uint8_t i = 0; i < 6; i++) {
    lcd.write((uint8_t) ((interrupt_count+i) % 5));
  }
  lcd.setCursor(13, 0);
  for (uint8_t i = 13; i < 20; i++) {
    lcd.write((uint8_t) ((interrupt_count+i) % 5));
  }
}

void printBootingLCD() {
  lcd.setCursor(5, 1);
  lcd.print(F("Booting v1"));
  lcd.setCursor(9, 2);
  lcd.blink();
}

void printVBatLCD() {
  lcd.setCursor(0, 2);
  // 12.6v full
  // per 20% -> 0.7v
  // 9.1v empty
  // diff: 3.5v
  if (v_value > 11.9) {
    lcd.createChar(LCD_CHAR_BAT, (int*)battery100);
    selected_battery_icon = &battery100;
  } else if (v_value > 11.2) {
    lcd.createChar(LCD_CHAR_BAT, (int*)battery80);
    selected_battery_icon = &battery80;
  } else if (v_value > 10.5) {
    lcd.createChar(LCD_CHAR_BAT, (int*)battery60);
    selected_battery_icon = &battery60;
  } else if (v_value > 9.8) {
    lcd.createChar(LCD_CHAR_BAT, (int*)battery40);
    selected_battery_icon = &battery40;
  } else {
    lcd.createChar(LCD_CHAR_BAT, (int*)battery20);
    selected_battery_icon = &battery20;
  }
  lcd.write(LCD_CHAR_BAT);

  lcd.setCursor(2, 2);
  lcd.print(v_value);
  lcd.print(F("V "));

  lcd.setCursor(9, 2);
  lcd.print(max(100 - (int)((BAT_MAX_V - v_value) / 0.035), 0));
  lcd.print(F("% "));
}

void setupVLowLayoutLCD() {
  lcd.noBlink();
  lcd.clear();
  printTitleLCD();
  lcd.setCursor(2, 1);
  lcd.print(F("Bat. voltage low"));
  lcd.setCursor(7, 2);
  lcd.print(v_value);
  lcd.print(F("V"));
}

void printRelayStateLCD() {
  lcd.setCursor(11, 3);
  lcd.print(relay_state ? F("On ") : F("Off"));
}

void printAAmpLCD() {
  lcd.setCursor(2, 3);
  lcd.print(a_value);
  lcd.print(F("A "));
}

void turnOnBacklightLCD() {
  lcd.setBacklight(255);
  lcd_backlight = true;
}

void turnOffBacklightLCD() {
  lcd.setBacklight(0);
  lcd_backlight = false;
}

void toggleBacklightLCD() {
  if (lcd_backlight) {
    turnOffBacklightLCD();
  } else {
    turnOnBacklightLCD();
  }
}

void loopLCD() {
  if (!v_low && !lcd_layout_setup && v_first_read_done && a_first_read_done) {
    setupLayoutLCD();
    lcd_layout_setup = true;
  } else if (!v_low && v_first_read_done && a_first_read_done) {
    printTitleWaveLCD();
    if (lcd_do_update) {
      printVBatLCD();
      printRelayStateLCD();
      printAAmpLCD();
      lcd_do_update = false;
    }
  }

  if (!lcd_v_low_screen && v_low) {
    setupVLowLayoutLCD();
    lcd_v_low_screen = true;
  }

  if (v_low && v_low_loop_count <= 15) {
    setupVLowLayoutLCD();
    toggleBacklightLCD();
  }
}

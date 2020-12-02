#include <avr/wdt.h>
#include <TimerOne.h>
#include <LowPower.h>

//#define DEBUG_SERIAL

uint8_t volatile interrupt_count = 0;
bool volatile lcd_do_update = false;

float volatile v_value = 0;
bool volatile v_first_read_done = false;
bool volatile v_low = false;
uint8_t v_low_loop_count = 0;

float volatile a_value = 0;
bool volatile a_first_read_done = false;

bool volatile relay_state = false;

/*
 * Relay----D2|  A  |     /-R(10k)----BAT+
 *   |        |  R  |     |
 *   \-----GND|  D  |A1---/-R(4.7k)
 *            |  U  |          |
 * BAT+       |  I  |          |
 * |A|VCC---5V|  N  |GND-------/-\----BAT-
 * |M|OUT---A2|  O  |            |       
 * |P|GND--GND|     |            \----GND|              |
 * AMP+       |     |5V---------------VCC|   Display    |
 *            |     |A4---------------SDA|              |
 *            |     |A5---------------SCL|              |
 *            |     |
 * BAT+----VIN|     |GND--------------BAT-
 */

void setup() {
  // put your setup code here, to run once:
  wdt_disable();

  setupRelayPin();
  setupLCD();

  #ifdef DEBUG_SERIAL
  Serial.begin(9600);
  #endif

  delay(3000);
  Timer1.initialize(50000); // Every 50ms
  Timer1.attachInterrupt(systemCheckInter);
  wdt_enable(WDTO_1S);
}

bool volatile flip = false;
void loop() {
  // put your main code here, to run repeatedly:
  wdt_reset();
  if (v_low) {
    systemCheckInter();
    if (v_low_loop_count <= 16) {
      printDebugSerial(F("Going to sleep 1s\n"), -1);
      LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    } else {
      printDebugSerial(F("Going to sleep forever\n"), -1);
      turnOffBacklightLCD();
      delay(100);
      LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    }
  }

  if (lcd_do_update) {
    printDebugSerial(F("Bat. Voltage "), v_value);
    printDebugSerial(F("Amperage "), a_value);
  }
  loopLCD();
}

void systemCheckInter(void) {
  digitalWrite(LED_BUILTIN, HIGH);
  getV();
  checkV();
  getA();
  interrupt_count++;
  digitalWrite(LED_BUILTIN, LOW);
}

void printDebugSerial(const __FlashStringHelper* string, float value) {
  #ifdef DEBUG_SERIAL
  if (string != NULL) Serial.print(string);
  if (value != -1) {
    Serial.print(value);
    Serial.println();
  }
  Serial.flush();
  #endif
}

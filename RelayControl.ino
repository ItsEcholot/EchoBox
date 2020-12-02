#define RELAY_PIN 3

void setupRelayPin() {
  pinMode(RELAY_PIN, OUTPUT);
}

void turnOnRelay() {
  digitalWrite(RELAY_PIN, HIGH);
  relay_state = true;
  lcd_do_update = true;
}

void turnOffRelay() {
  digitalWrite(RELAY_PIN, LOW);
  relay_state = false;
  lcd_do_update = true;
}

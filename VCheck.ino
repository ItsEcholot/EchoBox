#define V_PIN A1
#define V_LOW_BAT_LIMIT 9.1
//#define DEBUG_V_DISABLE_CHECK

uint8_t volatile v_samples_target_num = 20;
uint8_t volatile v_samples_num = 0;
int volatile v_sum = 0;

void getV() {
  if (v_samples_num < v_samples_target_num) {
    v_sum += analogRead(V_PIN);
    v_samples_num++;
  } else {
    v_first_read_done = true;
    v_value = ((float)v_sum / (float)v_samples_target_num * 5.03) / 1024.0;
    v_value *= 3.04;//3.125;//3.36111;//2.775;
    v_samples_num = 0;
    v_sum = 0;
    lcd_do_update = true;
  }
}

void checkV() {
  #ifndef DEBUG_V_DISABLE_CHECK
  if (v_first_read_done && v_value < V_LOW_BAT_LIMIT && !v_low) { // Low Bat.
    Timer1.stop();
    wdt_disable();
    turnOffRelay();
    v_low = true;
    v_samples_target_num = 2;
  } else 
  #endif
  if (!v_low && v_first_read_done) {
    turnOnRelay();
  } else if (v_low) {
    v_low_loop_count++;
  }
}

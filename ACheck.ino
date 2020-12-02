#define A_PIN A2

uint8_t volatile a_samples_target_num = 20;
uint8_t volatile a_samples_num = 0;
int volatile a_sum = 0;

void getA() {
  if (a_samples_num < a_samples_target_num) {
    a_sum += analogRead(A_PIN);
    a_samples_num++;
  } else {
    a_first_read_done = true;
    a_value = (2500 - ((((float) a_sum / (float) a_samples_target_num) / 1024.0) * 5000)) / 100;
    a_samples_num = 0;
    a_sum = 0;
    lcd_do_update = true;
  }

  if (v_low && a_samples_target_num != 2) {
    a_samples_target_num = 2;
  }
}

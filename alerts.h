#ifndef ALERTS_H
#define ALERTS_H

#include "config.h"
#include "sensors.h"

// INIT PWM
void pwm_init() {
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // Pentru anod comun: pornim cu LED stins (HIGH = stins)
  // Pentru catod comun: LOW = stins
  analogWrite(PIN_LED_R, LED_VAL(0));
  analogWrite(PIN_LED_G, LED_VAL(0));
  analogWrite(PIN_LED_B, LED_VAL(0));
}

// CONTROL LED RGB
void led_set(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(PIN_LED_R, LED_VAL(r));
  analogWrite(PIN_LED_G, LED_VAL(g));
  analogWrite(PIN_LED_B, LED_VAL(b));
}

void led_off() {
  led_set(0, 0, 0);
}

// ALERTE
void alerts_update(PlantState* s, bool snoozed) {
  static uint32_t lastBeep = 0;
  static bool buzzerOn = false;
  uint32_t now = millis();
  
  if (snoozed) {
    // Snooze activ: doar LED pulseaza verde-galben, fara buzzer
    uint32_t fade = (now / 10) % 512;
    if (fade > 255) fade = 511 - fade;
    led_set(fade, fade, 0);  // galben
    if (buzzerOn) {
      noTone(PIN_BUZZER);
      buzzerOn = false;
    }
    return;
  }
  
  switch (s->status) {
    case 0:  // OK - verde stabil
      led_set(0, 100, 0);
      if (buzzerOn) {
        noTone(PIN_BUZZER);
        buzzerOn = false;
      }
      break;
      
    case 1:  // WARN - galben, beep ocazional
      led_set(150, 100, 0);
      if (now - lastBeep > 3000) {
        tone(PIN_BUZZER, 1000, 100);
        lastBeep = now;
      }
      break;
      
    case 2:  // CRITIC - pulseaza rosu, beep mai des
      {
        uint32_t pulse = (now / 5) % 512;
        if (pulse > 255) pulse = 511 - pulse;
        led_set(pulse, 0, 0);
      }
      if (now - lastBeep > 1000) {
        tone(PIN_BUZZER, 2000, 200);
        lastBeep = now;
      }
      break;
  }
}

#endif
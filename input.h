#ifndef INPUT_H
#define INPUT_H

#include "config.h"

enum button_event_t {
  BTN_NONE,
  BTN1_SHORT,
  BTN1_LONG,
  BTN2_SHORT,
  BTN2_LONG
};

// Stari interne pentru fiecare buton (state machine de debouncing)
struct ButtonState {
  uint32_t pressStart;      // millis() la apasare
  uint32_t lastChange;      // pt debouncing
  bool pressed;             // stare curenta
  bool longFired;           // apasare lunga deja semnalata
};

volatile ButtonState btn1State = {0, 0, false, false};
volatile ButtonState btn2State = {0, 0, false, false};
volatile button_event_t eventQueue = BTN_NONE;

void btn1_isr();

// INIT BUTOANE
void buttons_init() {
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);
  
  // Btn1 pe D3 -> INT1
  attachInterrupt(digitalPinToInterrupt(PIN_BTN1), btn1_isr, CHANGE);
  
  // Btn2 pe D4 -> nu are interrupt extern, folosim PCINT
  // PCINT20 = D4 (pe PCMSK2)
  PCICR |= (1 << PCIE2);    // enable PCINT pe portul D
  PCMSK2 |= (1 << PCINT20); // enable PCINT20 (D4)
}

// ISR-uri
void btn1_isr() {
  uint32_t now = millis();
  if (now - btn1State.lastChange < DEBOUNCE_MS) return;
  btn1State.lastChange = now;
  
  bool currentlyPressed = (digitalRead(PIN_BTN1) == LOW);
  
  if (currentlyPressed && !btn1State.pressed) {
    btn1State.pressed = true;
    btn1State.pressStart = now;
    btn1State.longFired = false;
  } else if (!currentlyPressed && btn1State.pressed) {
    btn1State.pressed = false;
    uint32_t duration = now - btn1State.pressStart;
    if (!btn1State.longFired && duration < LONG_PRESS_MS) {
      eventQueue = BTN1_SHORT;
    }
  }
}

ISR(PCINT2_vect) {
  // Detectam doar schimbarile pe D4
  uint32_t now = millis();
  if (now - btn2State.lastChange < DEBOUNCE_MS) return;
  btn2State.lastChange = now;
  
  bool currentlyPressed = (digitalRead(PIN_BTN2) == LOW);
  
  if (currentlyPressed && !btn2State.pressed) {
    btn2State.pressed = true;
    btn2State.pressStart = now;
    btn2State.longFired = false;
  } else if (!currentlyPressed && btn2State.pressed) {
    btn2State.pressed = false;
    uint32_t duration = now - btn2State.pressStart;
    if (!btn2State.longFired && duration < LONG_PRESS_MS) {
      eventQueue = BTN2_SHORT;
    }
  }
}

// POLLING APASARE LUNGA
// Apasarea lunga e detectata pe loop (nu in ISR) pentru a evita probleme
void check_long_press() {
  uint32_t now = millis();
  
  if (btn1State.pressed && !btn1State.longFired) {
    if (now - btn1State.pressStart >= LONG_PRESS_MS) {
      btn1State.longFired = true;
      eventQueue = BTN1_LONG;
    }
  }
  
  if (btn2State.pressed && !btn2State.longFired) {
    if (now - btn2State.pressStart >= LONG_PRESS_MS) {
      btn2State.longFired = true;
      eventQueue = BTN2_LONG;
    }
  }
}

// CITIRE EVENIMENT (din loop)
button_event_t buttons_get_event() {
  // Verifica apasari lungi mai intai
  check_long_press();
  
  noInterrupts();
  button_event_t evt = eventQueue;
  eventQueue = BTN_NONE;
  interrupts();
  
  return evt;
}

#endif
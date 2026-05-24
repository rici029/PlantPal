#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// PINI
#define PIN_DHT       2
#define PIN_BTN1      3   // INT1
#define PIN_BTN2      4   // PCINT
#define PIN_BUZZER    6
#define PIN_LED_R     9   // OC1A (Timer1)
#define PIN_LED_G     10  // OC1B (Timer1)
#define PIN_LED_B     11  // OC2A (Timer2)

#define PIN_SOIL      A0
#define PIN_LDR       A1

// HARDWARE
#define DHT_TYPE      DHT11
#define LCD_I2C_ADDR  0x27
#define LCD_COLS      16
#define LCD_ROWS      2

#define LED_ANODE_COMMON  0

#if LED_ANODE_COMMON
  #define LED_VAL(x)  (255 - (x))
#else
  #define LED_VAL(x)  (x)
#endif

// TIMING
#define SENSOR_INTERVAL     2000   // citire senzori la 2s
#define DISPLAY_INTERVAL    500    // refresh LCD la 0.5s
#define ALERT_INTERVAL      200    // update alerte la 0.2s
#define DEBOUNCE_MS         50     // debouncing butoane
#define LONG_PRESS_MS       800    // peste 800ms = apasare lunga
#define SNOOZE_DURATION     30000  // snooze 30s

// PAGINI LCD
#define PAGE_MAIN     0   // Temp + Umid aer
#define PAGE_SOIL     1   // Sol + Lumina
#define PAGE_STATUS   2   // Stare generala + alerte
#define PAGE_HISTORY  3   // Istoric ultimele citiri
#define PAGE_EDIT     4   // Editare praguri (accesibila doar prin long-press)
#define NUM_PAGES     4   // PAGE_EDIT nu intra in ciclu normal

// CALIBRARE SENZORI
// Calibrare senzor sol capacitiv (citire ADC)
// Aer (uscat) = valoare mare; apa (umed) = valoare mica
#define SOIL_DRY      590   // valoare la aer
#define SOIL_WET      300   // valoare in apa

// Calibrare LDR (citire ADC)
#define LIGHT_DARK    50    // mana acoperita
#define LIGHT_BRIGHT  900   // lumina puternica

// ISTORIC
#define HISTORY_SIZE  10

#endif
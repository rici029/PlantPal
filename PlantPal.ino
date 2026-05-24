#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#include "config.h"
#include "sensors.h"
#include "display.h"
#include "alerts.h"
#include "input.h"
#include "state.h"

// Obiecte globale
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);
DHT dht(PIN_DHT, DHT_TYPE);

// Variabile de stare globale
PlantState state;
Thresholds thresholds;
HistoryBuffer history;

// Timestamps pentru sampling non-blocking
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastAlertUpdate = 0;
unsigned long alertSnoozeUntil = 0;

// Pagina curenta afișata pe LCD
uint8_t currentPage = PAGE_MAIN;

// Mod editare praguri
bool editMode = false;
uint8_t editParam = 0;  // 0=temp, 1=umid_aer, 2=sol, 3=lumina

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println(F("=== PlantPal v1.0 ==="));
  Serial.println(F("Step 1: Initializare..."));
  
  Serial.println(F("Step 2a: Wire init..."));
  Wire.begin();
  Wire.setClock(100000);
  Serial.println(F("Step 2a: Wire OK"));

  Serial.println(F("Step 2b: LCD begin..."));
  lcd.begin(LCD_COLS, LCD_ROWS);
  Serial.println(F("Step 2b: LCD OK"));
  
  Serial.println(F("Step 3: LCD backlight..."));
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("PlantPal v1.0"));
  Serial.println(F("Step 3: LCD print OK"));
  
  Serial.println(F("Step 4: DHT begin..."));
  dht.begin();
  Serial.println(F("Step 4: DHT OK"));
  
  Serial.println(F("Step 5: ADC init..."));
  adc_init();
  Serial.println(F("Step 5: ADC OK"));
  
  Serial.println(F("Step 6: PWM init..."));
  pwm_init();
  Serial.println(F("Step 6: PWM OK"));
  
  Serial.println(F("Step 7: Buttons init..."));
  buttons_init();
  Serial.println(F("Step 7: Buttons OK"));
  
  Serial.println(F("Step 8: Thresholds init..."));
  thresholds_init(&thresholds);
  Serial.println(F("Step 8: Thresholds OK"));
  
  Serial.println(F("Step 9: History init..."));
  history_init(&history);
  Serial.println(F("Step 9: History OK"));
  
  Serial.println(F("Step 10: Timer1 init..."));
  timer1_init();
  Serial.println(F("Step 10: Timer1 OK"));
  
  delay(1500);
  lcd.clear();
  
  Serial.println(F("=== SETUP COMPLET ==="));
  Serial.println(F("Btn1: navigatie | Btn2: snooze"));
}

void loop() {
  unsigned long now = millis();
  
  // Citire senzori la intervale fixe
  if (now - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = now;
    
    unsigned long t_start = micros();
    sensors_read(&state);
    unsigned long t_read = micros() - t_start;
    
    // Profilare cod - print pe Serial
    Serial.print(F("[PROFILE] Citire senzori: "));
    Serial.print(t_read);
    Serial.println(F(" us"));
    
    // Evaluare stare planta
    state_evaluate(&state, &thresholds);
    
    // Adauga in istoric
    history_add(&history, &state);
    
    // Print valori pe Serial
    sensors_print(&state);
  }
  
  // Procesare evenimente butoane (consumate din ISR)
  button_event_t btnEvent = buttons_get_event();
  if (btnEvent != BTN_NONE) {
    handle_button(btnEvent, now);
  }
  
  // Update display la 500ms
  if (now - lastDisplayUpdate >= DISPLAY_INTERVAL) {
    lastDisplayUpdate = now;
    display_update(&lcd, &state, &thresholds, &history, currentPage, editMode, editParam);
  }
  
  // Update alerte la 200ms
  if (now - lastAlertUpdate >= ALERT_INTERVAL) {
    lastAlertUpdate = now;
    
    bool snoozed = (alertSnoozeUntil > now);
    alerts_update(&state, snoozed);
  }
}

// Procesare apasari butoane
void handle_button(button_event_t evt, unsigned long now) {
  switch (evt) {
    case BTN1_SHORT:
      if (editMode) {
        // In editare: schimba parametrul curent
        editParam = (editParam + 1) % 5;
      } else {
        // Normal: schimba pagina LCD
        currentPage = (currentPage + 1) % NUM_PAGES;
        lcd.clear();
      }
      break;
      
    case BTN1_LONG:
      // Intra/iesi din modul editare praguri
      editMode = !editMode;
      lcd.clear();
      if (editMode) {
        Serial.println(F("[EDIT] Mod editare praguri ACTIV"));
        currentPage = PAGE_EDIT;
      } else {
        Serial.println(F("[EDIT] Mod editare DEZACTIVAT"));
        currentPage = PAGE_MAIN;
      }
      break;
      
    case BTN2_SHORT:
      if (editMode) {
        // In editare: creste pragul curent
        thresholds_increment(&thresholds, editParam);
      } else {
        // Normal: snooze alerta 30 sec
        alertSnoozeUntil = now + SNOOZE_DURATION;
        Serial.println(F("[SNOOZE] Alerte dezactivate 30s"));
        // Beep scurt de confirmare
        tone(PIN_BUZZER, 1500, 100);
      }
      break;
      
    case BTN2_LONG:
      if (editMode) {
        // In editare: reseteaza la valori default
        thresholds_init(&thresholds);
        Serial.println(F("[EDIT] Praguri resetate la default"));
      } else {
        // Normal: reset snooze + test sirena
        alertSnoozeUntil = 0;
        Serial.println(F("[SNOOZE] Anulat"));
      }
      break;
      
    default:
      break;
  }
}
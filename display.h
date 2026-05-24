#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "sensors.h"
#include "state.h"

// Caractere custom pentru LCD
byte iconDrop[8]   = {0b00100, 0b00100, 0b01110, 0b01110, 0b11111, 0b11111, 0b01110, 0b00000};
byte iconSun[8]    = {0b00100, 0b10101, 0b01110, 0b11011, 0b01110, 0b10101, 0b00100, 0b00000};
byte iconTemp[8]   = {0b00100, 0b01010, 0b01010, 0b01010, 0b10001, 0b10001, 0b01110, 0b00000};
byte iconWarn[8]   = {0b00100, 0b00100, 0b01010, 0b01010, 0b10001, 0b10101, 0b01110, 0b00000};

void display_init_icons(LiquidCrystal_I2C* lcd) {
  lcd->createChar(0, iconDrop);
  lcd->createChar(1, iconSun);
  lcd->createChar(2, iconTemp);
  lcd->createChar(3, iconWarn);
}

void display_page_main(LiquidCrystal_I2C* lcd, PlantState* s) {
  lcd->setCursor(0, 0);
  lcd->write(2);  // temp icon
  lcd->print(F(":"));
  lcd->print((int)s->temperature);
  lcd->print(F("C "));
  lcd->write(0);  // drop icon
  lcd->print(F(":"));
  lcd->print((int)s->air_humidity);
  lcd->print(F("%   "));
  
  lcd->setCursor(0, 1);
  lcd->print(F("Aer: "));
  if (s->status == 0) lcd->print(F("OK       "));
  else if (s->status == 1) lcd->print(F("Atentie  "));
  else lcd->print(F("CRITIC!  "));
}

void display_page_soil(LiquidCrystal_I2C* lcd, PlantState* s) {
  lcd->setCursor(0, 0);
  lcd->print(F("Sol: "));
  lcd->print(s->soil_moisture);
  lcd->print(F("%        "));
  
  lcd->setCursor(0, 1);
  lcd->write(1);  // sun icon
  lcd->print(F(" Lumina: "));
  lcd->print(s->light_level);
  lcd->print(F("%  "));
}

void display_page_status(LiquidCrystal_I2C* lcd, PlantState* s) {
  lcd->setCursor(0, 0);
  lcd->print(F("Status: "));
  switch (s->status) {
    case 0: lcd->print(F("OK       ")); break;
    case 1: lcd->print(F("ATENTIE  ")); break;
    case 2: lcd->print(F("CRITIC!! ")); break;
  }
  
  lcd->setCursor(0, 1);
  // Afiseaza ce parametru e cu probleme
  uint32_t t = millis() / 3000;  // schimba la 3 sec
  switch (t % 4) {
    case 0: lcd->print(F("T:"));
            lcd->print((int)s->temperature);
            lcd->print(F("C       "));
            break;
    case 1: lcd->print(F("H aer:"));
            lcd->print((int)s->air_humidity);
            lcd->print(F("%    "));
            break;
    case 2: lcd->print(F("Sol:"));
            lcd->print(s->soil_moisture);
            lcd->print(F("%       "));
            break;
    case 3: lcd->print(F("Lumina:"));
            lcd->print(s->light_level);
            lcd->print(F("%    "));
            break;
  }
}

void display_page_history(LiquidCrystal_I2C* lcd, HistoryBuffer* h) {
  lcd->setCursor(0, 0);
  lcd->print(F("Istoric ultimele"));
  
  lcd->setCursor(0, 1);
  if (h->count == 0) {
    lcd->print(F("Niciunul        "));
  } else {
    // Cycle prin ultimele citiri
    uint32_t t = millis() / 1500;  // o citire/1.5sec
    uint8_t idx = (h->head + (h->count - 1 - (t % h->count))) % HISTORY_SIZE;
    PlantState* old = &h->buffer[idx];
    
    lcd->print(F("T:"));
    lcd->print((int)old->temperature);
    lcd->print(F(" S:"));
    lcd->print(old->soil_moisture);
    lcd->print(F("% L:"));
    lcd->print(old->light_level);
    lcd->print(F("%  "));
  }
}

void display_page_edit(LiquidCrystal_I2C* lcd, Thresholds* th, uint8_t param) {
  lcd->setCursor(0, 0);
  lcd->print(F("EDIT prag "));
  
  switch (param) {
    case 0: lcd->print(F("T max  ")); break;
    case 1: lcd->print(F("T min  ")); break;
    case 2: lcd->print(F("H min  ")); break;
    case 3: lcd->print(F("Sol    ")); break;
    case 4: lcd->print(F("Lumina ")); break;
  }
  
  lcd->setCursor(0, 1);
  lcd->print(F("Val: "));
  switch (param) {
    case 0: lcd->print(th->temp_max);    lcd->print(F("C       ")); break;
    case 1: lcd->print(th->temp_min);    lcd->print(F("C       ")); break;
    case 2: lcd->print(th->humid_min);   lcd->print(F("%       ")); break;
    case 3: lcd->print(th->soil_min);    lcd->print(F("%       ")); break;
    case 4: lcd->print(th->light_min);   lcd->print(F("%       ")); break;
  }
}

void display_update(LiquidCrystal_I2C* lcd, PlantState* s, Thresholds* th, 
                    HistoryBuffer* h, uint8_t page, bool editMode, uint8_t editParam) {
  static bool iconsLoaded = false;
  if (!iconsLoaded) {
    display_init_icons(lcd);
    iconsLoaded = true;
  }
  
  if (editMode) {
    display_page_edit(lcd, th, editParam);
    return;
  }
  
  switch (page) {
    case PAGE_MAIN:    display_page_main(lcd, s);       break;
    case PAGE_SOIL:    display_page_soil(lcd, s);       break;
    case PAGE_STATUS:  display_page_status(lcd, s);     break;
    case PAGE_HISTORY: display_page_history(lcd, h);    break;
  }
}

#endif
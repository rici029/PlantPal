#ifndef STATE_H
#define STATE_H

#include "config.h"
#include "sensors.h"

// BUFFER ISTORIC CIRCULAR
struct HistoryBuffer {
  PlantState buffer[HISTORY_SIZE];
  uint8_t head;
  uint8_t count;
};

void history_init(HistoryBuffer* h) {
  h->head = 0;
  h->count = 0;
}

void history_add(HistoryBuffer* h, PlantState* s) {
  h->buffer[h->head] = *s;
  h->head = (h->head + 1) % HISTORY_SIZE;
  if (h->count < HISTORY_SIZE) h->count++;
}

// PRAGURI DEFAULT
void thresholds_init(Thresholds* t) {
  t->temp_min = 15.0;
  t->temp_max = 28.0;
  t->humid_min = 30;
  t->soil_min = 30;
  t->light_min = 20;
}

void thresholds_increment(Thresholds* t, uint8_t param) {
  switch (param) {
    case 0:  // Temp MAX
      t->temp_max += 1.0;
      if (t->temp_max > 35.0) t->temp_max = 20.0;
      
      // Verifica sa nu fie mai mic decat Temp MIN
      if (t->temp_max <= t->temp_min) {
        t->temp_max = 20.0;  // forteaza wrap mai sus
      }
      break;
    case 1:  // Temp MIN
      t->temp_min += 1.0;
      if (t->temp_min > 22.0) t->temp_min = 10.0;
      
      // Verifica sa nu depaseasca Temp MAX
      if (t->temp_min >= t->temp_max) {
        t->temp_min = 10.0;  // forteaza wrap
      }
      break;
    case 2:  // Umiditate aer min
      t->humid_min += 5;
      if (t->humid_min > 70) t->humid_min = 20;
      break;
    case 3:  // Sol min
      t->soil_min += 5;
      if (t->soil_min > 80) t->soil_min = 20;
      break;
    case 4:  // Lumina min
      t->light_min += 5;
      if (t->light_min > 70) t->light_min = 10;
      break;
  }
}

// EVALUARE STARE
void state_evaluate(PlantState* s, Thresholds* t) {
  uint8_t warnings = 0;
  uint8_t criticals = 0;
  
  // Verifica temperatura
  if (s->temperature > t->temp_max + 3) criticals++;
  else if (s->temperature > t->temp_max) warnings++;
  
  if (s->temperature < t->temp_min - 3) criticals++;
  else if (s->temperature < t->temp_min) warnings++;
  
  // Verifica umiditate aer
  if (s->air_humidity < t->humid_min - 10) criticals++;
  else if (s->air_humidity < t->humid_min) warnings++;
  
  // Verifica sol
  if (s->soil_moisture < t->soil_min - 10) criticals++;
  else if (s->soil_moisture < t->soil_min) warnings++;
  
  // Verifica lumina
  if (s->light_level < t->light_min) warnings++;
  
  // Determina statusul final
  if (criticals > 0) s->status = 2;       // CRITIC
  else if (warnings > 0) s->status = 1;   // WARN
  else s->status = 0;                     // OK
}


void timer1_init() {
}

#endif
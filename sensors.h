#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"

// Structura cu starea curenta a plantei
struct PlantState {
  float temperature;      // °C
  float air_humidity;     // %
  uint8_t soil_moisture;  // 0-100%
  uint8_t light_level;    // 0-100%
  uint16_t soil_raw;      // citire ADC bruta
  uint16_t light_raw;     // citire ADC bruta
  uint8_t status;         // 0=OK, 1=WARN, 2=CRITIC
  uint32_t timestamp;     // millis() la citire
};

// Praguri configurabile
struct Thresholds {
  float temp_min;
  float temp_max;
  uint8_t humid_min;
  uint8_t soil_min;
  uint8_t light_min;
};

// Forward declarations
extern DHT dht;

// ACCES DIRECT ADC
// In loc de analogRead() (care e lent ~100us), folosim registre direct

void adc_init() {
  // ADMUX: REFS1=0, REFS0=1 -> referinta AVCC (5V)
  ADMUX = (1 << REFS0);
  
  // ADCSRA: ADEN=1 enable, prescaler 128 (16MHz/128 = 125kHz, in range 50-200kHz)
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel) {
  // Selecteaza canalul ADC (pastreaza REFS din ADMUX)
  ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
  
  // Start conversion
  ADCSRA |= (1 << ADSC);
  
  // Asteapta finalizarea (bit ADSC se reseteaza automat)
  while (ADCSRA & (1 << ADSC));
  
  // Returneaza valoarea pe 10 biti
  return ADC;
}

// CITIRE SENZORI

void sensors_read(PlantState* s) {
  // Citire DHT11 (folosim biblioteca pt protocol complex 1-wire)
  s->temperature = dht.readTemperature();
  s->air_humidity = dht.readHumidity();
  
  // Citire ADC bare-metal pt sol si lumina
  s->soil_raw = adc_read(0);   // A0
  s->light_raw = adc_read(1);  // A1
  
  // Conversie in procente
  // Senzor capacitiv: valoare mica = umed, valoare mare = uscat
  int soil_p = map(s->soil_raw, SOIL_DRY, SOIL_WET, 0, 100);
  s->soil_moisture = constrain(soil_p, 0, 100);
  
  // LDR: valoare mica = intuneric, valoare mare = lumina
  int light_p = map(s->light_raw, LIGHT_DARK, LIGHT_BRIGHT, 0, 100);
  s->light_level = constrain(light_p, 0, 100);
  
  s->timestamp = millis();
  
  // Verificare validitate DHT
  if (isnan(s->temperature) || isnan(s->air_humidity)) {
    s->temperature = 0;
    s->air_humidity = 0;
  }
}

void sensors_print(PlantState* s) {
  Serial.print(F("T: "));
  Serial.print(s->temperature, 1);
  Serial.print(F("C | H: "));
  Serial.print(s->air_humidity, 0);
  Serial.print(F("% | Sol: "));
  Serial.print(s->soil_moisture);
  Serial.print(F("% | Lum: "));
  Serial.print(s->light_level);
  Serial.print(F("% | Status: "));
  
  switch (s->status) {
    case 0: Serial.println(F("OK")); break;
    case 1: Serial.println(F("WARN")); break;
    case 2: Serial.println(F("CRITIC")); break;
  }
}

#endif
# PlantPal 🌱

Sistem inteligent de monitorizare a plantelor de apartament, construit pe Arduino Uno.

## Despre proiect

PlantPal monitorizeaza in timp real conditiile de mediu pentru plante (temperatura, umiditate aer, umiditate sol, lumina) si ofera feedback multi-canal utilizatorului:
- **Afisaj LCD** cu 4 pagini de informatii
- **LED RGB** ca indicator vizual al starii (verde/galben/rosu)
- **Buzzer pasiv** pentru alerte sonore gradate
- **Butoane** pentru navigare, snooze si configurare praguri

## Componente hardware

- Arduino Uno R3 (ATmega328P)
- DHT11 - senzor temperatura si umiditate aer
- Senzor capacitiv umiditate sol v2.0
- Fotorezistor LDR + rezistor 10k ohm (divizor de tensiune)
- LCD 1602 cu interfata I2C
- Modul LED RGB KY-016
- Buzzer pasiv
- 2x push-buttons
- Breadboard + fire jumper
- Alimentare: USB / power bank 10000 mAh

## Functionalitati

- **3 stari de monitorizare:** OK (verde), Atentie (galben), Critic (rosu pulsator)
- **4 pagini LCD:** main, sol/lumina, status, istoric
- **Buffer circular** cu istoric ultimelor 10 citiri
- **Mod editare praguri** runtime (apasare lunga buton 1)
- **Snooze alerta** 30 secunde (apasare scurta buton 2)
- **Detectie apasare scurta/lunga** prin masurare durata in ISR

## Laboratoare integrate

| Lab | Folosit pentru |
|-----|----------------|
| GPIO | Butoane, DHT11, LED RGB |
| Intreruperi | INT1 pentru buton 1, polling pentru buton 2 |
| Timere + PWM | LED RGB (3 canale), buzzer |
| ADC | Citire bare-metal senzori sol si LDR (acces direct la registre) |
| I2C | Comunicare cu LCD |

## Cum se foloseste

**Mod normal:**
- Apasare scurta **Buton 1** → schimba pagina LCD
- Apasare scurta **Buton 2** → snooze 30 sec
- Apasare lunga **Buton 2** → anuleaza snooze

**Mod editare praguri:**
- Apasare lunga **Buton 1** → intra in editare
- Apasare scurta **Buton 1** → cycle prin parametri (T max, T min, H, Sol, Lumina)
- Apasare scurta **Buton 2** → creste pragul curent
- Apasare lunga **Buton 2** → reset la valori default
- Apasare lunga **Buton 1** → iese din editare

## Biblioteci necesare

- `Wire.h` (standard Arduino)
- `LiquidCrystal_I2C` by Frank de Brabander
- `DHT sensor library` by Adafruit (+ Adafruit Unified Sensor)

## Documentatie completa

Documentatia detaliata a proiectului este disponibila pe wiki-ul cursului PM:  
[Link wiki proiect](https://ocw.cs.pub.ro/courses/pm/prj2026/florin.stancu/bogdan.panaitiu)


## Autor

Realizat de Panaitiu Bogdan-Ionut - Grupa 334CA - Facultatea de Automatica si Calculatoare, UPB

Curs: PM (Proiectarea cu Microprocesoare), 2025-2026

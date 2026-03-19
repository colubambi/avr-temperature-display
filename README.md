# avr-temperature-display

Bare-metal C for ATmega — reads ADC input from a temperature sensor and displays the value on a 7-segment display.

Written without Arduino libraries. Direct register manipulation using the AVR datasheet.

## What it does

- Initializes the ADC with AVCC (5V) as reference voltage and a prescaler of 128 (125kHz sample rate — within the ATmega's 50–200kHz recommended range)
- Reads a 10-bit ADC value from PA0
- Converts the raw reading to Celsius: `temperature = (ADC * 500) / 1024`
- Outputs the units digit to a common-anode 7-segment display on Port B every 500ms

## Hardware

- **MCU:** ATmega (tested at 16MHz)
- **Sensor:** Analog temperature sensor on PA0 (e.g. LM35)
- **Display:** Common-anode 7-segment on Port B

## Limitation

Only the units digit is displayed (e.g. 25°C shows as `5`). Showing both digits requires a multiplexed 2-digit display setup — two digit select pins toggled rapidly with separate Port B writes for tens and units. Not implemented here.

## Bug fix (v1.1)

Original code had `ADPS1` written twice instead of `ADPS1 | ADPS0`. This set prescaler to 64 (250kHz) — slightly outside the recommended ADC clock range. Fixed to correctly set all three prescaler bits for prescaler 128 (125kHz).

```c
// Before (wrong — prescaler 64, 250kHz)
ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS1);

// After (correct — prescaler 128, 125kHz)
ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
```

## Build

Compile with avr-gcc:

```bash
avr-gcc -mmcu=atmega32 -DF_CPU=16000000UL -Os -o avr-temp.elf avr-temperature-display.c
avr-objcopy -O ihex avr-temp.elf avr-temp.hex
```

Flash with avrdude:

```bash
avrdude -c usbasp -p m32 -U flash:w:avr-temp.hex
```

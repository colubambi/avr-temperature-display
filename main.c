#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// 7-segment display values (common anode)
// 0-9 patterns; 0 means segment ON for common anode
unsigned char SegCor[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

void adc_init() {
	// REFS0 = 1: Use AVCC (5V) as Vref
	// MUX0-4 = 0: Select ADC0 (PA0)
	ADMUX = (1 << REFS0);
	
	// ADEN = 1: Enable ADC
	// ADPS0-2 = 1: Prescaler 128 (16MHz/128 = 125KHz, within 50-200KHz range)
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS1);
}

uint16_t adc_read() {
	ADCSRA |= (1 << ADSC);          // Start conversion
	while (ADCSRA & (1 << ADSC));   // Wait for completion
	return ADC;                     // Return 10-bit result
}

int main(void) {
	DDRB = 0xFF; // Output for 7-segment
	adc_init();

	while (1) {
		uint16_t raw_adc = adc_read();
		
		// Calculation for 5V Vref: (ADC * 500) / 1024
		// We use long to prevent overflow during calculation
		uint8_t temperature = (uint8_t)((raw_adc * 500UL) / 1024UL);

		// Displaying the units digit (e.g., if 25°C, shows '5')
		// Note: To show both digits, you'd need a multiplexed display setup
		PORTB = SegCor[temperature % 10];

		_delay_ms(500);
	}
}
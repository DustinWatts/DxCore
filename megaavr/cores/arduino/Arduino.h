/*
  Arduino.h - Main include file for the Arduino SDK
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Arduino_h
#define Arduino_h

#include "api/ArduinoAPI.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#undef F
#define F(str) (str)

#ifdef __cplusplus
extern "C"{
#endif

/* Analog reference options */

/* Change in mega4809: two places to define analog reference
 - VREF peripheral defines internal reference
 - analog peripherals define internal/Vdd/external
*/

 // internal from VREF

 /* Values shifted to avoid clashing with ADC REFSEL defines
	Will shift back in analog_reference function
  */

#define INTERNAL1V024 VREF_REFSEL_1V024_gc
#define INTERNAL2V048 VREF_REFSEL_2V048_gc
#define INTERNAL4V096 VREF_REFSEL_4V096_gc
#define INTERNAL2V5 VREF_REFSEL_2V500_gc
#define DEFAULT     VREF_REFSEL_VDD_gc
#define VDD         VREF_REFSEL_VDD_gc
#define EXTERNAL    VREF_REFSEL_VREFA_gc


#define ADC_DAC0 ADC_MUXPOS_DAC0_gc
#define ADC_TEMPERATURE ADC_MUXPOS_TEMPSENSE_gc

#define VCC_5V0 2
#define VCC_3V3 1
#define VCC_1V8 0

#define interrupts() sei()
#define noInterrupts() cli()


// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
  #define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

/* Allows performing a correction on the CPU value using the signature row
	values indicating oscillator error provided from the device manufacturer */
#define PERFORM_SIGROW_CORRECTION_F_CPU 0

uint16_t clockCyclesPerMicrosecondComp(uint32_t clk);
uint16_t clockCyclesPerMicrosecond();
unsigned long clockCyclesToMicroseconds(unsigned long cycles);
unsigned long microsecondsToClockCycles(unsigned long microseconds);

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.

extern const uint8_t digital_pin_to_port[];
extern const uint8_t digital_pin_to_bit_mask[];
extern const uint8_t digital_pin_to_bit_position[];
extern const uint8_t digital_pin_to_timer[];

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
//
// These perform slightly better as macros compared to inline functions
//
#define NOT_A_PIN 255
#define NOT_A_PORT 255
#define NOT_AN_INTERRUPT 255

#define PA 0
#define PB 1
#define PC 2
#define PD 3
#define PE 4
#define PF 5
#define NUM_TOTAL_PORTS 6

// These are used for two things: identifying the timer on a pin
// and for the MILLIS_TIMER define that the uses can test which timer
// actually being used for millis is actually being used
// Reasoning these constants are what they are:
// Low 3 bits are the number of that peripheral
// other bits specify the type of timer
// TCA=0x08, TCB=0x10, TCD=0x10 (leaving room in case Microchip ever decides to release a TCC)
// Things that aren't hardware timers with output compare are after that
// DAC output isn't a timer, but has to be treated as such by PINMODE
// RTC timer is a tiner, but certainly not that kind of timer
#define NOT_ON_TIMER 0x00
#define TIMERA0 0x10
#define TIMERA1 0x11
#define TIMERB0 0x20
#define TIMERB1 0x21
#define TIMERB2 0x22
#define TIMERB3 0x23
#define TIMERB4 0x24
#define TIMERD0 0x40
#define TIMERRTC 0x90
#define DACOUT 0x80

void setup_timers();

#define digitalPinToPort(pin) ( (pin < NUM_TOTAL_PINS) ? digital_pin_to_port[pin] : NOT_A_PIN )
#define digitalPinToBitPosition(pin) ( (pin < NUM_TOTAL_PINS) ? digital_pin_to_bit_position[pin] : NOT_A_PIN )
#define analogPinToBitPosition(pin) ( (digitalPinToAnalogInput(pin)!=NOT_A_PIN) ? digital_pin_to_bit_position[pin] : NOT_A_PIN )
#define digitalPinToBitMask(pin) ( (pin < NUM_TOTAL_PINS) ? digital_pin_to_bit_mask[pin] : NOT_A_PIN )
#define analogPinToBitMask(pin) ( (digitalPinToAnalogInput(pin)!=NOT_A_PIN) ? digital_pin_to_bit_mask[pin] : NOT_A_PIN )
#define digitalPinToTimer(pin) ( (pin < NUM_TOTAL_PINS) ? digital_pin_to_timer[pin] : NOT_ON_TIMER )

#define portToPortStruct(port) ( (port < NUM_TOTAL_PORTS) ? ((PORT_t *)&PORTA + port) : NULL)
#define digitalPinToPortStruct(pin) ( (pin < NUM_TOTAL_PINS) ? ((PORT_t *)&PORTA + digitalPinToPort(pin)) : NULL)
#define getPINnCTRLregister(port, bit_pos) ( ((port != NULL) && (bit_pos < NOT_A_PIN)) ? ((volatile uint8_t *)&(port->PIN0CTRL) + bit_pos) : NULL )
#define digitalPinToInterrupt(P) (P)

#define portOutputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->OUT ) )
#define portInputRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->IN ) )
#define portModeRegister(P) ( (volatile uint8_t *)( &portToPortStruct(P)->DIR ) )

//#defines to identify part families
#if defined(__AVR_AVR128DA64__)||defined(__AVR_AVR64DA64__)
#define DA_64_PINS
#elif defined(__AVR_AVR128DA48__)||defined(__AVR_AVR64DA48__)||defined(__AVR_AVR32DA48__)
#define DA_48_PINS
#elif defined(__AVR_AVR128DA32__)||defined(__AVR_AVR64DA32__)||defined(__AVR_AVR32DA32__)
#define DA_32_PINS
#elif defined(__AVR_AVR128DA28__)||defined(__AVR_AVR64DA28__)||defined(__AVR_AVR32DA28__)
#define DA_28_PINS
#else
#error "Can't-happen: unknown chip somehow being used"
#endif

#define __AVR_DA__

#define DACORE "0.1.0"
#define DACORE_MAJOR 0
#define DACORE_MINOR 1
#define DACORE_PATCH 0
#define DACORE_RELEASED 0
#define DACORE_NUM 0x00010000



#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include "UART.h"

#endif

#include "pins_arduino.h"
#endif

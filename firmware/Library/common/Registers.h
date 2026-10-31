/*
 * Registers.h
 *
 *  Created on: Oct 21, 2023
 *      Author: ahmad
 */

#ifndef INC_REGISTERS_H_
#define INC_REGISTERS_H_

/******** DIO Registers ********/
#define PINA *((const volatile u8 *)0x39)
#define DDRA *((volatile u8 *)0x3A)
#define PORTA *((volatile u8 *)0x3B)
#define PINB *((const volatile u8 *)0x36)
#define DDRB *((volatile u8 *)0x37)
#define PORTB *((volatile u8 *)0x38)
#define PINC *((const volatile u8 *)0x33)
#define DDRC *((volatile u8 *)0x34)
#define PORTC *((volatile u8 *)0x35)
#define PIND *((const volatile u8 *)0x30)
#define DDRD *((volatile u8 *)0x31)
#define PORTD *((volatile u8 *)0x32)

/******** ExtInt Registers ********/
#define MCUCR *((volatile u8 *)0x55)
#define MCUCSR *((volatile u8 *)0x54)
#define GICR *((volatile u8 *)0x5B)
#define GIFR *((volatile u8 *)0x5A)

/******** SREG Registers ********/
#define SREG *((volatile u8 *)0x5F)

/******** ADC Registers ********/
#define ADC_DATA *((volatile u16 *)0x24)
#define ADCL *((volatile u8 *)0x24)
#define ADCH *((volatile u8 *)0x25)
#define ADCSRA *((volatile u8 *)0x26)
#define ADMUX *((volatile u8 *)0x27)
#define SFIOR *((volatile u8 *)0x50)
/******** UART Registers ********/
#define UDR *((volatile u8 *)0x2C)
#define UCSRA *((volatile u8 *)0x2B)
#define UCSRB *((volatile u8 *)0x2A)
#define UBRRL *((volatile u8 *)0x29)
#define UCSRC *((volatile u8 *)0x40)
#define UBRRH *((volatile u8 *)0x29)
/******** Internal EEPROM Registers ********/
#define EEARH *((volatile u8 *)0x3F)
#define EEARL *((volatile u8 *)0x3E)
#define EEDR *((volatile u8 *)0x3D)
#define EECR *((volatile u8 *)0x3C)




#endif /* INC_REGISTERS_H_ */

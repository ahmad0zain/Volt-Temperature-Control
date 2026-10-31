/*
 * Interrupts.h
 *
 *  Created on: Sep 23, 2023
 *      Author: ahmad
 */

#ifndef INC_INTERRUPTS_H_
#define INC_INTERRUPTS_H_

#define ISR(vector)                            \
    void vector(void) __attribute__((signal)); \
    void vector(void)

#define VECTOR_INT0 __vector_1
#define VECTOR_INT1 __vector_2
#define VECTOR_INT2 __vector_3
#define VECTOR_TIM2_COMP __vector_4
#define VECTOR_TIM2_OVF __vector_5
#define VECTOR_TIM1_CAPT __vector_6
#define VECTOR_TIM1_COMPA __vector_7
#define VECTOR_TIM1_COMPB __vector_8
#define VECTOR_TIM1_OVF __vector_9
#define VECTOR_TIM0_COMP __vector_10
#define VECTOR_TIM0_OVF __vector_11
#define VECTOR_ADC __vector_16
#define VECTOR_UART_RX __vector_13
#define VECTOR_UART_REG_EMP __vector_14
#define VECTOR_UART_TX __vector_15
#define VECTOR_SPI __vector_12

#endif /* INC_INTERRUPTS_H_ */

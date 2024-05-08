/*
 * user_ADC.h
 *
 *  Created on: Jan 21, 2024
 *      Author: Charles Lee Scoville
 */

#ifndef user_ADC_H_
#define user_ADC_H_

/*********************************************************************************
* Definitions
*********************************************************************************/
/* Default to channel 4 (Pin 20) */
#define ADC_Default_Channel ADC_Channel_4

/* 24Mhz (Div 2) is about as fast as the ADC module can be clocked. And only above 4.5V.*/
/* 12Mhz (Div 4) is as fast as the ADC module should be clocked at 3.3V > 4.5V. */
/* 6Mhz (Div 8) is as fast as the ADC module should be clocked at 2.8V > 3.3V. */
#define ADC_Divider RCC_PCLK2_Div4

/* Set the clocks per sample. Mostly this is the reciprocal of the above. */
#define ADC_SampleTime ADC_SampleTime_43Cycles

/*********************************************************************************
* Global variables
*********************************************************************************/
/* Raw ADC buffers for processing. */
extern uint16_t ADC_Channel_0_value;
extern uint16_t ADC_Channel_1_value;
extern uint16_t ADC_Channel_2_value;
extern uint16_t ADC_Channel_3_value;
extern uint16_t ADC_Channel_4_value;
extern uint16_t ADC_Channel_5_value;
extern uint16_t ADC_Channel_6_value;
extern uint16_t ADC_Channel_7_value;
extern uint16_t ADC_Channel_Vrefint_value;
extern uint16_t ADC_Channel_Vcalint_value;

/* Channel we are currently reading. */
extern uint8_t current_channel;

extern uint32_t RNG_GetCondensedEntropy(void);

/*********************************************************************************
* Declare exported/public functions
*********************************************************************************/
extern void ADC1_launch(void);
extern void ADC1_CFG(void);

/*********************************************************************************
* Declare interrupt callback functions
*********************************************************************************/
void ADC1_IRQHandler(void) __attribute__((interrupt));
extern void ADC1_IRQHandler();

#endif /* user_ADC_H */

// EOF

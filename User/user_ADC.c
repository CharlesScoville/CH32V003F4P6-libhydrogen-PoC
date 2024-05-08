/*
 * user_ADC.c
 *
 *  Created on: Jan 21, 2024
 *      Author: Charles Lee Scoville
 */

#include <ch32v00x.h>
//#include <stdbool.h>
#include "user_ADC.h"


/* Raw ADC buffers for processing.  */
uint16_t ADC_Channel_0_value = 0;
uint16_t ADC_Channel_1_value = 0;
uint16_t ADC_Channel_2_value = 0;
uint16_t ADC_Channel_3_value = 0;
uint16_t ADC_Channel_4_value = 0;
uint16_t ADC_Channel_5_value = 0;
uint16_t ADC_Channel_6_value = 0;
uint16_t ADC_Channel_7_value = 0;

uint16_t ADC_Channel_Vrefint_value = 0;
uint16_t ADC_Channel_Vcalint_value = 0;


/* Channel we are currently reading. Default to channel 4 (Pin 20) */
uint8_t current_channel = ADC_Default_Channel;

uint64_t entropy_buffer = 0;
uint8_t entropy_buffer_count = 0;

uint32_t extract = 0;
uint8_t extract_count = 0;

FlagStatus volatile extract_RDY = RESET;

uint32_t RNG_GetCondensedEntropy(void) {
	while (extract_RDY == RESET) {
		//sleep_delay_ms(1);
		__NOP();
	}

	extract_RDY = RESET;
	return extract;
}

/*********************************************************************
 * @fn 		ADC1_CFG
 *
 * @brief 	Configure the ADC1 module
 *
 * @accept	none
 *
 * @return	none
 *
 * Notes:
 * The settings were chosen for speed and maximal software control.
 * This has the cost of more consumption of energy and CPU time.
 *********************************************************************/
void ADC1_CFG(void) {
	/* Rout AHB bus clock to the ADC module. */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* Set the ADC clock divider */
	RCC_ADCCLKConfig(ADC_Divider);

	/* Wise to put the ADC in a known good state before messing with it. */
	ADC_DeInit(ADC1);

	/* Allocate memory for holding the ADC module's configuration. */
	ADC_InitTypeDef ADC_InitStructure = { 0 };

	/* Decide how we are going to use the ADC module. */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
	ADC_InitStructure.ADC_NbrOfChannel = 1;

	/* Commit to our configuration */
	ADC_Init(ADC1, &ADC_InitStructure);

	/* Start/Run the ADC module with the above configuration */
	ADC_Cmd(ADC1, ENABLE);

	/* Wait for the ADC to warm up for a bit */
	for(uint i=1;i<=19200;i++){ __NOP(); }; // ~19,200 ipms @ 48Mhz.

	/* Clear the old ADC calibration. */
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1))
		;

	/* Discover the fresh ADC calibration. */
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1))
		;

	/* According to the datasheet, we now have a value in RDATAR...
	  it is not clear if we are supposed to do something with it. */

	/* Wait for the ADC to settle into this new calibration */
	for(uint i=1;i<=19200;i++){ __NOP(); }; // ~19,200 ipms @ 48Mhz.

	/* Enable ADC1 to generate interrupt at End Of Conversion. */
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

/* Configure PFIC/NVIC to allow, in general ADC interrupt + handler vector. */
	NVIC_EnableIRQ(ADC_IRQn);
}

/*********************************************************************
 * @fn 	 	ADC1_launch
 *
 * @brief 	Starting the ADC module
 *
 * @accept	none
 *
 * @return	none
 *
 * Notes:
 * This assumes a default SampleTime that may not be ideal for your
 * application. Similarly, it starts off of the default start channel.
 *********************************************************************/
void ADC1_launch(void) {
	/* Configure an ADC channel */
	ADC_RegularChannelConfig(ADC1, current_channel, 1, ADC_SampleTime);

	/* Start the ADC module */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*********************************************************************
 * @fn 	 	ADC1_IRQHandler
 *
 * @brief 	Callback function for grabbing ADC value
 *
 * @accept	none
 *
 * @return	none
 *
 * Notes:
 * We really want to avoid calling functions from interrupt
 * routines as doing so incurs stack manipulation overhead.
 *********************************************************************/
void inline ADC1_IRQHandler(void) {
	uint32_t tmpreg1 = 0, tmpreg2 = 0;

	switch(current_channel) {

		case ADC_Channel_Vrefint:
			/* Dump ADC raw value to buffer */
			ADC_Channel_Vrefint_value = ADC1->RDATAR;

			/* Make room for incoming entropy bit by
			 * shifting the buffer over once */
			entropy_buffer = entropy_buffer << 1;

			/* Move lsb of ADC reading (bit 6 of RDATAR)
			 *  to the space we just made */
			entropy_buffer = entropy_buffer + ((ADC1->RDATAR & 0x40) == 0x40);

			/* When the raw entropy buffer gets full,
			 * we need to extract and whiten. */
			entropy_buffer_count++;
			if (entropy_buffer_count > 63) {
				entropy_buffer_count = 0;

				for (uint8_t j = 0; (j < 31); j++) {

					/* 64->32 bit Von Neuman extractor */
					if ((entropy_buffer & 1) ^ ((entropy_buffer >> 1) & 1)) {
						extract = (extract << 1)+((entropy_buffer >> 1) & 1);
						extract_count++;
					}

					entropy_buffer = entropy_buffer >> 2;

					/* When we have filled our extraction buffer, let libhydrogen know. */
					if (extract_count > 31) {
						extract_count = 0;
						extract_RDY = SET;

						/* Break from the surrounding for() loop,
						 * not this if() block. */
						break;
					}
				}
			}

			current_channel = ADC_Channel_3;
			/* Break from the switch case. */
			break;


		case ADC_Channel_3:
			/* Dump ADC raw value to buffer */
			ADC_Channel_3_value = ADC1->RDATAR;

			/* Change channel */
			current_channel = ADC_Channel_4;
			break;

		case ADC_Channel_4:
			/* Dump ADC raw value to buffer */
			ADC_Channel_4_value = ADC1->RDATAR;

			/* Change channel */
			current_channel = ADC_Channel_Vrefint;
			break;


		default:
			/* Change channel */
			current_channel = ADC_Channel_Vrefint;
			break;
	}


	/* Reset ADC for next conversion. */
	/* TODO This is ugly and wrong and should be cleaned up */
	tmpreg1 = ADC1->SAMPTR2;
	tmpreg2 = (uint32_t)0x00000007 << (3 * current_channel);
	tmpreg1 &= ~tmpreg2;
	tmpreg2 = (uint32_t)ADC_SampleTime << (3 * current_channel);
	tmpreg1 |= tmpreg2;
	ADC1->SAMPTR2 = tmpreg1;

	tmpreg1 = ADC1->RSQR3;
	tmpreg2 = (uint32_t)0x0000001F;
	tmpreg1 &= ~tmpreg2;
	tmpreg2 = (uint32_t)current_channel;
	tmpreg1 |= tmpreg2;
	ADC1->RSQR3 = tmpreg1;

	ADC1->CTLR2 |= (uint32_t)0x00500000;
}

// EOF

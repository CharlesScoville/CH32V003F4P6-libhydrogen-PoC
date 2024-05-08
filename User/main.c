/*
 *** Libhydrogen on WCH32V003 ***

 This project demonstrates libhydrogen functionality on WCH's CH32V003. It uses
 the least significant bits of the ADC pointed at the internal voltage reference
 as the entropy source. You'll need the official WCH toolchain and a dev board to
 use this as is/turn key. The chip on a breakout PCB + WCH link-e is sufficient.

 A few key modifications were made to the WCH supplied framework/stack/template,
 mostly related to interrupts, definitions, and helper debug functions. Similarly,
 the IDE project settings needed some tweaking to get the compiler to include the
 library, and to see the part selection define flag. These were, unfortunately,
 scattered all over the place, so I have no concise way to summarize them.

 TL;DR: It's in your best interest to do a direct copy/paste of the whole project.
*/

#include <ch32v00x.h>
#include "debug.h"
#include "hydrogen.h"

#include "user_ADC.h"

/*********************************************************************************
* Global defines
**********************************************************************************/
#define ever ;;

/*********************************************************************************
* Global variables
**********************************************************************************/
/* System up-time. *NOTE* This can *ONLY* hold about 49 days worth of milliseconds. */
uint32_t uptime_ms = 0;

/*********************************************************************************
* Function definitions
*********************************************************************************/

/* Delay function that utilizes systimer rather than just burning a delay loop */

/* TODO: In theory, one would execute WFE() to put the chip into a low power state.
 * In practice, it is not possible to enter any kind of Sleep mode when the
 * microprocessor is in Debug mode. So it's difficult to know if this works. */

void inline static sleep_delay_ms(uint32_t ms) {
	for(uint32_t delta = uptime_ms + ms; delta > uptime_ms; ) {
			__NOP();
			//__WFE();
	}
}

/* Helper function for printing byte arrays in hex */
void printHex(uint8_t * array, size_t bytes) {
	printf("\n\r0x\033[38;5;136m%02X", array[0]);

	for(size_t i = 1; i < (bytes - 1); i++) {
		printf("%02X", array[i]);
	}

	printf("\033[0m\n\r");
}


/* Basic setup for making the chip work as expected */
void SYSTEM_CFG(void) {
	/* Configure main system clock source */
	SystemCoreClockUpdate();

	/* Enable low speed internal clock. */
	RCC_LSICmd(ENABLE);

	/* This enables sleep mode via PWR module. */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Configure PLL (clock multiplier) High Speed Internal (48MHz)  */
	RCC_PLLConfig(RCC_PLLSource_HSI_MUL2);

	/* Configure system timer. */
	SysTimer();

	/* Configure SysTick to allow 1ms interrupt + handler vector. */
	NVIC_EnableIRQ(SysTicK_IRQn);

	/* Configure USART as sink for printf() */
	USART_Printf_Init(576000);

	printf("\033[2J"); 		/* Clear the terminal buffer. */
	printf("\033[H");		/* Home the cursor.           */
	printf("\033[?25l");	/* Make the cursor invisible. */

	printf("Part: \033[38;5;14mWCH CH32V003\033[0m\r\n", SystemCoreClock);

	printf("SystemClk: \033[38;5;14m%luHz\033[0m\r\n", SystemCoreClock);

	printf("FLASH size: \033[38;5;14m%uKB\033[0m\r\n", *((uint16_t*) 0x1FFFF7E0));

	printf("MISA Register: \033[38;5;14m%lx\033[0m\r\n\r\n", __get_MISA());

	/*  Configure interrupt controller hardware priority groups  */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/*********************************************************************/
int main(void)
{

	/* Bring up essential system things (bootstrap the SoC) */
	SYSTEM_CFG();

	/* Set up the ADC (need this for entropy source) */
	ADC1_CFG();

	/* Start the ADC */
	ADC1_launch();

	/* Boot up libhydrogen (be sure to define "CH32V003 in pre-processor settings!) */
    hydro_init();

    for(ever)
    {

    	//***************************** LibHydrogen *****************************
    	#define CONTEXT "Example"
    	#define MESSAGE "libhydrogen! Working on CH32V003!"
    	#define MESSAGE_LEN sizeof(MESSAGE)

    	printf("Creating key pair. . .  \n\r");
    	uint32_t start_time = uptime_ms;
    	hydro_sign_keypair key_pair;
    	hydro_sign_keygen(&key_pair);
    	printf("Key pair created! (Took %lu)\n\r", (uptime_ms - start_time));

    	uint8_t signature[hydro_sign_BYTES];

    	printf("Signing the following message with secret key. . .  \n\r");
    	printf("\"\033[38;5;166m%s\033[0m\"\n\r", MESSAGE);
    	start_time = uptime_ms;
    	hydro_sign_create(signature, MESSAGE, MESSAGE_LEN, CONTEXT, key_pair.sk);
    	printf("Signed! (Took %lums)\n\r", (uptime_ms - start_time));

    	/* Here we can modify the signature so as to show it will fail verification. */
    	//signature[0] = signature[0] ^ 0x01;

    	printf("Now verifying signature using the public key. . . \n\r");
    	start_time = uptime_ms;
    	if (hydro_sign_verify(signature, MESSAGE, MESSAGE_LEN, CONTEXT, key_pair.pk) != 0) {
    		uint32_t verifytime = (uptime_ms - start_time);
    		printf("\033[38;5;9mSignature does not sign this message!\033[0m\n\r");
    		printf("Signature was:");
    		printHex(signature, hydro_sign_BYTES);
    		printf("Verification took m%lums\n\r", verifytime);
    	} else {
    		uint32_t verifytime = (uptime_ms - start_time);
    		printf("\033[38;5;10mMessage confirmed authentic via signature!\033[0m\n\r");
    		printf("Public key was :");
    		printHex(key_pair.pk, sizeof(key_pair.pk));
    		printf("Secret key was :");
    		printHex(key_pair.sk, sizeof(key_pair.sk));
    		printf("Signature was:");
    		printHex(signature, hydro_sign_BYTES);
    		printf("Verification Took %lums\n\r", verifytime);
    	}
    	//*************************** END LibHydrogen ***************************/

    	sleep_delay_ms(10000);

    }
}

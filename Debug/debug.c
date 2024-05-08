/********************************** (C) COPYRIGHT  *******************************
 * File Name          : debug.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/08
 * Description        : This file contains all the functions prototypes for UART
 *                      Printf , Delay functions.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include <debug.h>

static uint8_t  p_us = 0;
static uint16_t p_ms = 0;

/*********************************************************************
 * @fn      Delay_Init
 *
 * @brief   Initializes Delay Function.
 *
 * @return  none
 */
void Delay_Init(void)
{
    p_us = SystemCoreClock / 8000000;
    p_ms = (uint16_t)p_us * 1000;
}

void SysTimer(void)
{
	Delay_Init();

    // Clear comparison IT flag.
    SysTick->SR &= ~STK_SR_CNTIF;

    // Move delta equal to 1ms into comparison register
    SysTick->CMP = p_ms;

    // Clear the counter register
    SysTick->CNT = 0;

    // SysTick hardware clears CNTR.
    SysTick->CTLR |= STK_CTLR_STRE;

    // SysTick generates interrupt.
    SysTick->CTLR |= STK_CTLR_STIE;

    // Turn on SysTick module
    SysTick->CTLR |= STK_CTLR_STE;
}

/*********************************************************************
 * @fn      Delay_Us
 *
 * @brief   Microsecond Delay Time.
 *
 * @param   n - Microsecond number.
 *
 * @return  None
 */
void Delay_Us(uint32_t n)
{
	// Allocate a temp register
    uint32_t i;

    // Clear comparison IT flag.
    SysTick->SR &= ~STK_SR_CNTIF;

    // calculate delta, equal to counts per us
    i = (uint32_t)n * p_us;

    // Move delta into comparison register
    SysTick->CMP = i;

    // Clear the counter register
    SysTick->CNT = 0;

    // Start the system counter SysTick.
    SysTick->CTLR |= STK_CTLR_STE;

    // Wait HERE for timer to count down.
    while((SysTick->SR & STK_SR_CNTIF) != SET);

    // Time elapsed, clear the comparison IT flag.
    SysTick->CTLR &= ~STK_SR_CNTIF;
}

/*********************************************************************
 * @fn      Delay_Ms
 *
 * @brief   Millisecond Delay Time.
 *
 * @param   n - Millisecond number.
 *
 * @return  None
 */
void Delay_Ms(uint32_t n)
{
	// Allocate a temp register
    uint32_t i;

    // Clear comparison IT flag.
    SysTick->SR &= ~STK_SR_CNTIF;

    // Calculate delta, equal to counts per ms
    i = (uint32_t)n * p_ms;

    // Move delta into comparison register
    SysTick->CMP = i;

    // Clear the counter register
    SysTick->CNT = 0;

    // Start the system counter SysTick.
    SysTick->CTLR |= STK_CTLR_STE;

    // Wait HERE for timer to count down.
    while((SysTick->SR & STK_SR_CNTIF) != SET);

    // Time elapsed, clear the comparison IT flag.
    SysTick->CTLR &= ~STK_CTLR_STE;
}

/*********************************************************************
 * @fn      USART_Printf_Init
 *
 * @brief   Initializes the USARTx peripheral.
 *
 * @param   baudrate - USART communication baud rate.
 *
 * @return  None
 */
void USART_Printf_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

/*********************************************************************
 * @fn      _write
 *
 * @brief   Support Printf Function
 *
 * @param   *buf - UART send Data.
 *          size - Data length.
 *
 * @return  size - Data length
 */
__attribute__((used)) 
int _write(int fd, char *buf, int size)
{
    int i;

    for(i = 0; i < size; i++){
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, *buf++);
    }

    return size;
}

/*********************************************************************
 * @fn      _sbrk
 *
 * @brief   Change the spatial position of data segment.
 *
 * @return  size: Data length
 */
void *_sbrk(ptrdiff_t incr)
{
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return NULL - 1;

    curbrk += incr;
    return curbrk - incr;
}




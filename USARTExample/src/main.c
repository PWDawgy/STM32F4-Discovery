/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed "as is", without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
//#include "stm32f4_discovery.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */


void GPIOD_Init(void);

void USART2_Init(void);
void led_toggle(void);


/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
    /* Initialize GPIOD PIN13 */
    GPIOD_Init();
    /* Initialize USART2 */
    USART2_Init();

    while(1)
    {
        /* Do nothing, all happens in ISR */
    }

}

/***********************************************
 * Initialize GPIOD PIN8 as push-pull output
 ***********************************************/
void GPIOD_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable the GPIO_LED Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_SetBits(GPIOD, GPIO_Pin_13);
}
/*****************************************************
 * Initialize USART2: enable interrupt on reception
 * of a character
 *****************************************************/
void USART2_Init(void)
{
    /* USART configuration structure for USART2 */
    USART_InitTypeDef usart2_init_struct;
    /* Bit configuration structure for GPIOA PIN2 and PIN3 */
    GPIO_InitTypeDef gpioa_init_struct;

    /* Enable Port A peripheral clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* Enable USART2 peripheral clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	/* Configure alternate function for Port A pins 2 & 3 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);


    /* Configure the alternate functionality for pins 2(USART2_TX) and 3 (USART2_RX) of port A. */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF;
    gpioa_init_struct.GPIO_OType = GPIO_OType_PP;
    gpioa_init_struct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpioa_init_struct);

    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    usart2_init_struct.USART_BaudRate = 115200;
    usart2_init_struct.USART_WordLength = USART_WordLength_8b;
    usart2_init_struct.USART_StopBits = USART_StopBits_1;
    usart2_init_struct.USART_Parity = USART_Parity_No ;
    usart2_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart2_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART2, &usart2_init_struct);
    /* Enable RXNE interrupt */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    /* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART2_IRQn);
 }
/*******************************************
 * Toggle LED
 *******************************************/
void led_toggle(void)
{
    /* Read LED output (GPIOD PIN13) status */
    uint8_t led_bit = GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_13);

    /* If LED output set, clear it */
    if(led_bit == (uint8_t)Bit_SET)
    {
        GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    }
    /* If LED output clear, set it */
    else
    {
        GPIO_SetBits(GPIOD, GPIO_Pin_13);
    }
}

/**********************************************************
 * USART2 interrupt request handler: on reception of a
 * character 't', toggle LED and transmit a character 'T'
 *********************************************************/
void USART2_IRQHandler(void)
{
    /* RXNE handler */
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        /* If received 't', toggle LED and transmit 'T' */
        if((char)USART_ReceiveData(USART2) == 't')
        {
            led_toggle();
            USART_SendData(USART2, 'T');
            /* Wait until Tx data register is empty, not really
             * required for this example but put in here anyway.
             */

        }
    }
}


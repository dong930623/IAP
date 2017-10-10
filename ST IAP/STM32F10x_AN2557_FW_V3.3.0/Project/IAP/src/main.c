/**
  ******************************************************************************
  * @file    IAP/src/main.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/** @addtogroup IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;

/* Private function prototypes -----------------------------------------------*/
static void IAP_Init(void);

/* Private functions ---------------------------------------------------------*/
/**
  等待进入烧写状态
  当程序收到$1234的时候进入烧写
  否则程序一直终止在这里等待接收完毕
  */
void waitIAP(){
   	/* uint8_t IAP[5],begin = 0,Res,count;
	 while(1){
	 	if(Receive_Byte(&Res,1000) == 0){
			 if(Res == '$'){	 //接收到$开始接收下一个字符
			 	 begin = 1;
				 count = 0;
				 IAP[0] = Res;
				 count ++;
			 }else if(begin == 1){
			 	 IAP[count] = Res;
				 count++;
				 if(count == 5){
				 	  if((IAP[0] == '$') && (IAP[1] == '1') && (IAP[2] == '2')
					     && (IAP[3] == '3') && (IAP[4] == '4')){//接收收到$1234开始进入正式烧写模式
						 	break; 
						 }else{
						  begin = 0;
						  count = 0;
						 }
				 }
			 }
		} 
	 }	*/
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	uint8_t key = 0;
	/* Flash unlock */
	FLASH_Unlock();	
	/* Initialize Key Button mounted on STM3210X-EVAL board */       
	//  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);  
	IAP_Init();
	/* Test if Key push-button on STM3210X-EVAL Board is pressed */
	Flash_Read(IsDownLoadFlagAddress, &key, 1);
	//  if (STM_EVAL_PBGetState(BUTTON_KEY)  == 0x00) //PC1
	while (1)
	{
		if( key == 1)
		{ 
			/* If Key is pressed */
			/* Execute the IAP driver in order to re-program the Flash */
			//waitIAP();
			key = 0;
			FLASH_Unlock();
			Flash_Write(IsDownLoadFlagAddress, &key,1);
			FLASH_Unlock();
			Main_Menu();
			/* Test if user code is programmed starting from address "ApplicationAddress" */
			if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
			{ 
				/* Jump to user application */
				JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
				Jump_To_Application = (pFunction) JumpAddress;
				/* Initialize user application's Stack Pointer */
				__set_MSP(*(__IO uint32_t*) ApplicationAddress);
				Jump_To_Application();
			}

		}
		/* Keep the user application running */
		else
		{ 
			/* Test if user code is programmed starting from address "ApplicationAddress" */
			if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
			{ 
				/* Jump to user application */
				JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
				Jump_To_Application = (pFunction) JumpAddress;
				/* Initialize user application's Stack Pointer */
				__set_MSP(*(__IO uint32_t*) ApplicationAddress);
				Jump_To_Application();
			}
			else
			{
				key = 1;
				Flash_Write(IsDownLoadFlagAddress, &key, 1);
			}
		}
	}
}

/**
* @brief  Initialize the IAP: Configure RCC, USART and GPIOs.
* @param  None
* @retval None
*/
void IAP_Init(void)
{
	USART_InitTypeDef USART_InitStructure;

	/* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
	/* USART configured as follow:
	- BaudRate = 115200 baud  
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	STM_EVAL_COMInit(COM1, &USART_InitStructure);  
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

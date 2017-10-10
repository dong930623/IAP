/**
  ******************************************************************************
  * @file    IAP/src/upload.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   This file provides the software which allows to upload an image 
  *          from internal Flash.
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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void SerialUpload(void)
{
  uint32_t status = 0; 

  SerialPutString("\n\n\rSelect Receive File ... (press any key to abort)\n\r");

  if (GetKey() == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    status = Ymodem_Transmit((uint8_t*)ApplicationAddress, (const uint8_t*)"UploadedFlashImage.bin", FLASH_IMAGE_SIZE);

    if (status != 0) 
    {
      SerialPutString("\n\rError Occured while Transmitting File\n\r");
    }
    else
    {
      SerialPutString("\n\rFile Trasmitted Successfully \n\r");
    }
  }
  else
  {
    SerialPutString("\r\n\nAborted by user.\n\r");  
  }
 
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2010 STMicroelectronics *****END OF FILE******/

/**
  * 往某个地址区域写入数据
  * iAddress 起始地址 buf 数组指针 iNbrToRead 需要写入数组的长度大小  
  */
uint16_t Flash_Write_Without_check(uint32_t iAddress, uint8_t *buf, uint16_t iNumByteToWrite) {
    uint16_t i;
    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
    i = 0;
    
//    FLASH_UnlockBank1();
    while((i < iNumByteToWrite) && (FLASHStatus == FLASH_COMPLETE))
    {
      FLASHStatus = FLASH_ProgramHalfWord(iAddress, *(uint16_t*)buf);
      i = i+2;
      iAddress = iAddress + 2;
      buf = buf + 2;
    }
    
    return iNumByteToWrite;
}
/**
  * 往某个地址区域写入数据
  * iAddress 起始地址 buf 数组指针 iNbrToRead 需要写入数组的长度大小  
  */
int Flash_Write(uint32_t iAddress, uint8_t *buf, uint32_t iNbrToWrite) {
                /* Unlock the Flash Bank1 Program Erase controller */
	    volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
        uint32_t secpos;
        uint32_t iNumByteToWrite = iNbrToWrite;
		uint16_t secoff;
		uint16_t secremain;  
  		uint16_t i = 0;    
        uint8_t tmp[PAGE_SIZE];
        
        FLASH_UnlockBank1();
		secpos=iAddress & (~(PAGE_SIZE -1 )) ;//扇区地址 
		secoff=iAddress & (PAGE_SIZE -1);     //在扇区内的偏移
		secremain=PAGE_SIZE-secoff;           //扇区剩余空间大小 
        
        
        if(iNumByteToWrite<=secremain) secremain = iNumByteToWrite;//不大于4096个字节
        
        while( 1 ) {
            Flash_Read(secpos, tmp, PAGE_SIZE);   //读出整个扇区
            for(i=0;i<secremain;i++) {       //校验数据
			       if(tmp[secoff+i]!=0XFF)break;       //需要擦除 
			   }
            if(i<secremain) {  //需要擦除
                FLASHStatus = FLASH_ErasePage(secpos); //擦除这个扇区
                if(FLASHStatus != FLASH_COMPLETE)
                  return -1;
                for(i=0;i<secremain;i++) {   //复制
                        tmp[i+secoff]=buf[i];   
                }
                Flash_Write_Without_check(secpos ,tmp ,PAGE_SIZE);//写入整个扇区  
            } else {
                Flash_Write_Without_check(iAddress,buf,secremain);//写已经擦除了的,直接写入扇区剩余区间.
            }
            
            if(iNumByteToWrite==secremain) //写入结束了
                break;
            else {
                secpos += PAGE_SIZE;
                secoff = 0;//偏移位置为0 
                buf += secremain;  //指针偏移
                iAddress += secremain;//写地址偏移    
                iNumByteToWrite -= secremain;  //字节数递减
                if(iNumByteToWrite>PAGE_SIZE) secremain=PAGE_SIZE;//下一个扇区还是写不完
                else secremain = iNumByteToWrite;  //下一个扇区可以写完了
            }
            
         }
        
        FLASH_LockBank1();
        return iNbrToWrite; 
}

/**
  * 从某个地址区域读出数据
  * iAddress 起始地址 buf 存放的数组指针 iNbrToRead 需要读出的长度大小  
  */
int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead) {
	        int i = 0;
	        while(i < iNbrToRead ) {
	           *(buf + i) = *(__IO uint8_t*) iAddress++;
	           i++;
	        }
	        return i;
}

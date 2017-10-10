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
  * ��ĳ����ַ����д������
  * iAddress ��ʼ��ַ buf ����ָ�� iNbrToRead ��Ҫд������ĳ��ȴ�С  
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
  * ��ĳ����ַ����д������
  * iAddress ��ʼ��ַ buf ����ָ�� iNbrToRead ��Ҫд������ĳ��ȴ�С  
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
		secpos=iAddress & (~(PAGE_SIZE -1 )) ;//������ַ 
		secoff=iAddress & (PAGE_SIZE -1);     //�������ڵ�ƫ��
		secremain=PAGE_SIZE-secoff;           //����ʣ��ռ��С 
        
        
        if(iNumByteToWrite<=secremain) secremain = iNumByteToWrite;//������4096���ֽ�
        
        while( 1 ) {
            Flash_Read(secpos, tmp, PAGE_SIZE);   //������������
            for(i=0;i<secremain;i++) {       //У������
			       if(tmp[secoff+i]!=0XFF)break;       //��Ҫ���� 
			   }
            if(i<secremain) {  //��Ҫ����
                FLASHStatus = FLASH_ErasePage(secpos); //�����������
                if(FLASHStatus != FLASH_COMPLETE)
                  return -1;
                for(i=0;i<secremain;i++) {   //����
                        tmp[i+secoff]=buf[i];   
                }
                Flash_Write_Without_check(secpos ,tmp ,PAGE_SIZE);//д����������  
            } else {
                Flash_Write_Without_check(iAddress,buf,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
            }
            
            if(iNumByteToWrite==secremain) //д�������
                break;
            else {
                secpos += PAGE_SIZE;
                secoff = 0;//ƫ��λ��Ϊ0 
                buf += secremain;  //ָ��ƫ��
                iAddress += secremain;//д��ַƫ��    
                iNumByteToWrite -= secremain;  //�ֽ����ݼ�
                if(iNumByteToWrite>PAGE_SIZE) secremain=PAGE_SIZE;//��һ����������д����
                else secremain = iNumByteToWrite;  //��һ����������д����
            }
            
         }
        
        FLASH_LockBank1();
        return iNbrToWrite; 
}

/**
  * ��ĳ����ַ�����������
  * iAddress ��ʼ��ַ buf ��ŵ�����ָ�� iNbrToRead ��Ҫ�����ĳ��ȴ�С  
  */
int Flash_Read(uint32_t iAddress, uint8_t *buf, int32_t iNbrToRead) {
	        int i = 0;
	        while(i < iNbrToRead ) {
	           *(buf + i) = *(__IO uint8_t*) iAddress++;
	           i++;
	        }
	        return i;
}

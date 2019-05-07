/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   SDIO sd�����������������ļ�ϵͳ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 H743 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "sdmmc/sdmmc_test.h"
#include "led/bsp_led.h"
#include "sdmmc/bsp_sdmmc_sd.h"
#include "./usart/bsp_debug_usart.h"

/******** SD���ͻ��������� *******/
__attribute__((section (".RAM_D1"))) uint8_t aTxBuffer[BUFFER_WORD_SIZE*4];

/******** SD���ջ��������� *******/
__attribute__((section (".RAM_D1"))) uint8_t aRxBuffer[BUFFER_WORD_SIZE*4];

__IO uint8_t RxCplt, TxCplt;

__IO uint8_t step = 0;

/**
  * @brief  ��������ʱִ�иú���
  * @param  ��
  * @retval ��
  */
static void Error_Handler(void)
{
  printf(" - Error \n");
	LED_RGBOFF;
  while(1)
  {
    /* �д����ɫ����˸ */
    LED1_TOGGLE;
		HAL_Delay(300);
  }
}
/**
  * @brief  �ȴ�SD���������״̬
  * @param  ��
  * @retval ��
  */
uint8_t Wait_SDCARD_Ready(void)
{
  uint32_t loop = SD_TIMEOUT;
  
  /* �ȴ������������ */
  /* ȷ��SD����������׼����ʹ�� */
  while(loop > 0)
  {
    loop--;
    if(HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER)
    {
        return HAL_OK;
    }
  }
  return HAL_ERROR;
}

/**
  * @brief ���մ�������ɻص�����
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  if(Wait_SDCARD_Ready() != HAL_OK)
  {
    Error_Handler();
  }
  SCB_InvalidateDCache_by_Addr((uint32_t*)aRxBuffer, BUFFER_WORD_SIZE*4);
  RxCplt=1;
}

/**
* @brief ���ʹ�������ɻص�����
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  if(Wait_SDCARD_Ready() != HAL_OK)
  {
    Error_Handler();
  }
  TxCplt=1;
}

/**
  * @brief SD����ص�����
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
  Error_Handler();
}
/**
  * @brief  ����MPU����
  * @note   �������洢���ӿ���AXI����ô����ַ��0x30040000
  *         ���������СΪ32KB����Ϊ��D2SRAM3��С��ͬ
  * @note   SDIO�ڲ�DMA�ɷ��ʵ�SRAM1������ַΪ0x24000000��
            ���������СΪ512KB
  * @param  ��
  * @retval ��
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  
  /* ����MPU */
  HAL_MPU_Disable();

  /* ��MPU��������ΪSRAM1��WT */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* ʹ��MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
  
}

void SD_Test(void)
{
	uint32_t index = 0;
	static uint32_t start_time = 0;
	static uint32_t stop_time = 0;
	
	MPU_Config();
	/*------------------------------ SD ��ʼ�� ------------------------------ */
	/* SD��ʹ��SDIO�жϼ�DMA�жϽ������ݣ��жϷ������λ��bsp_sdio_sd.c�ļ�β*/
	if(BSP_SD_Init() != HAL_OK)
	{    
		LED_RED;
		printf("SD����ʼ��ʧ�ܣ���ȷ��SD������ȷ���뿪���壬��һ��SD�����ԣ�\n");
	}
	else
	{
		if(BSP_SD_Erase(ADDRESS, ADDRESS+BUFFERSIZE) != HAL_OK)
		{
			Error_Handler();
		}			
		while(1)
		{
			switch(step)
			{
				case 0:
				{
					/*��ʼ�����ͻ�����*/
					for (index = 0; index < BUFFERSIZE; index++)
					{
						aTxBuffer[index] = DATA_PATTERN + index;
					}
					/*���Dcache������*/
					SCB_CleanDCache_by_Addr((uint32_t*)aTxBuffer, BUFFER_WORD_SIZE*4);
					printf(" ****************** ��ʼд����� ******************* \n");
					printf(" - д�뻺������С�� %lu MB   \n", (long)(DATA_SIZE>>20));
					index = 0;
					start_time = HAL_GetTick();
					step++;
				}
				break;
				case 1:
				{
					TxCplt = 0;
					/*��ʼ�������ݣ�DMA��ʽ*/
					if(BSP_SD_WriteBlocks_DMA(aTxBuffer, ADDRESS, NB_BLOCK_BUFFER) != HAL_OK)
					{
						Error_Handler();
					}
					step++;
				}
				break;
				case 2:
				{
					if(TxCplt != 0)
					{
						/* �������������˸һ�� */
						LED3_TOGGLE;
						
						/* ������������� */
						index++;
						if(index<NB_BUFFER)
						{
							/* ��������û������� */
							step--;
						}
						else
						{
							stop_time = HAL_GetTick();
							printf(" - д������ʱ��(ms): %lu  -  д���ٶ�: %02.2f MB/s  \n",(long)(stop_time - start_time),\
																							(float)((float)(DATA_SIZE>>10)/(float)(stop_time - start_time)));
							/* �������ݶ������� */
							step++;
						}
					}
				}
				break;
				case 3:
				{
					/*��ʼ�����ջ�����*/
					for (index = 0; index < BUFFERSIZE; index++)
					{
						aRxBuffer[index] = 0;
					}
					/*���Dcache������*/
					SCB_CleanDCache_by_Addr((uint32_t*)aRxBuffer, BUFFER_WORD_SIZE*4);
					printf(" ******************* ��ʼ��ȡ���� ******************* \n");
					printf(" - ��ȡ��������С: %lu MB   \n", (long)(DATA_SIZE>>20));
					start_time = HAL_GetTick();
					index = 0;
					step++;
				}
				break;
				case 4:
				{
					/*��ʼ�������ݣ�DMA��ʽ*/
					RxCplt = 0;
					if(BSP_SD_ReadBlocks_DMA(aRxBuffer, ADDRESS, NB_BLOCK_BUFFER) != HAL_OK)
					{
						Error_Handler();
					}
					step++;
				}
				break;
				case 5:
				{
					if(RxCplt != 0)
					{
						/* �������������˸һ�� */
						LED3_TOGGLE;
						/* ������������� */
						index++;
						if(index<NB_BUFFER)
						{
							 /* ��������û������� */
							step--;
						}
						else
						{
							stop_time = HAL_GetTick();
							printf(" ��ȡ����ʱ��(ms): %lu  -  ��ȡ�ٶ�: %02.2f MB/s  \n", (long)(stop_time - start_time),\
																						(float)((float)(DATA_SIZE>>10)/(float)(stop_time - start_time)));
							/* �������ݶ������� */
							step++;
						}
					}
				}
				break;
				case 6:
				{
					/*�����ջ���������*/
					index=0;
					printf(" ********************* У������ ********************** \n");
					while((index<BUFFERSIZE) && (aRxBuffer[index] == aTxBuffer[index]))
					{
						index++;
					}
					
					if(index != BUFFERSIZE)
					{
						printf(" У�����ݳ��� !!!!   \n");
						Error_Handler();
					}
					printf(" У��������ȷ  \n");
					step++;
				}
				break;			
				default :
					Error_Handler();
			}
			/*��д�������*/
			if(step == 7)
			{
				LED_GREEN;
				printf(" SD��д������� \n");
				break;
			}
		}
	}

}

/*********************************************END OF FILE**********************/

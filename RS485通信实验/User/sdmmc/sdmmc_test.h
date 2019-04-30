#ifndef __SDIO_TEST_H
#define __SDIO_TEST_H

#include "stm32h7xx.h"

extern __IO uint8_t step;
	
/* ���ݴ�С10Mb */
#define DATA_SIZE              ((uint32_t)0x00A00000U) 
/* ��������С��256Kb */
#define BUFFER_SIZE            ((uint32_t)0x00040000U)
/* ���������� */
#define NB_BUFFER              DATA_SIZE / BUFFER_SIZE
/* ÿ������������������ */
#define NB_BLOCK_BUFFER        BUFFER_SIZE / BLOCKSIZE 
/* ��������С��˫�ֽڣ� */
#define BUFFER_WORD_SIZE       (BUFFER_SIZE>>2)        

#define SD_TIMEOUT             ((uint32_t)0x00100000U)
/* SD��д�ĵ�ַ */
#define ADDRESS                ((uint32_t)0x00000400U)
/* ��������� */
#define DATA_PATTERN           ((uint32_t)0xB5F3A5F3U) 

#define COUNTOF(__BUFFER__)    (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#define BUFFERSIZE             (COUNTOF(aTxBuffer) - 1)

void SD_Test(void);

#endif
/*****************************END OF FILE**************************/

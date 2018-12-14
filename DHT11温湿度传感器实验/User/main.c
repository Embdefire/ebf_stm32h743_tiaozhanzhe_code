/**
  ******************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   GPIO���--ʹ�ù̼������LED��
  ******************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32H743������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************
  */  
#include "stm32h7xx.h"
#include "main.h"
#include "systick/bsp_SysTick.h"
#include "./usart/bsp_usart.h"
#include "DHT11/bsp_dht11.h"
#include "./lcd/bsp_lcd.h"
#include "./delay/core_delay.h" 
DHT11_Data_TypeDef DHT11_Data;
char dispBuff[100];
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	/* ϵͳʱ�ӳ�ʼ����400MHz */
	SystemClock_Config();
  
  
  
  /*��ʼ��USART������ģʽΪ 115200 8-N-1*/
  UARTx_Config();
	
	/* LCD �˿ڳ�ʼ�� */ 
	LCD_Init();
	/* LCD ��һ���ʼ�� */ 
	LCD_LayerInit(0, LCD_FB_START_ADDRESS,RGB565);
	/* LCD �ڶ����ʼ�� */ 
	LCD_LayerInit(1, LCD_FB_START_ADDRESS+(LCD_GetXSize()*LCD_GetYSize()*4),RGB565);
	/* ʹ��LCD������������ */ 
	LCD_DisplayOn(); 

	/* ѡ��LCD��һ�� */
	LCD_SelectLayer(0);

	/* ��һ����������ʾȫ�� */ 
	LCD_Clear(LCD_COLOR_BLACK);  

	/* ѡ��LCD�ڶ��� */
	LCD_SelectLayer(1);

	/* �ڶ�����������ʾȫ�� */ 
	LCD_Clear(LCD_COLOR_TRANSPARENT);

	/* ���õ�һ�͵ڶ����͸����,��СֵΪ0�����ֵΪ255*/
	LCD_SetTransparency(0, 255);
	LCD_SetTransparency(1, 0);

	/* ѡ��LCD��һ�� */
	LCD_SelectLayer(0);

	/* ��������ʾȫ�� */
	LCD_Clear(LCD_COLOR_BLACK);	
	/*����������ɫ������ı�����ɫ(�˴��ı�������ָLCD�ı����㣡ע������)*/
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	/*ѡ������*/
	LCD_SetFont(&LCD_DEFAULT_FONT);
	/* DHT11��ʼ�� */
  SysTick_Init();
  
	DHT11_GPIO_Config();
	
	printf("\r\n***����STM32H743 dht11 ��ʪ�ȴ�����ʵ��***\r\n");
	
  while(1)
	{
		/*����DHT11_Read_TempAndHumidity��ȡ��ʪ�ȣ����ɹ����������Ϣ*/
		if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
		{
      /*ʹ��c��׼��ѱ���ת�����ַ���*/
      sprintf(dispBuff,"Hum:%d.%d RH, Temp:%d.%d  C",DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
      LCD_ClearLine(7);
      /*����������ɫ������ı�����ɫ(�˴��ı�������ָLCD�ı����㣡ע������)*/
      LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
      /*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
      LCD_DisplayStringLine_EN_CH(7,(uint8_t* )dispBuff);
      //���϶ȷ���
      LCD_DisplayChar(390, 155, '.');
      LCD_DisplayChar(390, 156, '.');
		}		
		else
		{
		  sprintf(dispBuff,"Read DHT11 ERROR!");
      LCD_ClearLine(7);
      /*����������ɫ������ı�����ɫ(�˴��ı�������ָLCD�ı����㣡ע������)*/
      LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
      /*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
      LCD_DisplayStringLine_EN_CH(7,(uint8_t* )dispBuff);      
		}
    Delay_ms(1000);
	} 
}

/**
  * @brief  System Clock ����
  *         system Clock ��������: 
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;
  
  /*ʹ�ܹ������ø��� */
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

  /* ��������ʱ��Ƶ�ʵ������ϵͳƵ��ʱ����ѹ���ڿ����Ż����ģ�
		 ����ϵͳƵ�ʵĵ�ѹ����ֵ�ĸ��¿��Բο���Ʒ�����ֲᡣ  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 
  /* ����HSE������ʹ��HSE��ΪԴ����PLL */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
 
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
  
	/* ѡ��PLL��Ϊϵͳʱ��Դ����������ʱ�ӷ�Ƶ�� */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK  | \
																 RCC_CLOCKTYPE_HCLK    | \
																 RCC_CLOCKTYPE_D1PCLK1 | \
																 RCC_CLOCKTYPE_PCLK1   | \
                                 RCC_CLOCKTYPE_PCLK2   | \
																 RCC_CLOCKTYPE_D3PCLK1);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    while(1) { ; }
  }
}
/****************************END OF FILE***************************/

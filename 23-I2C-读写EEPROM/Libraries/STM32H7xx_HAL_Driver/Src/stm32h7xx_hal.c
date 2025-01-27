/**
  ******************************************************************************
  * @file    stm32h7xx_hal.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date   29-December-2017
  * @brief   HAL module driver.
  *          This is the common part of the HAL initialization
  *
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
    [..]
    The common HAL driver contains a set of generic and common APIs that can be
    used by the PPP peripheral drivers and the user to start using the HAL.
    [..]
    The HAL contains two APIs' categories:
         (+) Common HAL APIs
         (+) Services HAL APIs

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @defgroup HAL  HAL
  * @brief HAL module driver.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
 * @brief STM32H7xx HAL Driver version number V1.2.0
   */
#define __STM32H7xx_HAL_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32H7xx_HAL_VERSION_SUB1   (0x02) /*!< [23:16] sub1 version */
#define __STM32H7xx_HAL_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32H7xx_HAL_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32H7xx_HAL_VERSION         ((__STM32H7xx_HAL_VERSION_MAIN << 24)\
                                        |(__STM32H7xx_HAL_VERSION_SUB1 << 16)\
                                        |(__STM32H7xx_HAL_VERSION_SUB2 << 8 )\
                                        |(__STM32H7xx_HAL_VERSION_RC))

#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)
#define VREFBUF_TIMEOUT_VALUE     (uint32_t)10   /* 10 ms  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTick;
static uint32_t uwTickPrio   = (1UL << __NVIC_PRIO_BITS); /* Invalid PRIO */
static HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT;  /* 1KHz */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_Private_Functions  HAL Private Functions
  * @{
  */

/** @defgroup HAL_Group1 Initialization and de-initialization Functions
 *  @brief    Initialization and de-initialization functions
 *
@verbatim
 ===============================================================================
              ##### Initialization and de-initialization functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Initializes the Flash interface the NVIC allocation and initial clock
          configuration. It initializes the systick also when timeout is needed
          and the backup domain when enabled.
      (+) De-Initializes common part of the HAL.
      (+) Configure The time base source to have 1ms time base with a dedicated
          Tick interrupt priority.
        (++) SysTick timer is used by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
        (++) Time base configuration function (HAL_InitTick ()) is called automatically
             at the beginning of the program after reset by HAL_Init() or at any time
             when clock is configured, by HAL_RCC_ClockConfig().
        (++) Source of time base is configured  to generate interrupts at regular
             time intervals. Care must be taken if HAL_Delay() is called from a
             peripheral ISR process, the Tick interrupt line must have higher priority
            (numerically lower) than the peripheral interrupt. Otherwise the caller
            ISR process will be blocked.
       (++) functions affecting time base configurations are declared as __weak
             to make  override possible  in case of other  implementations in user file.
@endverbatim
  * @{
  */

/**
  * @brief  This function is used to initialize the HAL Library; it must be the first
  *         instruction to be executed in the main program (before to call any other
  *         HAL function), it performs the following:
  *           Configures the SysTick to generate an interrupt each 1 millisecond,
  *           which is clocked by the HSI (at this stage, the clock is not yet
  *           configured and thus the system is running from the internal HSI at 16 MHz).
  *           Set NVIC Group Priority to 4.
  *           Calls the HAL_MspInit() callback function defined in user file
  *           "stm32h7xx_hal_msp.c" to do the global low level hardware initialization
  *
  * @note   SysTick is used as time base for the HAL_Delay() function, the application
  *         need to ensure that the SysTick time base is always set to 1 millisecond
  *         to have correct HAL operation.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_Init(void)
{
  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
  if(HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Init the low level hardware */
  HAL_MspInit();

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  This function de-Initializes common part of the HAL and stops the systick.
  *         This function is optional.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DeInit(void)
{
  /* Reset of all peripherals */
  __HAL_RCC_AHB3_FORCE_RESET();
  __HAL_RCC_AHB3_RELEASE_RESET();

  __HAL_RCC_AHB1_FORCE_RESET();
  __HAL_RCC_AHB1_RELEASE_RESET();

  __HAL_RCC_AHB2_FORCE_RESET();
  __HAL_RCC_AHB2_RELEASE_RESET();

  __HAL_RCC_AHB4_FORCE_RESET();
 __HAL_RCC_AHB4_RELEASE_RESET();

  __HAL_RCC_APB3_FORCE_RESET();
  __HAL_RCC_APB3_RELEASE_RESET();

  __HAL_RCC_APB1L_FORCE_RESET();
  __HAL_RCC_APB1L_RELEASE_RESET();

  __HAL_RCC_APB1H_FORCE_RESET();
  __HAL_RCC_APB1H_RELEASE_RESET();

   __HAL_RCC_APB2_FORCE_RESET();
   __HAL_RCC_APB2_RELEASE_RESET();

  __HAL_RCC_APB4_FORCE_RESET();
  __HAL_RCC_APB4_RELEASE_RESET();

  /* De-Init the low level hardware */
  HAL_MspDeInit();

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the MSP.
  * @retval None
  */
__weak void HAL_MspInit(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the MSP.
  * @retval None
  */
__weak void HAL_MspDeInit(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_MspDeInit could be implemented in the user file
   */
}

/**
  * @brief This function configures the source of the time base.
  *        The time source is configured  to have 1ms time base with a dedicated
  *        Tick interrupt priority.
  * @note This function is called  automatically at the beginning of program after
  *       reset by HAL_Init() or at any time when clock is reconfigured  by HAL_RCC_ClockConfig().
  * @note In the default implementation, SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals.
  *       Care must be taken if HAL_Delay() is called from a peripheral ISR process,
  *       The the SysTick interrupt must have higher priority (numerically lower)
  *       than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
  *       The function is declared as __weak  to be overwritten  in case of other
  *       implementation  in user file.
  * @param TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
//__weak HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
//{
//  /* Configure the SysTick to have interrupt in 1ms time basis*/
//  if (HAL_SYSTICK_Config(SystemCoreClock / (1000U / uwTickFreq)) > 0U)
//  {
//    return HAL_ERROR;
//  }

//  /* Configure the SysTick IRQ priority */
//  if (TickPriority < (1UL << __NVIC_PRIO_BITS))
//  {
//    HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
//    uwTickPrio = TickPriority;
//  }
//  else
//  {
//    return HAL_ERROR;
//  }

//  /* Return function status */
//  return HAL_OK;
//}

/**
  * @}
  */

/** @defgroup HAL_Group2 HAL Control functions
 *  @brief    HAL Control functions
 *
@verbatim
 ===============================================================================
                      ##### HAL Control functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
      (+) Provide a tick value in millisecond
      (+) Provide a blocking delay in millisecond
      (+) Suspend the time base source interrupt
      (+) Resume the time base source interrupt
      (+) Get the HAL API driver version
      (+) Get the device identifier
      (+) Get the device revision identifier
      (+) Enable/Disable Debug module during SLEEP mode
      (+) Enable/Disable Debug module during STOP mode
      (+) Enable/Disable Debug module during STANDBY mode

@endverbatim
  * @{
  */

/**
  * @brief This function is called to increment  a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
 * @note This function is declared as __weak to be overwritten in case of other
  *      implementations in user file.
  * @retval None
  */
__weak void HAL_IncTick(void)
{
  uwTick += (uint32_t)uwTickFreq;
}

/**
  * @brief Provides a tick value in millisecond.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval tick value
  */
//__weak uint32_t HAL_GetTick(void)
//{
//  return uwTick;
//}

/**
  * @brief This function returns a tick priority.
  * @retval tick priority
  */
uint32_t HAL_GetTickPrio(void)
{
  return uwTickPrio;
}

/**
  * @brief Set new tick Freq.
  * @retval Status
  */
HAL_StatusTypeDef HAL_SetTickFreq(HAL_TickFreqTypeDef Freq)
{
  HAL_StatusTypeDef status  = HAL_OK;
  assert_param(IS_TICKFREQ(Freq));

  if (uwTickFreq != Freq)
  {
    uwTickFreq = Freq;

    /* Apply the new tick Freq  */
    status = HAL_InitTick(uwTickPrio);
  }

  return status;
}

/**
  * @brief Return tick frequency.
  * @retval tick period in Hz
  */
HAL_TickFreqTypeDef HAL_GetTickFreq(void)
{
  return uwTickFreq;
}

/**
  * @brief This function provides minimum delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay  specifies the delay time length, in milliseconds.
  * @retval None
  */
//__weak void HAL_Delay(uint32_t Delay)
//{
//  uint32_t tickstart = HAL_GetTick();
//  uint32_t wait = Delay;

//  /* Add a freq to guarantee minimum wait */
//  if (wait < HAL_MAX_DELAY)
//  {
//    wait += (uint32_t)(uwTickFreq);
//  }

//  while ((HAL_GetTick() - tickstart) < wait)
//  {
//  }
//}

/**
  * @brief Suspend Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_SuspendTick()
  *       is called, the the SysTick interrupt will be disabled and so Tick increment
  *       is suspended.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_SuspendTick(void)
{
  /* Disable SysTick Interrupt */
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief Resume Tick increment.
  * @note In the default implementation , SysTick timer is the source of time base. It is
  *       used to generate interrupts at regular time intervals. Once HAL_ResumeTick()
  *       is called, the the SysTick interrupt will be enabled and so Tick increment
  *       is resumed.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @retval None
  */
__weak void HAL_ResumeTick(void)
{
  /* Enable SysTick Interrupt */
  SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
}

/**
  * @brief  Returns the HAL revision
  * @retval version : 0xXYZR (8bits for each decimal, R for RC)
  */
uint32_t HAL_GetHalVersion(void)
{
 return __STM32H7xx_HAL_VERSION;
}

/**
  * @brief  Returns the device revision identifier.
  * @retval Device revision identifier
  */
uint32_t HAL_GetREVID(void)
{
   return((DBGMCU->IDCODE) >> 16);
}

/**
  * @brief  Returns the device identifier.
  * @retval Device identifier
  */
uint32_t HAL_GetDEVID(void)
{
   return((DBGMCU->IDCODE) & IDCODE_DEVID_MASK);
}

/**
  * @brief Configure the internal voltage reference buffer voltage scale.
  * @param VoltageScaling  specifies the output voltage to achieve
  *          This parameter can be one of the following values:
  *            @arg SYSCFG_VREFBUF_VOLTAGE_SCALE0: VREF_OUT1 around 2.048 V. 
  *                                                This requires VDDA equal to or higher than 2.4 V.
  *            @arg SYSCFG_VREFBUF_VOLTAGE_SCALE1: VREF_OUT2 around 2.5 V. 
  *                                                This requires VDDA equal to or higher than 2.8 V.
  *            @arg SYSCFG_VREFBUF_VOLTAGE_SCALE2: VREF_OUT3 around 1.5 V. 
  *                                                This requires VDDA equal to or higher than 1.8 V.
  *            @arg SYSCFG_VREFBUF_VOLTAGE_SCALE3: VREF_OUT4 around 1.8 V. 
  *                                                This requires VDDA equal to or higher than 2.1 V.
  * @retval None
  */
void HAL_SYSCFG_VREFBUF_VoltageScalingConfig(uint32_t VoltageScaling)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_VREFBUF_VOLTAGE_SCALE(VoltageScaling));
  
  MODIFY_REG(VREFBUF->CSR, VREFBUF_CSR_VRS, VoltageScaling);
}

/**
  * @brief Configure the internal voltage reference buffer high impedance mode.
  * @param Mode  specifies the high impedance mode
  *          This parameter can be one of the following values:
  *            @arg SYSCFG_VREFBUF_HIGH_IMPEDANCE_DISABLE: VREF+ pin is internally connect to VREFINT output.
  *            @arg SYSCFG_VREFBUF_HIGH_IMPEDANCE_ENABLE: VREF+ pin is high impedance.
  * @retval None
  */
void HAL_SYSCFG_VREFBUF_HighImpedanceConfig(uint32_t Mode)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_VREFBUF_HIGH_IMPEDANCE(Mode));
  
  MODIFY_REG(VREFBUF->CSR, VREFBUF_CSR_HIZ, Mode);
}

/**
  * @brief  Tune the Internal Voltage Reference buffer (VREFBUF).
  * @retval None
  */
void HAL_SYSCFG_VREFBUF_TrimmingConfig(uint32_t TrimmingValue)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_VREFBUF_TRIMMING(TrimmingValue));
  
  MODIFY_REG(VREFBUF->CCR, VREFBUF_CCR_TRIM, TrimmingValue);
}

/**
  * @brief  Enable the Internal Voltage Reference buffer (VREFBUF).
  * @retval HAL_OK/HAL_TIMEOUT
  */
HAL_StatusTypeDef HAL_SYSCFG_EnableVREFBUF(void)
{
  uint32_t  tickstart = 0;
  
  SET_BIT(VREFBUF->CSR, VREFBUF_CSR_ENVR);
  
  /* Get Start Tick*/
  tickstart = HAL_GetTick();

  /* Wait for VRR bit  */
  while(READ_BIT(VREFBUF->CSR, VREFBUF_CSR_VRR) == RESET)
  {
    if((HAL_GetTick() - tickstart) > VREFBUF_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }
  
  return HAL_OK;
}

/**
  * @brief  Disable the Internal Voltage Reference buffer (VREFBUF).
  *
  * @retval None
  */
void HAL_SYSCFG_DisableVREFBUF(void)
{
  CLEAR_BIT(VREFBUF->CSR, VREFBUF_CSR_ENVR);
}

/**
  * @brief  Ethernet PHY Interface Selection either MII or RMII
  * @param  SYSCFG_ETHInterface: Selects the Ethernet PHY interface
  *   This parameter can be one of the following values:
  *   @arg SYSCFG_ETH_MII : Select the Media Independent Interface
  *   @arg SYSCFG_ETH_RMII: Select the Reduced Media Independent Interface
  * @retval None
  */
void HAL_SYSCFG_ETHInterfaceSelect(uint32_t SYSCFG_ETHInterface)
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_ETHERNET_CONFIG(SYSCFG_ETHInterface));

  MODIFY_REG(SYSCFG->PMCR, SYCFG_PMCR_EPIS_SEL, (uint32_t)(SYSCFG_ETHInterface));  
}


/**
  * @brief  Analog Switch control for dual analog pads.
  * @param  SYSCFG_AnalogSwitch: Selects the analog pad
  *   This parameter can be one or a combination of the following values:
  *   @arg SYSCFG_SWITCH_PA0 : Select PA0 analog switch
  *   @arg SYSCFG_SWITCH_PA1:  Select PA1 analog switch
  *   @arg SYSCFG_SWITCH_PC2 : Select PC2 analog switch
  *   @arg SYSCFG_SWITCH_PC3:  Select PC3 analog switch
  * @param  SYSCFG_SwitchState: Open or Close the analog switch between dual pads (PXn and PXn_C)
  *   This parameter can be one or a combination of the following values:
  *   @arg SYSCFG_SWITCH_PA0_OPEN  
  *   @arg SYSCFG_SWITCH_PA0_CLOSE 
  *   @arg SYSCFG_SWITCH_PA1_OPEN  
  *   @arg SYSCFG_SWITCH_PA1_CLOSE 
  *   @arg SYSCFG_SWITCH_PC2_OPEN  
  *   @arg SYSCFG_SWITCH_PC2_CLOSE 
  *   @arg SYSCFG_SWITCH_PC3_OPEN 
  *   @arg SYSCFG_SWITCH_PC3_CLOSE 
  * @retval None
  */

void HAL_SYSCFG_AnalogSwitchConfig(uint32_t SYSCFG_AnalogSwitch , uint32_t SYSCFG_SwitchState )
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_ANALOG_SWITCH(SYSCFG_AnalogSwitch));
  assert_param(IS_SYSCFG_SWITCH_STATE(SYSCFG_SwitchState));
  
  MODIFY_REG(SYSCFG->PMCR, (uint32_t) SYSCFG_AnalogSwitch, (uint32_t)(SYSCFG_SwitchState));  
}


/**
  * @brief  Enables the booster to reduce the total harmonic distortion of the analog
  *         switch when the supply voltage is lower than 2.7 V.
  * @note   Activating the booster allows to guaranty the analog switch AC performance
  *         when the supply voltage is below 2.7 V: in this case, the analog switch
  *         performance is the same on the full voltage range
  * @retval None
  */
void HAL_SYSCFG_EnableBOOST(void)
{
 SET_BIT(SYSCFG->PMCR, SYSCFG_PMCR_BOOSTEN) ;
}

/**
  * @brief  Disables the booster
  * @note   Activating the booster allows to guaranty the analog switch AC performance
  *         when the supply voltage is below 2.7 V: in this case, the analog switch
  *         performance is the same on the full voltage range
  * @retval None
  */
void HAL_SYSCFG_DisableBOOST(void)
{
 CLEAR_BIT(SYSCFG->PMCR, SYSCFG_PMCR_BOOSTEN) ;
}


/**
  * @brief  BootCM7 address 0 configuration
  * @param  BootRegister :Specifies the Boot Address register (Address0 or Address1)
  *   This parameter can be one of the following values:
  *   @arg SYSCFG_BOOT_ADDR0 : Select the boot address0 
  *   @arg SYSCFG_BOOT_ADDR1:  Select the boot address1
  * @param  BootAddress :Specifies the CM7 Boot Address to be loaded in Address0 or Address1
  * @retval None
  */
void HAL_SYSCFG_CM7BootAddConfig(uint32_t BootRegister, uint32_t BootAddress)
{
  /* Check the parameters */
  assert_param(IS_SYSCFG_BOOT_REGISTER(BootRegister));
  assert_param(IS_SYSCFG_BOOT_ADDRESS(BootAddress));
  if ( BootRegister == SYSCFG_BOOT_ADDR0 )
  {
    /* Configure CM7 BOOT ADD0 */
    MODIFY_REG(SYSCFG->UR2, SYSCFG_UR2_BOOT_ADD0, ((BootAddress >> 16) << POSITION_VAL(SYSCFG_UR2_BOOT_ADD0)));
  }   
  else 
  {
    /* Configure CM7 BOOT ADD1 */
    MODIFY_REG(SYSCFG->UR3, SYSCFG_UR3_BOOT_ADD1, (BootAddress >> 16));
  }
  
}


/**
  * @brief  Enables the I/O Compensation Cell.
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V.
  * @retval None
  */
void HAL_EnableCompensationCell(void)
{
  SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_EN) ;
}

/**
  * @brief  Power-down the I/O Compensation Cell.
  * @note   The I/O compensation cell can be used only when the device supply
  *         voltage ranges from 2.4 to 3.6 V.
  * @retval None
  */
void HAL_DisableCompensationCell(void)
{
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_EN) ;
}


/**
  * @brief  To Enable optimize the I/O speed when the product voltage is low.
  * @note   This bit is active only if PRODUCT_BELOW_25V user option bit is set. It must be
  *         used only if the product supply voltage is below 2.5 V. Setting this bit when VDD is
  *         higher than 2.5 V might be destructive.
  * @retval None
  */
void HAL_SYSCFG_EnableIOSpeedOptimize(void)
{
   SET_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV) ;
}

/**
  * @brief  To Disable optimize the I/O speed when the product voltage is low.
  * @note   This bit is active only if PRODUCT_BELOW_25V user option bit is set. It must be
  *         used only if the product supply voltage is below 2.5 V. Setting this bit when VDD is
  *         higher than 2.5 V might be destructive.
  * @retval None
  */
void HAL_SYSCFG_DisableIOSpeedOptimize(void)
{
  CLEAR_BIT(SYSCFG->CCCSR, SYSCFG_CCCSR_HSLV) ;
}

/**
  * @brief  Code selection for the I/O Compensation cell
  * @param  SYSCFG_CompCode: Selects the code to be applied for the I/O compensation cell
  *   This parameter can be one of the following values:
  *   @arg SYSCFG_CELL_CODE : Select Code from the cell (available in the SYSCFG_CCVR)
  *   @arg SYSCFG_REGISTER_CODE: Select Code from the SYSCFG compensation cell code register (SYSCFG_CCCR)
  * @retval None
  */
void HAL_SYSCFG_CompensationCodeSelect(uint32_t SYSCFG_CompCode)
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_CODE_SELECT(SYSCFG_CompCode));
  MODIFY_REG(SYSCFG->CCCSR, SYSCFG_CCCSR_CS, (uint32_t)(SYSCFG_CompCode));  
}

/**
  * @brief  Code selection for the I/O Compensation cell
  * @param  SYSCFG_PMOSCode: PMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *          SYSCFG_CMPCR is set
  * @param  SYSCFG_NMOSCode: NMOS compensation code
  *         This code is applied to the I/O compensation cell when the CS bit of the
  *          SYSCFG_CMPCR is set
  * @retval None
  */
void HAL_SYSCFG_CompensationCodeConfig(uint32_t SYSCFG_PMOSCode, uint32_t SYSCFG_NMOSCode )
{
  /* Check the parameter */
  assert_param(IS_SYSCFG_CODE_CONFIG(SYSCFG_PMOSCode));
  assert_param(IS_SYSCFG_CODE_CONFIG(SYSCFG_NMOSCode));
  MODIFY_REG(SYSCFG->CCCR, SYSCFG_CCCR_NCC|SYSCFG_CCCR_PCC, (((uint32_t)(SYSCFG_PMOSCode)<< 4)|(uint32_t)(SYSCFG_NMOSCode)) );  
}


/**
  * @brief  Enable the Debug Module during Domain1 SLEEP mode
  * @retval None
  */
void HAL_EnableDBGSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD1);
}

/**
  * @brief  Disable the Debug Module during Domain1 SLEEP mode
  * @retval None
  */
void HAL_DisableDBGSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEPD1);
}

/**
  * @brief  Enable the Debug Module during Domain1 STOP mode
  * @retval None
  */
void HAL_EnableDBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD1);
}

/**
  * @brief  Disable the Debug Module during Domain1 STOP mode
  * @retval None
  */
void HAL_DisableDBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD1);
}

/**
  * @brief  Enable the Debug Module during Domain1 STANDBY mode
  * @retval None
  */
void HAL_EnableDBGStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD1);
}

/**
  * @brief  Disable the Debug Module during Domain1 STANDBY mode
  * @retval None
  */
void HAL_DisableDBGStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD1);
}



/**
  * @brief  Enable the Debug Module during Domain3 STOP mode
  * @retval None
  */
void HAL_EnableDomain3DBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD3);
}

/**
  * @brief  Disable the Debug Module during Domain3 STOP mode
  * @retval None
  */
void HAL_DisableDomain3DBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOPD3);
}

/**
  * @brief  Enable the Debug Module during Domain3 STANDBY mode
  * @retval None
  */
void HAL_EnableDomain3DBGStandbyMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD3);
}

/**
  * @brief  Disable the Debug Module during Domain3 STANDBY mode
  * @retval None
  */
void HAL_DisableDomain3DBGStandbyMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBYD3);
}

/**
  * @brief  Set the FMC Memory Mapping Swapping config.
  * @param  BankMapConfig: Defines the FMC Bank mapping configuration. This parameter can be
            FMC_SWAPBMAP_DISABLE, FMC_SWAPBMAP_SDRAM_SRAM, FMC_SWAPBMAP_SDRAMB2
  * @retval HAL state
  */
void HAL_SetFMCMemorySwappingConfig(uint32_t BankMapConfig)
{
  /* Check the parameter */
  assert_param(IS_FMC_SWAPBMAP_MODE(BankMapConfig));
  MODIFY_REG(FMC_Bank1->BTCR[0], FMC_BCR1_BMAP, BankMapConfig);
}

/**
  * @brief  Get FMC Bank mapping mode.
  * @retval The FMC Bank mapping mode. This parameter can be 
            FMC_SWAPBMAP_DISABLE, FMC_SWAPBMAP_SDRAM_SRAM, FMC_SWAPBMAP_SDRAMB2
*/
uint32_t HAL_GetFMCMemorySwappingConfig(void)
{
  return READ_BIT(FMC_Bank1->BTCR[0], FMC_BCR1_BMAP);
}

/**
  * @brief  Configure the EXTI input event line edge
  * @note    No edge configuration for direct lines but for configurable lines:(EXTI_LINE0..EXTI_LINE21),
  *          EXTI_LINE49,EXTI_LINE51,EXTI_LINE82,EXTI_LINE84,EXTI_LINE85 and EXTI_LINE86.
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0....EXTI_LINE88)excluding :line45, line81,line83 which are reserved  
  * @param   EXTI_Edge: Specifies  EXTI line Edge used.
  *          This parameter can be one of the following values :
  *   @arg EXTI_RISING_EDGE : Configurable line, with Rising edge trigger detection
  *   @arg EXTI_FALLING_EDGE: Configurable line, with Falling edge trigger detection
  * @retval None
  */
void HAL_EXTI_EdgeConfig(uint32_t EXTI_Line , uint32_t EXTI_Edge )
{
  /* Check the parameter */
    assert_param(IS_EXTI_CONFIG_LINE(EXTI_Line));
    assert_param(IS_EXTI_EDGE_LINE(EXTI_Edge));

    /* Clear Rising Falling edge configuration */
    CLEAR_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->FTSR1)) + ((EXTI_Line >> 5 ) * 0x20)), (uint32_t)(1 << (EXTI_Line & 0x1F))); 
    CLEAR_BIT( *(__IO uint32_t *) (((uint32_t) &(EXTI->RTSR1)) + ((EXTI_Line >> 5 ) * 0x20)), (uint32_t)(1 << (EXTI_Line & 0x1F)));

     if( (EXTI_Edge & EXTI_RISING_EDGE) == EXTI_RISING_EDGE)
        {
         SET_BIT( *(__IO uint32_t *) (((uint32_t) &(EXTI->RTSR1)) + ((EXTI_Line >> 5 ) * 0x20)), (uint32_t)(1 << (EXTI_Line & 0x1F))); 
        }
      if( (EXTI_Edge & EXTI_FALLING_EDGE) == EXTI_FALLING_EDGE)
        {
         SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->FTSR1)) + ((EXTI_Line >> 5 ) * 0x20)), (uint32_t)(1 << (EXTI_Line & 0x1F)));
        }
}
                                                                                    
/**
  * @brief  Generates a Software interrupt on selected EXTI line.
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *          (EXTI_LINE0..EXTI_LINE21),EXTI_LINE49,EXTI_LINE51,EXTI_LINE82,EXTI_LINE84,EXTI_LINE85 and EXTI_LINE86.
  * @retval None
  */
void HAL_EXTI_GenerateSWInterrupt(uint32_t EXTI_Line)
{
  /* Check the parameters */
  assert_param(IS_EXTI_CONFIG_LINE(EXTI_Line));

  SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->SWIER1)) + ((EXTI_Line >> 5 ) * 0x20)), (uint32_t)(1 << (EXTI_Line & 0x1F))); 
}


/**
  * @brief  Clears the EXTI's line pending flags for Domain D1
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0....EXTI_LINE88)excluding :line45, line81,line83 which are reserved 
  * @retval None
  */
void HAL_EXTI_D1_ClearFlag(uint32_t EXTI_Line)
{
  /* Check the parameters */
 assert_param(IS_EXTI_D1_LINE(EXTI_Line));
 SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI_D1->PR1)) + ((EXTI_Line >> 5 ) * 0x10)), (uint32_t)(1 << (EXTI_Line & 0x1F))); 

}

/**
  * @brief  Configure the EXTI input event line for Domain D1
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0....EXTI_LINE88)excluding :line45, line81,line83 which are reserved  
  * @param   EXTI_Mode: Specifies which EXTI line is used as interrupt or an event. 
  *          This parameter can be one or a combination of the following values :
  *   @arg EXTI_MODE_IT :  Interrupt Mode selected
  *   @arg EXTI_MODE_EVT : Event Mode selected
  * @param   EXTI_LineCmd controls (Enable/Disable) the EXTI line.

  * @retval None
  */
void HAL_EXTI_D1_EventInputConfig(uint32_t EXTI_Line , uint32_t EXTI_Mode,  uint32_t EXTI_LineCmd )
{
  /* Check the parameter */
    assert_param(IS_EXTI_D1_LINE(EXTI_Line));
    assert_param(IS_EXTI_MODE_LINE(EXTI_Mode));

  if( (EXTI_Mode & EXTI_MODE_IT) == EXTI_MODE_IT)
      {
         if( EXTI_LineCmd == DISABLE)
           {
           /* Clear EXTI line configuration */
            CLEAR_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI_D1->IMR1)) + ((EXTI_Line >> 5 ) * 0x10)),(uint32_t)(1 << (EXTI_Line & 0x1F)) ); 
           }
         else 
           {
            SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI_D1->IMR1)) + ((EXTI_Line >> 5 ) * 0x10)), (uint32_t)(1 << (EXTI_Line & 0x1F))); 
           }
     }
 if( (EXTI_Mode & EXTI_MODE_EVT) == EXTI_MODE_EVT)
      {
          if( EXTI_LineCmd == DISABLE)
           {
             /* Clear EXTI line configuration */
             CLEAR_BIT(  *(__IO uint32_t *) (((uint32_t) &(EXTI_D1->EMR1)) + ((EXTI_Line >> 5 ) * 0x10)), (uint32_t)(1 << (EXTI_Line & 0x1F))); 
           }
           else
           {  
            SET_BIT(  *(__IO uint32_t *) (((uint32_t) &(EXTI_D1->EMR1)) + ((EXTI_Line >> 5 ) * 0x10)), (uint32_t)(1 << (EXTI_Line & 0x1F))); 
           } 
      }

}


/**
  * @brief  Configure the EXTI input event line for Domain D3 
  * @param   EXTI_Line: Specifies the EXTI LINE, it can be one of the following values,
  *         (EXTI_LINE0...EXTI_LINE15),(EXTI_LINE19...EXTI_LINE21),EXTI_LINE25, EXTI_LINE34,
  *          EXTI_LINE35,EXTI_LINE41,(EXTI_LINE48...EXTI_LINE53),EXTI_LINE88 
  * @param   EXTI_LineCmd controls (Enable/Disable) the EXTI line.
  * @param   EXTI_ClearSrc: Specifies the clear source of D3 pending event.
  *          This parameter can be one of the following values :
  *   @arg BDMA_CH6_CLEAR : BDMA ch6 event selected as D3 domain pendclear source
  *   @arg BDMA_CH7_CLEAR : BDMA ch7 event selected as D3 domain pendclear source
  *   @arg LPTIM4_OUT_CLEAR : LPTIM4 out selected as D3 domain pendclear source
  *   @arg LPTIM5_OUT_CLEAR : LPTIM5 out selected as D3 domain pendclear source
  * @retval None
  */
void HAL_EXTI_D3_EventInputConfig(uint32_t EXTI_Line, uint32_t EXTI_LineCmd , uint32_t EXTI_ClearSrc  )
{

 /* Check the parameter */
    assert_param(IS_EXTI_D3_LINE(EXTI_Line));
    assert_param(IS_EXTI_D3_CLEAR(EXTI_ClearSrc));

    if( EXTI_LineCmd == DISABLE)
      {
      /* Clear EXTI line configuration */
       CLEAR_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->D3PMR1)) + ((EXTI_Line >> 5 ) * 0x20)),(uint32_t)(1 << (EXTI_Line & 0x1F)) ); 
      }
   else
     { 
       SET_BIT(*(__IO uint32_t *) (((uint32_t) &(EXTI->D3PMR1)) +((EXTI_Line >> 5 ) * 0x20)), (uint32_t)(1 << (EXTI_Line & 0x1F)));
     }
 

    if ( (EXTI_Line>>4)%2 ==0)
    {
     MODIFY_REG(*(__IO uint32_t *) (((uint32_t) &(EXTI->D3PCR1L)) + ((EXTI_Line >> 5 ) * 0x20)), \
    (uint32_t)(3 << ((EXTI_Line*2) & 0x1F)), (uint32_t)(EXTI_ClearSrc << ((EXTI_Line*2) & 0x1F))) ;
    }

    else 
      {
      MODIFY_REG(*(__IO uint32_t *) (((uint32_t) &(EXTI->D3PCR1H)) + ((EXTI_Line >> 5 ) * 0x20)), \
     (uint32_t)(3 << ((EXTI_Line*2) & 0x1F)), (uint32_t)(EXTI_ClearSrc << ((EXTI_Line*2) & 0x1F))) ;
      }

}


  
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

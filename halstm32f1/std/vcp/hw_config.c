/**
  ******************************************************************************
  * @file    hw_config.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Hardware Configuration & Setup
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "mcush.h"
#include "hal.h"
#include "hw_config.h"

#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "task_blink.h"

extern QueueHandle_t hal_vcp_queue_rx;
extern QueueHandle_t hal_vcp_queue_tx;


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
USART_InitTypeDef USART_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
uint8_t  USART_Rx_Buffer [USART_RX_DATA_SIZE]; 
uint32_t USART_Rx_ptr_in = 0;
uint32_t USART_Rx_ptr_out = 0;
uint32_t USART_Rx_length  = 0;

uint8_t  USB_Tx_State = 0;
/* Extern variables ----------------------------------------------------------*/

extern LINE_CODING linecoding;


/* PA11  --- USB_DM
   PA12  --- USB_DP 
*/
void init_usb_pins_at_startup(void)
{
    GPIO_InitTypeDef gpio_init;

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR, ENABLE );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );
    gpio_init.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    //gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Mode = GPIO_Mode_IPD;
    //gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    //gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init( GPIOA, &gpio_init );
    //GPIO_ResetBits( GPIOA, GPIO_Pin_11 | GPIO_Pin_12 );
}


void init_usb_pins(void)
{
    //GPIO_InitTypeDef gpio_init;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );
    //gpio_init.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    //gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    //gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    //GPIO_Init( GPIOA, &gpio_init );
}


void hal_pre_init(void)
{
    /* prepare DP/DM pins before hal_init */
    //init_usb_pins_at_startup();
}



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
#if HAL_RESET_USB_PINS
    /* pull DM/DP low to reset */
    hal_gpio_set_output( 0, 3<<11 );  
    hal_gpio_clr( 0, 3<<11 );  
    hal_delay_ms( 5 );
#endif
    init_usb_pins(); 
}

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
    /* Select USBCLK source */
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
    
    /* Enable the USB clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
    hal_led_clr(3);
    /* Set the device state to suspend */
    bDeviceState = SUSPENDED;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
    DEVICE_INFO *pInfo = &Device_Info;

    hal_led_set(3);
    /* Set the device state to the correct state */
    if (pInfo->Current_Configuration != 0)
    {
      /* Device configured */
      bDeviceState = CONFIGURED;
    }
    else
    {
      bDeviceState = ATTACHED;
    }
    /*Enable SystemCoreClock*/
    //SystemInit();
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the USB Wake-up interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
}

/*******************************************************************************
* Function Name  :  USART_Config_Default.
* Description    :  configure the EVAL_COM1 with default values.
* Input          :  None.
* Return         :  None.
*******************************************************************************/
void USART_Config_Default(void)
{
}

/*******************************************************************************
* Function Name  :  USART_Config.
* Description    :  Configure the EVAL_COM1 according to the line coding structure.
* Input          :  None.
* Return         :  Configuration status
                    TRUE : configuration done with success
                    FALSE : configuration aborted.
*******************************************************************************/
bool USART_Config(void)
{
    return (TRUE);
}

/*******************************************************************************
* Function Name  : USB_To_USART_Send_Data.
* Description    : send the received data from USB to the UART 0.
* Input          : data_buffer: data address.
                   Nb_bytes: number of bytes to send.
* Return         : none.
*******************************************************************************/
void USB_To_USART_Send_Data(uint8_t* data_buffer, uint8_t Nb_bytes)
{
    uint32_t i;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    for (i = 0; i < Nb_bytes; i++)
    {
        if( xQueueSendFromISR( hal_vcp_queue_rx, data_buffer + i, &xHigherPriorityTaskWoken ) == errQUEUE_FULL )
            break;
    }  
}

/*******************************************************************************
* Function Name  : Handle_USBAsynchXfer.
* Description    : send data to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
void Handle_USBAsynchXfer (void)
{
  uint16_t USB_Tx_ptr;
  uint16_t USB_Tx_length;
  
  if(USB_Tx_State != 1)
  {
    if (USART_Rx_ptr_out == USART_RX_DATA_SIZE)
    {
      USART_Rx_ptr_out = 0;
    }
    
    if(USART_Rx_ptr_out == USART_Rx_ptr_in) 
    {
      USB_Tx_State = 0; 
      return;
    }
    
    if(USART_Rx_ptr_out > USART_Rx_ptr_in) /* rollback */
    { 
      USART_Rx_length = USART_RX_DATA_SIZE - USART_Rx_ptr_out;
    }
    else 
    {
      USART_Rx_length = USART_Rx_ptr_in - USART_Rx_ptr_out;
    }
    
    if (USART_Rx_length > VIRTUAL_COM_PORT_DATA_SIZE)
    {
      USB_Tx_ptr = USART_Rx_ptr_out;
      USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;
      
      USART_Rx_ptr_out += VIRTUAL_COM_PORT_DATA_SIZE;	
      USART_Rx_length -= VIRTUAL_COM_PORT_DATA_SIZE;	
    }
    else
    {
      USB_Tx_ptr = USART_Rx_ptr_out;
      USB_Tx_length = USART_Rx_length;
      
      USART_Rx_ptr_out += USART_Rx_length;
      USART_Rx_length = 0;
    }
    USB_Tx_State = 1; 
    UserToPMABufferCopy(&USART_Rx_Buffer[USB_Tx_ptr], ENDP1_TXADDR, USB_Tx_length);
    SetEPTxCount(ENDP1, USB_Tx_length);
    SetEPTxValid(ENDP1); 
  }  
}
/*******************************************************************************
* Function Name  : UART_To_USB_Send_Data.
* Description    : send the received data from UART 0 to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
void USART_To_USB_Send_Data(void)
{
    char c;

    while( xQueueReceiveFromISR( hal_vcp_queue_tx, &c, 0 ) == pdTRUE )
    {
        USART_Rx_Buffer[USART_Rx_ptr_in] = c;
     
        USART_Rx_ptr_in++;
  
        if(USART_Rx_ptr_in == USART_RX_DATA_SIZE)
            USART_Rx_ptr_in = 0;
        
        if( USART_Rx_ptr_in == USART_Rx_ptr_out )
            break;
    }
    //set_errno(USART_Rx_ptr_in);
    //hal_led_toggle(0);
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
    char buf[32];

    hal_get_serial_number( buf );
    byte_to_unicode( (uint8_t*)buf, (uint16_t*)&Virtual_Com_Port_StringSerial[2], (VIRTUAL_COM_PORT_SIZ_STRING_SERIAL-1)/2, 0 );
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

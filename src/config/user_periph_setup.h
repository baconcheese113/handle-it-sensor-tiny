/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

// #include "arch.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "spi_flash.h"
#include "i2c.h"
#include "i2c_eeprom.h"


/*
 * DEFINES
 ****************************************************************************************
 */


/****************************************************************************************/
/* UART2 configuration to use with arch_console print messages                          */
/****************************************************************************************/
#define UART                        UART2
// Define UART2 Tx Pad
#define UART2_TX_PORT           GPIO_PORT_0
#define UART2_TX_PIN            GPIO_PIN_11

// Define UART2 Settings
#define UART2_BAUDRATE              UART_BAUDRATE_115200
#define UART2_DATABITS              UART_DATABITS_8
#define UART2_PARITY                UART_PARITY_NONE
#define UART2_STOPBITS              UART_STOPBITS_1
#define UART2_AFCE                  UART_AFCE_DIS
#define UART2_FIFO                  UART_FIFO_EN
#define UART2_TX_FIFO_LEVEL         UART_TX_FIFO_LEVEL_0
#define UART2_RX_FIFO_LEVEL         UART_RX_FIFO_LEVEL_0

/****************************************************************************************/
/* SPI configuration                                                                    */
/****************************************************************************************/
// Define SPI Pads
#define SPI_EN_PORT             GPIO_PORT_0
#define SPI_EN_PIN              GPIO_PIN_1

#define SPI_CLK_PORT            GPIO_PORT_0
#define SPI_CLK_PIN             GPIO_PIN_4

#define SPI_DO_PORT             GPIO_PORT_0
#define SPI_DO_PIN              GPIO_PIN_0

#define SPI_DI_PORT             GPIO_PORT_0
#define SPI_DI_PIN              GPIO_PIN_3

// Define SPI Configuration
#define SPI_MS_MODE             SPI_MS_MODE_MASTER
#define SPI_CP_MODE             SPI_CP_MODE_0
#define SPI_WSZ                 SPI_MODE_8BIT
#define SPI_CS                  SPI_CS_0

#define SPI_SPEED_MODE          SPI_SPEED_MODE_4MHz
#define SPI_EDGE_CAPTURE        SPI_MASTER_EDGE_CAPTURE

/****************************************************************************************/
/* SPI Flash configuration                                                              */
/****************************************************************************************/
#define SPI_FLASH_DEV_SIZE          (256 * 1024)

/****************************************************************************************/
/* Wake-up from hibernation and measure pressure configuration                          */
/****************************************************************************************/
#define PRESSURE_PORT		        GPIO_PORT_0
#define	PRESSURE_PIN		        GPIO_PIN_5
#define PRESSURE_PIN_MASK           (1 << PRESSURE_PIN)

#define RESET_FLASH_PORT            GPIO_PORT_0
#define RESET_FLASH_PIN             GPIO_PIN_6

/***************************************************************************************/
/* Production debug output configuration                                               */
/***************************************************************************************/
#if PRODUCTION_DEBUG_OUTPUT
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_0
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_7
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @brief   Initializes application's peripherals and pins
 ****************************************************************************************
 */
void periph_init(void);

/**
 ****************************************************************************************
 * @brief   Reserves application's specific GPIOs
 * @details Used only in Development mode (#if DEVELOPMENT_DEBUG)
 *          i.e. to reserve P0_1 as Generic Purpose I/O:
 *          RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
 ****************************************************************************************
 */
void GPIO_reservations(void);

/**
 ****************************************************************************************
 * @brief   Sets the functionality of application pads
 * @details i.e. to set P0_1 as Generic purpose Output:
 *          GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
 ****************************************************************************************
 */
void set_pad_functions(void);

#endif // _USER_PERIPH_SETUP_H_

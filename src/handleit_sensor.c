/**
 ****************************************************************************************
 *
 * @file handleit_sensor.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (C) 2012-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"
#include "gapc_task.h"
#include "user_periph_setup.h"
#include "wkupct_quadec.h"
#include "app_easy_msg_utils.h"
#include "gpio.h"
#include "handleit_sensor.h"
#include "arch.h"
#include "arch_api.h"
#include "arch_hibernation.h"
#include "app_task.h"
#include <stdio.h>
#include "arch_system.h"
#include "custs1.h"
#include "custs1_task.h"
#include "uart.h"
#include "uart_utils.h"
#include "user_custs1_def.h"
#include "arch_console.h"
#include "app_default_handlers.h"
#include "app_easy_gap.h"
#include "user_version.h"

#if (BLE_SUOTA_RECEIVER)
#include "app_suotar.h"
#endif

#if defined (CFG_SPI_FLASH_ENABLE)
    #include "spi_flash.h"
#endif

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

/**
 ****************************************************************************************
 * @brief Function to check sensor data and update BLE characteristic
 ****************************************************************************************
 */
void update_adv_data(void)
{
    arch_printf("update_adv_data()\n\r");
    bool isPressing = !GPIO_GetPinStatus(PRESSURE_PORT, PRESSURE_PIN);
    arch_printf("isPressing is %s\n\r", isPressing ? "TRUE" : "FALSE");
    
    // Update BLE characteristic
    struct custs1_val_set_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_SET_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_set_req,
                                                          DEF_SVC2_WRITE_VAL_1_CHAR_LEN);

    // req->conidx = app_env->conidx;
    // req->notification = true;
    req->handle = SVC2_WRITE_1_VAL;
    req->length = DEF_SVC2_WRITE_VAL_1_CHAR_LEN;
    // Since it's difficult to tell if force is applied to the resistor, we always assume isPressing
    req->value[0] = 0x1F; // isPressing ? 0x1F : 0x00;
    req->value[1] = 0;
    req->value[2] = 0;
    req->value[3] = 0;

    ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief Handles APP_WAKEUP_MSG sent when device exits deep sleep. Triggered by button press.
 ****************************************************************************************
*/
static void app_wakeup_cb(void)
{
    arch_printf("app_wakeup_cb()\n\r");
    arch_printf("state:%s\n\r", ke_state_get(TASK_APP));
    // If state is not idle, ignore the message
    if (ke_state_get(TASK_APP) == APP_CONNECTABLE)
    {
        default_advertise_operation();
    }
}

/**
 ****************************************************************************************
 * @brief Routine to resume system from sleep state.
 ****************************************************************************************
 */
static void app_resume_system_from_sleep(void)
{
    if (arch_ble_ext_wakeup_get())
    {
        arch_set_sleep_mode(app_default_sleep_mode);
        arch_ble_force_wakeup();
        arch_ble_ext_wakeup_off();
        app_easy_wakeup();
    }
}

/**
 ****************************************************************************************
 * @brief Button press callback function. Registered in WKUPCT driver.
 ****************************************************************************************
 */
static void app_button_press_cb(void)
{
    arch_printf("app_button_press_cb()\n\r");
#if (BLE_PROX_REPORTER)
    if (alert_state.lvl != PROXR_ALERT_NONE)
    {
        arch_printf("\tapp_proxr_alert_stop()\n\r");
        app_proxr_alert_stop();
    }
#endif
    app_resume_system_from_sleep();

    
    update_adv_data();

    app_button_enable();
}


/**
 ****************************************************************************************
 * @brief Sets button as wakeup trigger
 ****************************************************************************************
*/
void app_button_enable(void)
{
    arch_printf("app_button_enable()\n\r");
    app_easy_wakeup_set(app_wakeup_cb);
    wkupct_register_callback(app_button_press_cb);
    // We can go to sleep with the pin high or low, so this determines which polarity we want to trigger the wake up
    uint8_t wkupct_pin_dyn = 
        GPIO_GetPinStatus(PRESSURE_PORT, PRESSURE_PIN) 
        ? WKUPCT_PIN_POLARITY_LOW 
        : WKUPCT_PIN_POLARITY_HIGH;
    wkupct_enable_irq(WKUPCT_PIN_SELECT(PRESSURE_PORT, PRESSURE_PIN), // select pin (PRESSURE_PORT, PRESSURE_PIN)
                    WKUPCT_PIN_POLARITY(PRESSURE_PORT, PRESSURE_PIN, wkupct_pin_dyn), // polarity low or high
                    1, // 1 event
                    40); // debouncing time = 40
}

void user_app_on_init(void)
{
    bool isPressing = !GPIO_GetPinStatus(PRESSURE_PORT, PRESSURE_PIN);
    arch_printf("user_app_init() and isPressing is %s\n\r", isPressing ? "TRUE" : "FALSE");
    // this will immediatelay put the device into hibernation after powering on
    spi_flash_power_down();

    default_app_on_init();
    
    // NVIC_DisableIRQ(GPIO0_IRQn);
    // register handler
    // GPIO_RegisterCallback(GPIO0_IRQn, handle_irq);
    // set irq type
    // GPIO_EnableIRQ(PRESSURE_PORT, PRESSURE_PIN, GPIO0_IRQn, true, true, 40);
}

void app_advertise_complete(const uint8_t status)
{
    arch_printf("app_advertise_complete()\n\r");

    // this method is called after the device connects, so this initializes the advertising data
    update_adv_data();
    
    if ((status == GAP_ERR_NO_ERROR) || (status == GAP_ERR_CANCELED))
    {
        arch_printf("GAP_ERR_NO_ERROR || GAP_ERR_CANCELED\n\r");
    }

    if (status == GAP_ERR_CANCELED)
    {
        arch_printf("GAP_ERR_CANCELED\n\r");
        arch_ble_ext_wakeup_on();

        // Configure PD_TIM
        // Close PD_TIM
        SetBits16(PMU_CTRL_REG, TIM_SLEEP, 1);
        // Wait until PD_TIM is closed
        while ((GetWord16(SYS_STAT_REG) & TIM_IS_DOWN) != TIM_IS_DOWN)

        //powering down flash before entering hibernation 
        spi_flash_power_down(); 			
                
        // Put system into hibernation
        arch_set_hibernation(PRESSURE_PIN_MASK,
                                CFG_HIBERNATION_RAM1,
                                CFG_HIBERNATION_RAM2,
                                CFG_HIBERNATION_RAM3,
                                CFG_HIBERNATION_REMAP,
                                CFG_HIBERNATION_PAD_LATCH_EN);
    }
}

void on_suotar_status_change(const uint8_t suotar_event)
{
#if (!SUOTAR_SPI_DISABLE)
    uint8_t dev_id;

    // Release Flash from power down
    spi_flash_release_from_power_down();

    // Try to auto-detect the device
    spi_flash_auto_detect(&dev_id);

    if (suotar_event == SUOTAR_END)
    {
        // Power down SPI Flash
        spi_flash_power_down();
    }
#endif
}

void user_app_on_disconnect(struct gapc_disconnect_ind const *param)
{
    default_app_on_disconnect(NULL);

#if (BLE_SUOTA_RECEIVER)
    // Issue a platform reset when it is requested by the suotar procedure
    if (suota_state.reboot_requested)
    {
        // Reboot request will be served
        suota_state.reboot_requested = 0;

        // Platform reset
        platform_reset(RESET_AFTER_SUOTA_UPDATE);
    }
#endif
}

void user_app_on_db_init_complete(void) {
    /* Set application version number in DISS */
    char sw_version[] = SDK_VERSION;
    struct diss_set_value_req *req = KE_MSG_ALLOC_DYN(DISS_SET_VALUE_REQ,
                                                    prf_get_task_from_id(TASK_ID_DISS),
                                                    TASK_APP,
                                                    diss_set_value_req,
                                                    sizeof(sw_version));
    req->value = DIS_SW_REV_STR_CHAR;
    req->length = sizeof(sw_version);
    memcpy(req->data, sw_version, sizeof(sw_version));
    ke_msg_send(req);

    default_app_on_db_init_complete();
}

/// @} APP

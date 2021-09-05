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
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "custs1.h"
#include "custs1_task.h"
#include "user_custs1_def.h"
#include "app_default_handlers.h"
#include "app_easy_gap.h"
#include "gpio.h"
#include "wkupct_quadec.h"
#include "user_periph_setup.h"
#include "handleit_sensor.h"

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/


void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}

/**
 ****************************************************************************************
 * @brief Function to check sensor data and update BLE characteristic
 ****************************************************************************************
 */
void update_adv_data(void)
{
    bool isPressing = GPIO_GetPinStatus(PRESSURE_PORT, PRESSURE_PIN);

    // Show LED state
    if(isPressing) {
        GPIO_SetActive(FORCE_PORT, FORCE_HIGH_PIN);
        GPIO_SetInactive(FORCE_PORT, FORCE_LOW_PIN);
    } else {
        GPIO_SetInactive(FORCE_PORT, FORCE_HIGH_PIN);
        GPIO_SetActive(FORCE_PORT, FORCE_LOW_PIN);
    }
    
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
    req->value[0] = isPressing ? 0x1F : 0x00;
    req->value[1] = 0;
    req->value[2] = 0;
    req->value[3] = 0;

    ke_msg_send(req);
}

/**
 ****************************************************************************************
 * @brief Button press callback function. Registered in WKUPCT driver.
 ****************************************************************************************
 */
static void app_button_press_cb(void)
{
    if (arch_ble_ext_wakeup_get())
    {
        arch_set_sleep_mode(app_default_sleep_mode);
        arch_ble_force_wakeup();
        arch_ble_ext_wakeup_off();
        app_easy_wakeup();

    }
    
    update_adv_data();
}

/**
 ****************************************************************************************
 * @brief Application wakeup callback function. Registered in API message utility.
 ****************************************************************************************
*/
static void app_wakeup_cb(void)
{
    // If state is not idle, ignore the message
    if (ke_state_get(TASK_APP) == APP_CONNECTABLE)
    {
        default_advertise_operation();
    }
}

/**
 ****************************************************************************************
 * @brief Sets button as wakeup trigger
 ****************************************************************************************
*/
static void app_button_enable(void)
{
    app_easy_wakeup_set(app_wakeup_cb);
    wkupct_register_callback(app_button_press_cb);
    wkupct_enable_irq(WKUPCT_PIN_SELECT(PRESSURE_PORT, PRESSURE_PIN), // select pin (PRESSURE_PORT, PRESSURE_PIN)
                      WKUPCT_PIN_POLARITY(PRESSURE_PORT, PRESSURE_PIN, WKUPCT_PIN_POLARITY_HIGH), // polarity low or high
                                          1, // 1 event
                                          40); // debouncing time = 40
}

void user_app_on_init(void)
{
    default_app_on_init();
    GPIO_SetInactive(FORCE_PORT, FORCE_HIGH_PIN);
    GPIO_SetActive(FORCE_PORT, FORCE_LOW_PIN);
    
    // NVIC_DisableIRQ(GPIO0_IRQn);
    // register handler
    // GPIO_RegisterCallback(GPIO0_IRQn, handle_irq);
    // set irq type
    // GPIO_EnableIRQ(PRESSURE_PORT, PRESSURE_PIN, GPIO0_IRQn, true, true, 40);

    // Use wkupct instead of IRQ
    app_button_enable();    
}

arch_main_loop_callback_ret_t app_on_full_power(void)
{
    update_adv_data();
    return GOTO_SLEEP;
}

/// @} APP

#ifndef BLE_ACS_H__
#define BLE_ACS__H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// BASE UUID: 87de0001-51b5-43c3-9ccb-993004dd54aa

#define ACS_UUID_BASE                       {0xAA, 0x54, 0xDD, 0x04, 0x30, 0x99, 0xCB, 0x9C, \
                                            0xC3, 0x43, 0xB5, 0x51, 0xFF, 0xFF, 0xDE, 0x87}
#define ACS_UUID_SERVICE                    0x0001
#define ACS_UUID_ACCEL_DATA_CHAR            0x0002
#define ACS_UUID_BUTTON_CHAR                0x0003


// Forward declaration of the ble_acs_t type.
typedef struct ble_acs_s ble_acs_t;

typedef void (*ble_acs_data_subscr_handler_t) (ble_acs_t * p_acs, bool is_data_subscribed);
typedef void (*ble_acs_btn_subscr_handler_t) (ble_acs_t * p_acs, bool is_btn_subscribed);


typedef struct
{
    ble_acs_data_subscr_handler_t    data_subscr_handler;
    ble_acs_btn_subscr_handler_t     btn_subscr_handler;
} ble_acs_init_t;

/**@brief LED Button Service structure. This structure contains various status information for the service. */
struct ble_acs_s
{
    uint16_t                          service_handle;
    ble_gatts_char_handles_t          accel_data_char_handles;
    ble_gatts_char_handles_t          btn_data_char_handles;
    uint8_t                           uuid_type;
    uint16_t                          conn_handle;
    ble_acs_btn_subscr_handler_t      btn_subscr_handler;
    ble_acs_data_subscr_handler_t     data_subscr_handler;
};

uint32_t ble_acs_init(ble_acs_t * p_acs, const ble_acs_init_t * p_acs_init);

void ble_acs_on_ble_evt(ble_acs_t * p_acs, ble_evt_t * p_ble_evt);

uint32_t ble_acs_accel_data_notify(ble_acs_t * p_acs, uint8_t * p_accel_data);

uint32_t ble_acs_on_button_change(ble_acs_t * p_acs, uint8_t button_state, uint8_t button_id);


#ifdef __cplusplus
}
#endif

#endif // BLE_ACS_H__

/** @} */
